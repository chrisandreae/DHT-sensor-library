// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, Chris Andreae, public domain

#include "DHT.h"
#include "fixpt.h"

#define DHTPIN 7     // what pin we're connected to

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN);

void setup() {
  Serial.begin(9600);
  Serial.println("DHTxx test!");
}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // The sensor should not be read more often than every second
  // (DHT11) or 1.7 seconds (DHT22).
  unsigned char err = dht.read();
  // Check if correctly read: returns 0 if successful or an error code
  if(err){
	Serial.print("Failed to read from DHT: ");
	Serial.println(err);
  }
  else{
	char hbuf[7], tbuf[7];
	// The sensor returns a 16-bit fixed point number with 8 bits of
	// precision. For a DHT11, the fractional part is always zero, so
	// you can save yourself the space and time of fixed point
	// calculations by right-shifting by 8 to obtain the integer part.
	fixpt_str(dht.humidity,    8, hbuf, sizeof(hbuf), 1);
	fixpt_str(dht.temperature, 8, tbuf, sizeof(tbuf), 1);

	Serial.print("DHT: Hum: ");
	Serial.print(hbuf);
	Serial.print("% Temp: ");
	Serial.print(tbuf);
	Serial.println("*C");
  }

  delay(2000);
}
