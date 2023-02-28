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

}

void NES_APUClass::SetClockSpeed(uint32_t clockspeed)
{

}

void NES_APUClass::SetSampleRate(uint32_t samplerate)
{

}

void NES_APUClass::Reset()
{

}

void NES_APUClass::WriteReg(uint8_t reg_nr, uint8_t value)
{

}

void NES_APUClass::CalcNextSample()
{

}

float NES_APUClass::GetSampleLeft()
{

}

float NES_APUClass::GetSampleRight()
{

}

int NES_APUClass::GetVoiceCount()
{
	return VOICE_COUNT_NES_APU;
}

float NES_APUClass::GetSampleVoice(int voice)
{

}

void NES_APUClass::MuteChannel(int voice, bool enable)
{

}
