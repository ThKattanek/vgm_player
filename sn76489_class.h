//////////////////////////////////////////////////
//                                              //
// SN76489 emulation                            //
// by thorsten kattanek                         //
//                                              //
// #file: sn76489_class.h                       //
//                                              //
// last changes at 02-13-2020                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#ifndef SN76489CLASS_H
#define SN76489CLASS_H

#include <cstdint>

class SN76489Class
{
public:
    SN76489Class();
    ~SN76489Class();

    void SetClockSpeed(uint32_t clockspeed);
    void SetSampleRate(uint32_t samplerate);
    void WriteReg(uint8_t value);
    float GetNextSample();

private:
    void CalcSubCounter();

    uint32_t clockspeed;    // as Hz
    uint32_t samplerate;    // as Hz
    uint16_t sub_counter;

    int16_t tone1_counter;
    int16_t tone2_counter;
    int16_t tone3_counter;

    uint16_t tone1_freq;
    uint16_t tone2_freq;
    uint16_t tone3_freq;

    float   tone1_output;
    float   tone2_output;
    float   tone3_output;

    uint16_t freq_latch;
    uint8_t is_wait_second_byte;   // 0=no / 1=yes - tone1 / 2=yes - tone2 / 3=yes - tone3
};

#endif // SN76489CLASS_H
