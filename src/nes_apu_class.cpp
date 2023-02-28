//////////////////////////////////////////////////
//                                              //
// VGM Player with QT                           //
// by thorsten kattanek                         //
//                                              //
// #file: nes_apu_class.cpp                     //
//                                              //
// last changes at 02-28-2023                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#include "nes_apu_class.h"

NES_APUClass::NES_APUClass()
{
	clockspeed = 1789772;
	samplerate = 44100;
	CalcSubCounter();

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

}

void NES_APUClass::WriteReg(uint8_t reg_nr, uint8_t value)
{

}

void NES_APUClass::CalcNextSample()
{
	sample_left_out = 0.0f;
	sample_right_out = 0.0f;

	ch1 = 1.0f;
	ch2 = 0.0f;
	ch3 = -1.0f;
	ch4 = 1.0f;
	ch5 = 0.0f;
}

float NES_APUClass::GetSampleLeft()
{
	return sample_left_out;
}

float NES_APUClass::GetSampleRight()
{
	return sample_right_out;
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

}
