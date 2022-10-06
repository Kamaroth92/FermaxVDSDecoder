#include "waveform.h"
#include <Arduino.h>
#include <vector>
using std::vector;

Waveform::Waveform(String name, String data)
{
    this->name = name;
    this->data = data;
};

int Waveform::get_bit(int index)
{
    return this->data[index];
};

String Waveform::get_name(){
    return this->name;
};

String Waveform::get_data(){
    return this->data;
};

void Waveform::test(){
    for (int i=0;i<this->instanceCount;i++){
        Serial.println(this->instances[i]->get_name());
    }
};