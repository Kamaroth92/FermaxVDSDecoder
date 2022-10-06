#include "intercom.h"
#include <Arduino.h>
#include <vector>
using std::vector;

IntercomDevice::IntercomDevice(uint8_t pin)
{
    this->pin = pin;
};