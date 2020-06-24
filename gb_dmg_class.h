//////////////////////////////////////////////////
//                                              //
// GB DMG emulation                             //
// by thorsten kattanek                         //
//                                              //
// #file: gb_dmg_class.h                        //
//                                              //
// last changes at 06-23-2020                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#ifndef GB_DMGCLASS_H
#define GB_DMGCLASS_H

#include <cstdint>

class GB_DMGClass
{
public:
    GB_DMGClass();
    ~GB_DMGClass();

    void SetClockSpeed(uint32_t clockspeed);
    void SetSampleRate(uint32_t samplerate);
    void WriteReg(uint8_t reg_nr, uint8_t value);
    float GetNextSample();

private:
    void Reset();
    void CalcSubCounter();

    uint32_t clockspeed;        // as Hz
    uint32_t samplerate;        // as Hz
    uint16_t sub_counter;

    // Registers for Square 1
    uint8_t NR10,NR11,NR12,NR13,NR14;

    // Registers for Square 2
    uint8_t NR21,NR22,NR23,NR24;

    // Registers for Wave
    uint8_t NR30,NR31,NR32,NR33,NR34;

    // Registers for Noise
    uint8_t NR41,NR42,NR43,NR44;

    // Registers for Controlls
    uint8_t NR50,NR51,NR52;

    // Wavetable 16 Byte for 32 4-Bit Samples
    uint8_t WAVE_TABLE[32];

    // Internal Registers
    uint8_t square_duty_table[4];

    // Squre1
    uint16_t square1_start_counter;
    uint8_t square1_duty;

    int16_t square1_counter;
    uint8_t square1_wave_counter;

    uint8_t square1_length_counter;

    float square1_out;

    // Squre2
    uint16_t square2_start_counter;
    uint8_t square2_duty;

    int16_t square2_counter;
    uint8_t square2_wave_counter;

    uint8_t square2_length_counter;

    float square2_out;
};

#endif // GB_DMGCLASS_H
