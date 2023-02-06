#include <Arduino.h>
#include "waveform.h"
#include "intercom.h"
#include <vector>
using std::vector;

#define PULSE_LENGTH 833     // microseconds; equals a baud of 1200bps
#define INTERCOM 19
#define DEBUG_OUTPUT 18

IntercomDevice intercom(INTERCOM, PULSE_LENGTH);
FrameData frame(intercom.baud_ms);
vector<FrameData> FrameList;
vector<Waveform> WaveformList;
//                             |---CMD?---||-APT?-|
// Waveform elevator("elevator", "01010001001001100000");
void IRAM_ATTR isr()
{
    digitalWrite(DEBUG_OUTPUT, LOW);
    unsigned long long int curr_time = esp_timer_get_time();
    if (intercom.active == false)
    {
        intercom.active = true;
        intercom.dump_data = true;
        frame.start();
    }
    byte state;

    if (digitalRead(intercom.pin) == HIGH)
    {
        state = 1;
    }
    else
    {
        state = 0;
    }

    frame.add_data_point(state);
    delayMicroseconds(5);
}

void setup()
{
    Serial.begin(115200);
    WaveformList.push_back(Waveform("apt", "01011000101001100010"));
    WaveformList.push_back(Waveform("elevator", "01010001001001100000"));
    attachInterrupt(intercom.pin, isr, CHANGE);
    pinMode(DEBUG_OUTPUT, OUTPUT);
    digitalWrite(DEBUG_OUTPUT, HIGH);
}

void loop()
{
    if (intercom.active && frame.frame_elapsed())
    {
        intercom.active = false;
        digitalWrite(DEBUG_OUTPUT, HIGH);
    }
    if (!intercom.active && intercom.dump_data)
    {
        intercom.dump_data = false;
        if (frame.data.size() > 1)
        {
            // Calculate datapoints
            vector<int> datapoints;
            datapoints.clear();
            for (int i = 0; i < (frame.data.size() - 1); i++)
            {
                FrameDataPoint current_datapoint = frame.data[i];
                int current_adj_time = intercom.adjusted_time(current_datapoint.rel_time);
                FrameDataPoint next_datapoint = frame.data[i + 1];
                int next_adj_time = intercom.adjusted_time(next_datapoint.rel_time);

                int num_data_points = (next_adj_time - current_adj_time) / intercom.baud_ms;
                for (int n = 0; n < (num_data_points); n++)
                {
                    datapoints.push_back(current_datapoint.state);
                    // counter++;
                }
            }
            Serial.print("Start time: ");
            Serial.print(frame.show_start_time());
            Serial.println(" microseconds");
            Serial.print("Frame stopped at: ");
            Serial.print(esp_timer_get_time());
            Serial.println(" microseconds");
            Serial.print("Time since last frame: ");
            if (FrameList.size() == 0)
            {
                Serial.print("0");
            }
            else
            {
                unsigned long long int previous_last_changed_time = FrameList.back().show_last_changed_time();
                Serial.print((frame.show_start_time() - previous_last_changed_time) / 1000ULL);
            }
            Serial.println(" milliseconds");
            Serial.print("Total frames captured: ");
            Serial.println(FrameList.size() + 1);
            Serial.print("Datapoints in frame: ");
            Serial.println(datapoints.size());

            int timer = 0;
            for (int state : datapoints)
            {
                if (state == 0)
                {
                    Serial.print("  |    ");
                }
                else
                {
                    Serial.print("    |  ");
                }
                Serial.print("Time: ");
                Serial.print(timer);
                Serial.print(", State: ");
                Serial.println(state);
                timer += intercom.baud_ms;
            }
            Serial.println("Comparing against known waveforms");
            // Pattern matching
            bool is_matched = false;
            for (Waveform t_waveform : WaveformList)
            {
                if (is_matched){
                    continue;
                }
                Serial.print("  ...checking ");
                Serial.print(t_waveform.get_name());
                Serial.print(", ");
                if (t_waveform.get_data().length() != datapoints.size())
                {
                    
                    Serial.println("length doesn't match");
                    continue;
                }
                bool match = true;
                int counter = 0;
                for (int i = 0; i < datapoints.size(); i++)
                {
                    // Serial.println();
                    if (match && String(datapoints[i]) != String(t_waveform.get_data()[i]))
                    {
                        Serial.println("bits don't match");
                        match = false;
                    }
                }
                if (match){
                    Serial.println("waveform matches ");
                    is_matched = true;
                }
            }
            FrameData current_frame = frame;
            FrameList.push_back(current_frame);
            Serial.println("------------------------------------------");
        }
        // else
        // {
        //     Serial.println("Frame too small, skipping...");
        // }
        frame.clear();
    }
    delay(10);
}

// TODO: WAVEFORM MATCHING STUFF!