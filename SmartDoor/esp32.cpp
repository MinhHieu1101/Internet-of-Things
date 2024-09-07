#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

#define SERVO_PIN 14

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};
byte rowPins[ROWS] = {19, 18, 5, 17};
byte colPins[COLS] = {16, 4, 2, 15};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo myServo;

//const char* ssid = "son";
//const char* password = "0983514480";
const char* ssid = "meow meow";
const char* password = "88888888";

const int mqtt_port = 8883;
const char* mqtt_server = "2d01e70236344cbb9fb248a51633aca5.s1.eu.hivemq.cloud";
const char* mqtt_user = "esp32";
const char* mqtt_password = "Admin123";

const char* thingspeak_api_key = "F65JNC5XTMRMI9K7";

WiFiClientSecure espClient;
PubSubClient client(espClient);

String currentPassword = "";
String inputPassword = "";
int servoPosition = 0;
int servoPosition2 = 0;
unsigned long interval;

// HiveMQ Cloud Let's Encrypt CA certificate (hardcoded)
static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  espClient.setCACert(root_ca);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  myServo.attach(SERVO_PIN);
  myServo.write(0);
  interval = millis();
  lcd.init();
  lcd.backlight();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  lcd.setCursor(0, 0);
  lcd.print("Welcome Home");
  
  char key = keypad.getKey();
  if (key) {
    Serial.println(key);
    if (key == '#') {
      inputPassword = "";
    } else if (key == '*') {
      if (inputPassword == currentPassword) {
		while (servoPosition2 < servoPosition) {
          myServo.write(servoPosition2);
          delay(20);
          servoPosition2++;
        }
		
		lcd.setCursor(0, 0);
        lcd.print("Access Granted");
		servoPosition2 = servoPosition;
		publishDoorState("Unlocked", inputPassword);
        interval = millis();
		delay(1500);
		lcd.clear();
      } else {
		lcd.setCursor(0, 0);
        lcd.print("Wrong Password");
		publishDoorState("Locked", inputPassword);
        logWrongPassword(inputPassword);
		delay(1500);
		lcd.clear();
      }
      inputPassword = "";
    } else {
      inputPassword += key;
	  lcd.setCursor(inputPassword.length() - 1, 1);
	  lcd.print('*');
    }
  }
  
  if (servoPosition2 > 0 && (millis() - interval) > 5000) {
	servoPosition2 = 0;
    lcd.setCursor(0, 0);
	lcd.print("Closing the door...");
	while (servoPosition > servoPosition2) {
      myServo.write(servoPosition);
      delay(20);
      servoPosition--;
    }
    delay(1500);
	lcd.clear();
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("door/password");
	  client.subscribe("servo/control");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
	  Serial.println(" Try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (String(topic) == "door/password") {
    currentPassword = message;
    Serial.print("Current password received: ");
    Serial.println(currentPassword);
  } else if (String(topic) == "servo/control") {
    servoPosition = message.toInt();
    Serial.print("Servo position received: ");
    Serial.println(servoPosition);
  }
}

void publishDoorState(String doorState, String inputPassword) {
  String payload = "{\"door_state\":\"" + doorState + "\", \"password_input\":\"" + inputPassword + "\"}";
  client.publish("access/history", payload.c_str());
  Serial.println(" ");
}

void logWrongPassword(String passwordInput) {
  String postData = String("api_key=") + thingspeak_api_key
                    + "&field1=" + passwordInput;

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://api.thingspeak.com/update");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpResponseCode = http.POST(postData);
    if (httpResponseCode > 0) {
      Serial.print("ThingSpeak Response Code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error sending to ThingSpeak: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
}