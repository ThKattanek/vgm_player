//////////////////////////////////////////////////
//                                              //
// VGM Player with QT                           //
// by thorsten kattanek                         //
//                                              //
// #file: nes_apu_class.cpp                     //
//                                              //
// last changes at 05-05-2023                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#include "nes_apu_class.h"

#include <QDebug>

NES_APUClass::NES_APUClass()
{
    clockspeed = 1789772;
	samplerate = 44100;
	CalcSubCounter();

    square_duty_table[0] =0x40; // 12,5%
    square_duty_table[1] =0x60; // 25%
    square_duty_table[2] =0x78; // 50%
    square_duty_table[3] =0x9f; // 25% neg

	Reset();
}

void NES_APUClass::SetClockSpeed(uint32_t clockspeed)
{
	this->clockspeed = clockspeed;
	CalcSubCounter();
}

void NES_APUClass::SetSampleRate(uint32_t samplerate)
{
	this->samplerate = samplerate;
	CalcSubCounter();
}

void NES_APUClass::Reset()
{   
    /// Registers
    rct1 = rct2 = 0;
    low_frequency_timer_control = 0;

    /// Intern Counters
    counter_ch1 = counter_ch2 = 0;
    counter_reload_ch1 = counter_reload_ch2 = 1;

    volume_ch1 = volume_ch2 = 0.0f;

    counter_frame_sequencer = 1.0f;
}

void NES_APUClass::WriteReg(uint8_t reg_nr, uint8_t value)
{
    switch(reg_nr)
    {
    case 0x00:
        duty1 = value >> 6;
        envelope1 = value & 0x3f;
        if(envelope1 & 0x10)
            volume_ch1 = ((envelope1 & 0x0f) / 15.0f) * 0.25f;
        else
            volume_ch1 = 0.0f;
        qDebug() << "Write 0x4000 - 0x" << Qt::hex << envelope1;
        break;
    case 0x02:
        rct1 = (rct1 & 0xff00) | value;
        counter_ch1 = counter_reload_ch1 = rct1;
        break;
    case 0x03:
        rct1 = (rct1 & 0xf8ff) | (value << 8);
        counter_ch1 = counter_reload_ch1 = rct1;
        sequencer_counter_ch1 = 0;
        break;
    case 0x04:
        duty2 = value >> 6;
        envelope2 = value & 0x3f;
        if(envelope2 & 0x10)
            volume_ch2 = ((envelope2 & 0x0f) / 15.0f) * 0.25f;
        else
            volume_ch2 = 0.0f;
        qDebug() << "Write 0x4004 - 0x" << Qt::hex << envelope2;
        break;
    case 0x06:
        rct2 = (rct2 & 0xff00) | value;
        counter_ch2 = counter_reload_ch2 = rct2;
        break;
    case 0x07:
        rct2 = (rct2 & 0xf8ff) | (value << 8);
        counter_ch2 = counter_reload_ch2 = rct2;
        sequencer_counter_ch2 = 0;
        break;
    case 0x17:
        low_frequency_timer_control = value;
        counter_frame_sequencer = 1.0f;
        frame_div_counter = 0;
        break;
    default:
        break;
    }
}

void NES_APUClass::CalcNextSample()
{
    counter_ch1 -= sub_counter_ch1;
    if(counter_ch1 <= 0.0f)
    {
        counter_ch1 += counter_reload_ch1;

        if(square_duty_table[duty1] & (1 << (sequencer_counter_ch1)))
            ch1 = volume_ch1;
        else
            ch1 = 0.0f;

        sequencer_counter_ch1++;
        sequencer_counter_ch1 &= 0x07;
    }

    counter_ch2 -= sub_counter_ch2;
    if(counter_ch2 <= 0.0f)
    {
        counter_ch2 += counter_reload_ch2;

        if(square_duty_table[duty2] & (1 << (sequencer_counter_ch2)))
            ch2 = volume_ch2;
        else
            ch2 = 0.0f;

        sequencer_counter_ch2++;
        sequencer_counter_ch2 &= 0x07;
    }

    // Framesequenzer 240 Hz
    ///////////////////////////////////////////////////////////////////////////////////////////////
    counter_frame_sequencer -= sub_counter_frame_sequencer;
    if(counter_frame_sequencer <= 0.0f)
    {
        counter_frame_sequencer += 1.0f;

        if(low_frequency_timer_control & 0x80)
        {
            switch(frame_div_counter)
            {
            case 0:
                ClockLinearCounter();
                ClockLengthCounter();
                break;
            case 1:
                ClockLinearCounter();
                break;
            case 2:
                ClockLinearCounter();
                ClockLengthCounter();
                break;
            case 3:
                ClockLinearCounter();
                break;
            case 4:
                IRQ();
                break;
            }
            frame_div_counter++;
            if(frame_div_counter >= 5)
                frame_div_counter = 0;
        }
        else
        {
            switch(frame_div_counter)
            {
            case 0:
                ClockLinearCounter();
                break;
            case 1:
                ClockLinearCounter();
                ClockLengthCounter();
                break;
            case 2:
                ClockLinearCounter();
                break;
            case 3:
                ClockLinearCounter();
                ClockLengthCounter();
                IRQ();
                break;
            }
            frame_div_counter++;
            if(frame_div_counter >= 4)
                frame_div_counter = 0;
        }
    }
}

float NES_APUClass::GetSampleLeft()
{
    //return sample_left_out = 0;
    return ch1 + ch2;
}

float NES_APUClass::GetSampleRight()
{
    //return sample_right_out = 0;
    return ch1 + ch2;
}

int NES_APUClass::GetVoiceCount()
{
	return VOICE_COUNT_NES_APU;
}

float NES_APUClass::GetSampleVoice(int voice)
{
	switch (voice) {
	case 0:
		return ch1;
		break;
	case 1:
		return ch2;
		break;
	case 2:
		return ch3;
		break;
	case 3:
		return ch4;
		break;
	case 4:
		return ch5;
		break;
	default:
		return 0.0f;
		break;
	}
}

void NES_APUClass::MuteChannel(int voice, bool enable)
{

}

void NES_APUClass::CalcSubCounter()
{
    sub_counter_ch1 = (float)clockspeed / (float)samplerate;
    sub_counter_ch1 /= 2.0f;
    sub_counter_ch2 = (float)clockspeed / (float)samplerate;
    sub_counter_ch2 /= 2.0f;
    sub_counter_frame_sequencer = ((float)FRAMECOUNTER_FREQUENZ / (float)samplerate);
    counter_frame_sequencer = 1.0f;
}

void NES_APUClass::ClockLengthCounter()
{
    //clock length counters and sweep units
}

void NES_APUClass::ClockLinearCounter()
{

}

void NES_APUClass::IRQ()
{
    /// Not Suported for VGM-Player
}
