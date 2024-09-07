import serial
import MySQLdb
from datetime import datetime
import time

#database configuration
db = MySQLdb.connect(host='localhost',
                     user='admin',
                     passwd='admin',
                     db='IoT_SU',
                     charset='utf8mb4')

ser = serial.Serial('/dev/ttyS2', 9600)

def fan_control(temp, threshold):
    current_date = datetime.now().strftime('%a %d/%m')
    #compare the temp from the sensor with the threshold on the database
    if temp >= threshold:
        action = 'mode1' + current_date
    else:
        action = 'mode2' + current_date
    ser.write(action.encode() + b'n')

try:
    while True:
        time.sleep(2)
        line = ser.readline().decode('utf-8').rstrip()
        
        split_values = line.split(",")

        if len(split_values) == 2:
            temp = float(split_values[0])
            hum = float(split_values[1])

        print(f"Temperature: {temp}, Humidity: {hum}")

        cursor = db.cursor()
        #insert into the sensor data table
        cursor.execute("INSERT INTO sensor_data (temperature, humidity, recorded_date) VALUES (%s, %s, NOW())", (temp, hum))

        #fetch the threshold of the fan
        cursor.execute("SELECT threshold FROM fan_threshold")
        threshold = cursor.fetchone()[0]
        
        fan_control(temp, threshold)
        
        print("New sensor data added to the database")
        
        db.commit()
        cursor.close()
        time.sleep(1)
except KeyboardInterrupt:
    print("Script interrupted by the user")
except MySQLdb.Error as db_error:
    print(f"Error while trying to insert data into MySQL database: {db_error}")
finally:
    db.close()