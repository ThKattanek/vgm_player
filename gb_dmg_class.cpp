//////////////////////////////////////////////////
//                                              //
// GB DMG emulation                             //
// by thorsten kattanek                         //
//                                              //
// #file: gb_dmg_class.cpp                      //
//                                              //
// last changes at 06-23-2020                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#include "gb_dmg_class.h"
#include <QDebug>

GB_DMGClass::GB_DMGClass()
{
    clockspeed = 4194304;
    samplerate = 44100;
    CalcSubCounter();

    square_duty_table[0] = 0x08;    // 12,5%
    square_duty_table[1] = 0x0C;    // 25%
    square_duty_table[2] = 0x3C;    // 50%
    square_duty_table[3] = 0xf3;    // 75%

    Reset();
}

GB_DMGClass::~GB_DMGClass()
{

}

void GB_DMGClass::SetClockSpeed(uint32_t clockspeed)
{
    this->clockspeed = clockspeed;
    CalcSubCounter();
}

void GB_DMGClass::SetSampleRate(uint32_t samplerate)
{
    this->samplerate = samplerate;
    CalcSubCounter();
}

void GB_DMGClass::WriteReg(uint8_t reg_nr, uint8_t value)
{
    switch(reg_nr + 0x10)
    {
    case 0x10:
        NR10 = value;
        break;
    case 0x11:
        NR11 = value;
        square1_length_counter = value & 0x3f;
        square1_duty = value >> 6;
        break;
    case 0x12:
        NR12 = value;
        break;
    case 0x13:
        NR13 = value;
        square1_start_counter = 2048-( NR13 | (NR14 & 0x07) << 8);
        break;
    case 0x14:
        NR14 = value;
        square1_start_counter = 2048-( NR13 | (NR14 & 0x07) << 8);
        break;
    case 0x16:
        NR21 = value;
        break;
    case 0x17:
        NR22 = value;
        break;
    case 0x18:
        NR23 = value;
        break;
    case 0x19:
        NR24 = value;
        break;
    case 0x1A:
        NR30 = value;
        break;
    case 0x1B:
        NR31 = value;
        break;
    case 0x1C:
        NR32 = value;
        break;
    case 0x1D:
        NR33 = value;
        break;
    case 0x1E:
        NR34 = value;
        break;
    case 0x20:
        NR41 = value;
        break;
    case 0x21:
        NR42 = value;
        break;
    case 0x22:
        NR43 = value;
        break;
    case 0x23:
        NR44 = value;
        break;
    case 0x24:
        NR50 = value;
        break;
    case 0x25:
        NR51 = value;
        break;
    case 0x26:
        NR52 = value;
        break;
    case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
    case 0x38: case 0x39: case 0x3A: case 0x3B: case 0x3C: case 0x3D: case 0x3E: case 0x3F:
        WAVE_TABLE[(reg_nr - 0x20)] = value;
        break;
    default:
        qDebug() << "Not Supported register!";
        break;
    }
}

float GB_DMGClass::GetNextSample()
{
    square1_counter -= sub_counter;
    if(square1_counter <= 0)
    {
        square1_counter += square1_start_counter;

        if(square_duty_table[square1_duty] & (1 << (7 - square1_wave_counter)))
        {
            square1_out = 1.0f;
        }
        else
        {
            square1_out = -1.0f;
        }

        square1_wave_counter++;
        square1_wave_counter &= 0x07;
    }

        return square1_out;
}

void GB_DMGClass::Reset()
{
    NR10=NR11=NR12=NR13=NR14=0;
    NR21=NR22=NR23=NR24=0;
    NR30=NR31=NR32=NR33=NR34=0;
    NR41=NR42=NR43=NR44=0;
    NR50=NR51=NR52=0;

    square1_wave_counter = 0;
    square1_length_counter = 0;
    square1_duty = 0;
}

void GB_DMGClass::CalcSubCounter()
{
    sub_counter = clockspeed / samplerate;
}
