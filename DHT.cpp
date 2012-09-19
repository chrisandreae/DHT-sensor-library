/* DHT library

   MIT license
   In part (c) 2012 Chris Andreae
   Includes portions written by Adafruit Industries
*/

#include "DHT.h"

DHT::DHT(uint8_t pin) {
	_pin = pin;
}

static uint8_t waitFor(uint8_t val, uint8_t pin, uint8_t timeout_us){
	uint32_t start = micros();
	uint32_t timeout_t = start + timeout_us;
	while(digitalRead(pin) != val){
		if(micros() > timeout_t) return 0xff;
	}
	return micros() - start;
}
#define WAIT_FOR(val, timeout, error) ({				\
			uint8_t __r = waitFor(val, _pin, timeout);	\
			if(__r == 0xff) return error;				\
			__r;										\
		})

/** May read only once every 2 seconds or so. This is not enforced. */
uint8_t DHT::read(void) {
  uint8_t data[5] = {0}; // read buffer
  uint8_t data_i = 0, data_b = 0;

  digitalWrite(_pin, LOW); // externally pulled up: don't use internal pullup

  // now pull it low for ~20 milliseconds
  pinMode(_pin, OUTPUT);
  delay(20);
  pinMode(_pin, INPUT);
  delayMicroseconds(40);

  // sensor will respond by pulling low for 80us and high for 80us
  WAIT_FOR(HIGH, 90, 1);
  WAIT_FOR(LOW,  90, 2);

  for(int i = 0; i < 40; ++i){
	  uint8_t* byte = &data[i/8];
	  *byte <<= 1;

	  // then each bit is 50us of low, and then either 25 or 70 us of high (0/1)
	  WAIT_FOR(HIGH, 60, 3);
	  uint8_t elapsed = WAIT_FOR(LOW, 90, 4);
	  if(elapsed > 40){
		  *byte |= 0x1;
	  }
  }

  // check we read 40 bits and that the checksum matches
  if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
	  humidity    = (data[0] << 8) | data[1];
	  temperature = (data[2] << 8) | data[3];
	  return 0;
  }
  else{
	  return 5;
  }
}
