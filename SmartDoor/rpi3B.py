import paho.mqtt.client as mqtt
import mysql.connector
import json

db_host = "localhost"
db_user = "admin"
db_pass = "admin"
db_name = "assignment3_smarthome"

mqtt_broker = "2d01e70236344cbb9fb248a51633aca5.s1.eu.hivemq.cloud"
mqtt_port = 8883
mqtt_user = "rpi3B"
mqtt_password = "Admin123"
mqtt_topic_door_password = "door/password"
mqtt_topic_servo_control = "servo/control"
mqtt_topic_access_history = "access/history"

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected successfully")
        client.subscribe(mqtt_topic_access_history)
        publish_servo_position_and_password(client)
    else:
        print(f"Connection failed with code {rc}")

def on_message(client, userdata, msg):
    payload = json.loads(msg.payload.decode())
    door_state = payload["door_state"]
    password_input = payload["password_input"]
    insert_access_history(door_state, password_input)
    publish_servo_position_and_password(client)

def insert_access_history(door_state, password_input):
    conn = mysql.connector.connect(host=db_host, user=db_user, password=db_pass, database=db_name)
    cursor = conn.cursor()
    query = "INSERT INTO AccessHistory (timestamp, door_state, password_input) VALUES (NOW(), %s, %s)"
    cursor.execute(query, (door_state, password_input))
    print("A new attempt to open the door has been registered!")
    conn.commit()
    cursor.close()
    conn.close()

def publish_servo_position_and_password(client):
    conn = mysql.connector.connect(host=db_host, user=db_user, password=db_pass, database=db_name)
    cursor = conn.cursor()
    cursor.execute("SELECT password FROM DoorPassword WHERE id=1")
    current_password = cursor.fetchone()[0]
    client.publish(mqtt_topic_door_password, current_password)
    cursor.execute("SELECT servo_position FROM ServoControl WHERE id=1")
    servo_position = cursor.fetchone()[0]
    client.publish(mqtt_topic_servo_control, servo_position)
    cursor.close()
    conn.close()

client = mqtt.Client()
client.username_pw_set(mqtt_user, mqtt_password)
#Let's Encrypt CA certificate
client.tls_set("isrgrootx1.pem")
client.on_connect = on_connect
client.on_message = on_message

client.connect(mqtt_broker, mqtt_port, 60)

client.loop_forever()