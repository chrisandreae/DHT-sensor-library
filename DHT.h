#ifndef DHT_H
#define DHT_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/* DHT library
   MIT license
   In part (c) 2012 Chris Andreae
   Includes portions written by Adafruit Industries
*/

class DHT {
 private:
  uint8_t _pin;

 public:
  DHT(uint8_t pin);

  uint8_t read(void);

  // fixed point temperature and humidity (8,8)
  uint16_t humidity;
  uint16_t temperature;
};
#endif
