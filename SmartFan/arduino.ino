#include <LiquidCrystal.h>
#include <DHT.h>

#define DHTPIN 8
#define DHTTYPE DHT11
#define LM35PIN A5
#define FANS A4

DHT dht(DHTPIN, DHTTYPE);

const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 7, d7 = 6;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
	pinMode(FANS, OUTPUT);
	analogWrite(FANS, 0);
	Serial.begin(9600);
	dht.begin();
	lcd.begin(16, 2);
}

void loop() {
	delay(2000);
	
	float hum = dht.readHumidity();
	//convert to celsius degree
	float temp = (analogRead(LM35PIN) * 5.0 / 1023.0) * 100.0;

	//update LCD with current temperature and humidity
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print(" Temp   |    RH");
	lcd.setCursor(0,1);
	lcd.print(temp);
	lcd.print("C | ");
	lcd.print(hum);
	lcd.print("%");
	
	delay(2000);

	//send data to Serial
	Serial.print(temp);
	Serial.print(",");
	Serial.println(hum);

	delay(2000);
	
	//check incoming Serial data
	if (Serial.available() > 0) {
		String action = Serial.readStringUntil('n');
		//extract the bytes data
		String mode = action.substring(0, 5);
		String today = action.substring(5);

		//control for the fan
		if (mode == "mode1") {
		  analogWrite(FANS, 250);
		  lcd.setCursor(0,0);
		  lcd.print("Fan ON");
		  delay(2000);
		  lcd.setCursor(0,0);
		  lcd.print(today);
		  delay(3000);
		}
		else if (mode == "mode2") {
		  analogWrite(FANS, 0);
		  lcd.setCursor(0,0);
		  lcd.print("Fan OFF");
		  delay(2000);
		  lcd.setCursor(0,0);
		  lcd.print(today);
		  delay(3000);
		}
	}

	//delay between sensor data reads
	delay(5000);
}