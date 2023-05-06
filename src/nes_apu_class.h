//////////////////////////////////////////////////
//                                              //
// VGM Player with QT                           //
// by thorsten kattanek                         //
//                                              //
// #file: nes_apu_class.h                       //
//                                              //
// last changes at 05-05-2023                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#ifndef NES_APUCLASS_H
#define NES_APUCLASS_H

#include <cstdint>

#define VOICE_COUNT_NES_APU 5
#define FRAMECOUNTER_FREQUENZ 240

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

private:
	void CalcSubCounter();
    void ClockLengthCounter();
    void ClockLinearCounter();
    void IRQ();

	uint32_t clockspeed;        // as Hz
	uint32_t samplerate;        // as Hz

    //// Register ////
    //// Low frequency timer control
    uint8_t duty1, duty2;
    uint16_t rct1, rct2;
    uint8_t low_frequency_timer_control;

    uint8_t square_duty_table[4];

	///////////////////////////////////////////////////////

	float ch1, ch2, ch3, ch4, ch5;

	bool  channel_mute[VOICE_COUNT_NES_APU];

	float sample_left_out;
	float sample_right_out;

    float counter_ch1;
    float counter_reload_ch1;
    float sub_counter_ch1;
    uint8_t sequencer_counter_ch1;

    float counter_ch2;
    float counter_reload_ch2;
    float sub_counter_ch2;
    uint8_t sequencer_counter_ch2;

    float sub_counter_frame_sequencer;
    float counter_frame_sequencer;

    uint8_t frame_div_counter;
};

#endif // NES_APUCLASS_H
