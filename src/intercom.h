#ifndef INTERCOM_CONTROLLER_H
#define INTERCOM_CONTROLLER_H

#include <Arduino.h>
#include <vector>

using std::vector;

class IntercomDevice
{
public:
    IntercomDevice(uint8_t pin, int baud_ms);
    bool active;
    bool dump_data;
    int baud_ms;
    uint8_t pin;
    unsigned long long int adjusted_time(unsigned long long int time);
};

class FrameDataPoint
{
public:
    FrameDataPoint(byte state, unsigned long long int last_changed_time, unsigned long long int rel_time);
    byte state;
    unsigned long int last_changed_time;
    unsigned long int rel_time;
};

class FrameData
{
    unsigned long long int start_time;
    unsigned long long int last_changed_time;
    int baud_ms;
    int high_pulse_stop_count = 10;
    // unsigned long int adjusted_relative_time;


public:
    FrameData(int baud_ms);
    vector<FrameDataPoint> data;
    bool frame_elapsed();
    unsigned long long int show_start_time();
    unsigned long long int show_last_changed_time();
    unsigned long int relative_time();
    void add_data_point(byte state);
    void start();
    void clear();
};

#endif /* INTERCOM_CONTROLLER_H */
