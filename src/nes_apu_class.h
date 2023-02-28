//////////////////////////////////////////////////
//                                              //
// VGM Player with QT                           //
// by thorsten kattanek                         //
//                                              //
// #file: nes_apu_class.h                       //
//                                              //
// last changes at 02-28-2023                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#ifndef NES_APUCLASS_H
#define NES_APUCLASS_H

#include <cstdint>

#define VOICE_COUNT_NES_APU 5

class NES_APUClass
{
public:
	NES_APUClass();

	void SetClockSpeed(uint32_t clockspeed);
	void SetSampleRate(uint32_t samplerate);
	void Reset();
	void WriteReg(uint8_t reg_nr, uint8_t value);
	void CalcNextSample();
	float GetSampleLeft();
	float GetSampleRight();
	int GetVoiceCount();
	float GetSampleVoice(int voice);
	void MuteChannel(int voice, bool enable);
};

#endif // NES_APUCLASS_H
