#pragma once
#ifndef SwartNinjaLED_H
#define SwartNinjaLED_H

#include <Arduino.h>

class SwartNinjaLED
{
public:
  SwartNinjaLED(int pin);
  void setup(void);

  char *getState(void);
  bool getRawState(void);
  bool setState(bool state);

  char *getBrightness(void);
  bool setBrightness(uint8_t value);

  uint16_t getColorTemperature(void);
  bool setColorTemperature(uint16_t colorTemperature);

private:
  int _pin;
  bool _state;
  uint8_t _brightness;
  void _updateLED(void);
};

#endif