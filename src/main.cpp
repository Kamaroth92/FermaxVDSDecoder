#include <Arduino.h>
#include "waveform.h"
#include "intercom.h"
#include <vector>
using std::vector;

#define PULSE_LENGTH 833    // microseconds; equals a baud of 1200bps
#define PULSE_COUNT_DELAY 6 // count
#define INTERCOM 19
#define DEBUG_OUTPUT 18

IntercomDevice intercom(INTERCOM);
vector<Waveform> WaveformList;
//                                          |-APT-|
Waveform elevator("elevator", "01010001001001100000");

// void IRAM_ATTR isr()
// {
//   unsigned long long int curr_time = esp_timer_get_time();
//   if (frame.active == false)
//   {
//     frame.active = true;
//     frame.dump_data = true;
//     frame.start_time = curr_time;
//     frame.index = 0;
//   }

//   frame.last_changed_time = curr_time;
//   byte state;
//   if (digitalRead(frame.PIN) == HIGH)
//   {
//     state = 1;
//   }
//   else
//   {
//     state = 0;
//   }

//   unsigned long int rel_time = curr_time - frame.start_time;
//   float divisor = rel_time;
//   rel_time = (round(divisor / PULSE_LENGTH) * PULSE_LENGTH);
//   if (frame.index == 0 || frame.data[frame.index - 1].state != state)
//   {
//     frame.data[frame.index] = {rel_time, state};
//     frame.index++;
//   }
//   delayMicroseconds(5);
// }

void setup()
{
    Serial.begin(115200);
    WaveformList.push_back(elevator);


}

void loop()
{
//     std::vector<int> vect;
//     vect.push_back(10);
    // elevator.test();
    // for (int i = 0; i < elevator.get_size(); i++)
    // {
    //     Serial.println(elevator.get_bit(i));
    // }
    for (Waveform waveform : WaveformList){
        Serial.print(waveform.get_name());
        Serial.print(": ");
        String t_data = waveform.get_data();
        // Serial.print();
        
        for (int i; i<t_data.length(); i++){
            Serial.print(t_data[i]);
        }
        // for (String y : x.get_data()){
        //     Serial.print(char(y)));
        //     Serial.print("-");
        // }
        Serial.println("");
    }
    Serial.println("");
    delay(1000);
}