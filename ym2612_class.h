//////////////////////////////////////////////////
//                                              //
// YM2612 emulation                             //
// by thorsten kattanek                         //
//                                              //
// #file: ym2612_class.h                        //
//                                              //
// last changes at 02-29-2020                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#ifndef YM2612CLASS_H
#define YM2612CLASS_H

#include <cstdint>

class YM2612Class
{
public:
    YM2612Class();
    ~YM2612Class();

    void SetClockSpeed(uint32_t clockspeed);
    void SetSampleRate(uint32_t samplerate);
    void WriteRegPort0(uint8_t reg, uint8_t value);
    void WriteRegPort1(uint8_t reg, uint8_t value);
    float GetNextSample();
};

#endif // YM2612CLASS_H
