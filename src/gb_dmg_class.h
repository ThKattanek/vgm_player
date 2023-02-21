//////////////////////////////////////////////////
//                                              //
// GB DMG emulation (Game Boy Sound)            //
// by thorsten kattanek                         //
//                                              //
// #file: gb_dmg_class.h                        //
//                                              //
// last changes at 02-21-2023                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#ifndef GB_DMGCLASS_H
#define GB_DMGCLASS_H

#include <cstdint>

#define VOICE_COUNT_GB_DMG 4

class GB_DMGClass
{
public:
    GB_DMGClass();
    ~GB_DMGClass();

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
	void SoloChannel(int voice);

private:
    void CalcSubCounter();
	uint16_t CalcNewFrequenyCh1();

    uint32_t clockspeed;        // as Hz
    uint32_t samplerate;        // as Hz

	// registers for channel 1
    uint8_t NR10,NR11,NR12,NR13,NR14;

	// registers for channel 2
    uint8_t NR21,NR22,NR23,NR24;

	// Registers for channel 3
    uint8_t NR30,NR31,NR32,NR33,NR34;

	// registers for channel 4
    uint8_t NR41,NR42,NR43,NR44;

	// registers for controlls
    uint8_t NR50,NR51,NR52;

	///////////////////////////////////////////////////////

	// internal Registers
    uint8_t square_duty_table[4];
	uint8_t noise_divisor_table[8];
	float volume_out_table[16];
	float dac_sample_table[16];
	float volume_channel3_table[4];
	float master_volume_table[8];

	// channel 1
	uint16_t channel1_frequency;
	uint16_t new_frequency;
	uint8_t channel1_duty;

	float channel1_counter;
	uint8_t channel1_wave_counter;

	uint8_t channel1_length_counter;
	bool channel1_enable;

	uint8_t channel1_volume_counter;
	uint8_t channel1_current_volume;

	uint8_t sweep_period;
	bool	sweep_negate;
	uint8_t	sweep_shift;

	bool	sweep_enable;
	uint16_t sweep_shadow_frequency;
	uint8_t sweep_timer;

	float channel1_out;

	// channel 2
	uint16_t channel2_frequency;
	uint8_t channel2_duty;

	float channel2_counter;
	uint8_t channel2_wave_counter;

	uint8_t channel2_length_counter;
	bool channel2_enable;

	uint8_t channel2_volume_counter;
	uint8_t channel2_current_volume;

	float channel2_out;

	// channel 3

	// samplebuffer with 16 Byte for 32 4-Bit Samples
	uint8_t sample_buffer[32];
	uint8_t sample_position_counter;

	uint16_t channel3_frequency;
	float channel3_counter;

	uint8_t channel3_length_counter;
	bool channel3_enable;

	float channel3_volume;
	float channel3_out;

	// channel 4
	uint16_t channel4_frequency;
	float channel4_counter;

	uint8_t noise_shift;
	bool noise_width_mode;
	uint8_t noise_divisor;

	uint16_t lfsr;

	uint8_t channel4_length_counter;
	bool channel4_enable;

	uint8_t channel4_volume_counter;
	uint8_t channel4_current_volume;

	float channel4_out;

	///////////////////////////////////////////////////////

	float ch1, ch2, ch3, ch4;

	bool  channel_mute[VOICE_COUNT_GB_DMG];

	float sample_left_out;
	float sample_right_out;

	float sub_counter_square;
	float sub_counter_wave;
	float sub_counter_noise;
	float sub_counter_frame_sequencer;
	float counter_frame_sequencer;

	uint8_t frame_sequencer;
	uint8_t old_frame_sequencer;
};

#endif // GB_DMGCLASS_H
