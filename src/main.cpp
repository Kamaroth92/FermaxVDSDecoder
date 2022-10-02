#include <Arduino.h>

#define PULSE_LENGTH 835    // microseconds
#define PULSE_COUNT_DELAY 6 // count
#define INTERCOM 19
#define DEBUG_OUTPUT 18

struct Waveform
{
  char *name;
  uint8_t id;
  uint8_t frame_count;
  uint8_t data[20];
  uint8_t next_frame;
};

Waveform elevator = {"elevator call", 1, 1, {0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0}, 0};

uint8_t frame_count;
unsigned long long int prev_frame_end_time;

struct FDatapoint
{
  unsigned long time;
  uint8_t state;
  unsigned long t_time;
};

struct Frame_s
{
  const uint8_t PIN;
  unsigned long long int start_time;
  unsigned long long int last_changed_time;
  bool active;
  bool dump_data;
  uint8_t index;
  FDatapoint data[30];
} Frame_default = {INTERCOM, 0, 0, false, false, 0};
typedef struct Frame_s Frame;

Frame frame = Frame_default;
void IRAM_ATTR isr()
{
  unsigned long long int curr_time = esp_timer_get_time();
  if (frame.active == false)
  {
    frame.active = true;
    frame.dump_data = true;
    frame.start_time = curr_time;
    frame.index = 0;
  }

  frame.last_changed_time = curr_time;
  byte state;
  if (digitalRead(frame.PIN) == HIGH)
  {
    state = 1;
  }
  else
  {
    state = 0;
  }

  unsigned long int rel_time = curr_time - frame.start_time;
  float divisor = rel_time;
  rel_time = (round(divisor / PULSE_LENGTH) * PULSE_LENGTH);
  if (frame.index == 0 || frame.data[frame.index - 1].state != state)
  {
    frame.data[frame.index] = {rel_time, state};
    frame.index++;
  }
  delayMicroseconds(5);
}

void setup()
{
  Serial.begin(115200);
  pinMode(DEBUG_OUTPUT, OUTPUT);
  digitalWrite(DEBUG_OUTPUT, HIGH);
  pinMode(frame.PIN, INPUT);
  attachInterrupt(frame.PIN, isr, CHANGE);
  frame_count = 1;
}

void loop()
{
  if (frame.active && esp_timer_get_time() - frame.last_changed_time > (PULSE_COUNT_DELAY * PULSE_LENGTH))
  {
    frame.active = false;
  }
  if (!frame.active && frame.dump_data)
  {
    frame.dump_data = false;
    if (frame.index > 1)
    {
      Serial.print("Frame: ");
      Serial.println(frame_count);

      Serial.print("Changes: ");
      Serial.println(frame.index);

      Serial.print("Time since last frame: ");
      if (frame_count == 1)
      {
        Serial.print("0");
      }
      else
      {
        Serial.print((frame.start_time - prev_frame_end_time) / 1000ULL);
      }
      Serial.println(" milliseconds");

      Serial.print("Total Length: ");
      Serial.print(frame.last_changed_time - frame.start_time);
      Serial.println(" microseconds");

      byte counter = 0;
      bool matches = true;
      for (int i = 0; i < frame.index - 1; i++)
      {
        FDatapoint fd = frame.data[i];
        FDatapoint n_fd = frame.data[i + 1];
        byte pulses = (n_fd.time - fd.time) / PULSE_LENGTH;

        for (int j = 0; j < pulses; j++)
        {
          if (fd.state == 0)
          {
            Serial.print("|   ");
          }
          else
          {
            Serial.print("  | ");
          }
          Serial.print(counter + 1);
          Serial.print(", Time: ");
          Serial.print(fd.time + (j * PULSE_LENGTH));
          Serial.print(", State: ");
          Serial.println(fd.state);

          // Match against known waveforms
          if (elevator.data[counter] != fd.state)
          {
            matches = false;
          }
          counter++;
        }
      }
      if (matches && counter-1 == 20)
      {
        Serial.println("matched signal: elevator_call");
      }
      Serial.println("--------------------------------");
      prev_frame_end_time = frame.last_changed_time;
      frame_count++;
    }
    else
    {
      Serial.println("Packet too small, skipping...");
    }
  }
}