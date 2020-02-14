//////////////////////////////////////////////////
//                                              //
// SN76489 emulation                            //
// by thorsten kattanek                         //
//                                              //
// #file: sn76489_class.cpp                     //
//                                              //
// last changes at 02-13-2020                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#include "sn76489_class.h"

SN76489Class::SN76489Class()
{
    clockspeed = 4000000;
    samplerate = 44100;

    tone1_output = 0.1f;
    tone2_output = 0.1f;
    tone3_output = 0.1f;

    tone1_freq = 0;
    tone2_freq = 0;
    tone3_freq = 0;

    tone1_attenuation = 0.0f;
    tone2_attenuation = 0.0f;
    tone3_attenuation = 0.0f;

    is_wait_second_byte = 0;
}

SN76489Class::~SN76489Class()
{

}

void SN76489Class::SetClockSpeed(uint32_t clockspeed)
{
    this->clockspeed = clockspeed;
    CalcSubCounter();
}

void SN76489Class::SetSampleRate(uint32_t samplerate)
{
    this->samplerate = samplerate;
    CalcSubCounter();
}

void SN76489Class::WriteReg(uint8_t value)
{
    switch (value & 0xf0)
    {
    case 0x80:  // Tone 1 Frequency first byte
        freq_latch = value & 0x0f;
        is_wait_second_byte = 1;
        break;
    case 0x90:  // Tone 1 Attenuation
        tone1_attenuation = volume_table[value & 0x0f];
        break;
    case 0xa0:  // Tone 2 Frequency first byte
        freq_latch = value & 0x0f;
        is_wait_second_byte = 2;
        break;
    case 0xb0:  // Tone 2 Attenuation
        tone2_attenuation = volume_table[value & 0x0f];
        break;
    case 0xc0:  // Tone 3 Frequency first byte
        freq_latch = value & 0x0f;
        is_wait_second_byte = 3;
        break;
    case 0xd0:  // Tone 3 Attenuation
        tone3_attenuation = volume_table[value & 0x0f];
        break;
    case 0xe0:  // Noise Control
        break;
    case 0xf0:  // Noise Attenuation
        break;

    default:  // Tone 1,2,3 Frequency second byte
        freq_latch |= static_cast<uint16_t>(value) << 4;
        switch(is_wait_second_byte)
        {
         case 1:
            tone1_freq = freq_latch;
            break;
        case 2:
           tone2_freq = freq_latch;
           break;
        case 3:
           tone3_freq = freq_latch;
           break;
        }

        is_wait_second_byte = 0;
        break;
    }
}

float SN76489Class::GetNextSample()
{
    tone1_counter -= sub_counter;
    if(tone1_counter <= 0)
    {
        tone1_counter += tone1_freq;
        tone1_output *= -1.0f;
    }

    tone2_counter -= sub_counter;
    if(tone2_counter <= 0)
    {
        tone2_counter += tone2_freq;
        tone2_output *= -1.0f;
    }

    tone3_counter -= sub_counter;
    if(tone3_counter <= 0)
    {
        tone3_counter += tone3_freq;
        tone3_output *= -1.0f;
    }

    return  tone1_output * tone1_attenuation + tone2_output * tone2_attenuation + tone3_output * tone3_attenuation;
}

void SN76489Class::CalcSubCounter()
{
    sub_counter = (clockspeed >> 4) / samplerate;
}
