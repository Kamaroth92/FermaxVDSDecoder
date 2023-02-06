#include "intercom.h"
#include <Arduino.h>
#include <vector>
using std::vector;

IntercomDevice::IntercomDevice(uint8_t pin, int baud_ms)
{
    pinMode(pin, INPUT);
    this->pin = pin;
    this->baud_ms = baud_ms;
};

unsigned long long int IntercomDevice::adjusted_time(unsigned long long int time)
{
    float ftime = time;
    unsigned long long int adjusted_time = (round(ftime / this->baud_ms) * this->baud_ms);
    return adjusted_time;
};

//---------------------------------------------------------------------

FrameDataPoint::FrameDataPoint(byte state, unsigned long long int last_changed_time, unsigned long long int rel_time)
{
    this->state = state;
    this->last_changed_time = last_changed_time;
    this->rel_time = rel_time;
};

//---------------------------------------------------------------------

FrameData::FrameData(int baud_ms)
{
    this->baud_ms = baud_ms;
};

unsigned long long int FrameData::show_start_time()
{
    return this->start_time;
};

unsigned long long int FrameData::show_last_changed_time()
{
    return this->last_changed_time;
};

// void FrameData::add_data_point(FrameDataPoint datapoint)
void FrameData::add_data_point(byte state)
{
    if (this->data.size() == 0 || state != this->data.back().state)
    {
        this->last_changed_time = esp_timer_get_time();
        this->data.push_back(FrameDataPoint(state, this->last_changed_time, this->relative_time()));
    }
};

unsigned long int FrameData::relative_time()
{
    return this->last_changed_time - this->start_time;
};

bool FrameData::frame_elapsed()
{
    if (this->data.size() > 0 && this->data.back().state == 1 &&
        (esp_timer_get_time() - this->last_changed_time) > (this->baud_ms * this->high_pulse_stop_count))
    {
        return true;
    }
    else
    {
        return false;
    }
};

void FrameData::start()
{
    unsigned long long int curr_time = esp_timer_get_time();
    this->start_time = curr_time;
    this->last_changed_time = curr_time;
};

void FrameData::clear()
{
    this->data.clear();
    this->last_changed_time = 0;
    this->start_time = 0;
};
