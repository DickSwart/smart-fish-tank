#include "SwartNinjaLED.h"

char brightnessValue[20];

///////////////////////////////////////////////////////////////////////////
//   CONSTRUCTOR, INIT()
///////////////////////////////////////////////////////////////////////////
SwartNinjaLED::SwartNinjaLED(int pin)
{
  this->_pin = pin;
}

void SwartNinjaLED::setup(void)
{

  analogWriteRange(255);

  pinMode(this->_pin, OUTPUT);

  // set initial state
  this->_state = false;
  this->_brightness = 255;
}

///////////////////////////////////////////////////////////////////////////
//   STATE
///////////////////////////////////////////////////////////////////////////

char *SwartNinjaLED::getState(void)
{
  return strdup((this->_state) ? "ON" : "OFF");
}

bool SwartNinjaLED::getRawState(void)
{
  return this->_state;
}

bool SwartNinjaLED::setState(bool state)
{
  // checks if the given state is different from the actual state
  if (state == this->_state)
    return false;

  // saves the new state value
  this->_state = state;

  // update the LED
  this->_updateLED();

  return true;
}

///////////////////////////////////////////////////////////////////////////
//   BRIGHTNESS
///////////////////////////////////////////////////////////////////////////
char *SwartNinjaLED::getBrightness(void)
{
  snprintf(brightnessValue, 20, "%d", this->_brightness);
  return brightnessValue;
}

bool SwartNinjaLED::setBrightness(uint8_t value)
{
  // checks if the value is smaller, bigger or equal to the actual brightness value
  if (value < 0 || value > 255 || value == _brightness)
    return false;

  // saves the new brightness value
  this->_brightness = value;

  // update the LED
  this->_updateLED();

  return true;
}

///////////////////////////////////////////////////////////////////////////
//   PRIVATE METHODS
///////////////////////////////////////////////////////////////////////////

void SwartNinjaLED::_updateLED()
{
  if (this->_state)
  {
    analogWrite(this->_pin, this->_brightness);
  }
  else
  {
    analogWrite(this->_pin, 0);
  }
}
