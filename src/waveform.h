#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <Arduino.h>
#include <vector>
using std::vector;

class Waveform
{
    String name;
    String data;

public:
    Waveform(String name, String data);
    int get_bit(int index);
    String get_name();
    String get_data();
    void test();

private:
    void setup();
    size_t instanceCount;
    Waveform* instances[30];
};

#endif /* WAVEFORM_H */