
#include <Arduino.h>
#include <vector>
#ifndef INTERCOM_CONTROLLER_H
#define INTERCOM_CONTROLLER_H

using std::vector;

class IntercomDevice
{
public:
    IntercomDevice(uint8_t pin);
    unsigned long long int start_time;
    unsigned long long int last_changed_time;
    bool active;
    bool dump_data;
    vector<int> data;

private:
    uint8_t pin;
};

#endif /* INTERCOM_CONTROLLER_H */


// struct FDatapoint
// {
//   unsigned long time;
//   uint8_t state;
//   unsigned long t_time;
// };

// struct Frame_s
// {
//   const uint8_t PIN;
//   unsigned long long int start_time;
//   unsigned long long int last_changed_time;
//   bool active;
//   bool dump_data;
//   uint8_t index;
//   FDatapoint data[30];
// } Frame_default = {INTERCOM, 0, 0, false, false, 0};