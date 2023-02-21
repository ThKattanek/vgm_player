//////////////////////////////////////////////////
//                                              //
// SN76489 emulation                            //
// by thorsten kattanek                         //
//                                              //
// #file: sn76489_class.h                       //
//                                              //
// last changes at 02-21-2023                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#ifndef SN76489CLASS_H
#define SN76489CLASS_H

#include <cstdint>

#define VOICE_COUNT_SN76489 4

static const float volume_table[16]={1.0f, 0.7943f, 0.631f, 0.5012f, 0.3981f, 0.3162f, 0.2512f, 0.2004f, 0.1585f, 0.1259f, 0.1f, 0.0794f, 0.0634f, 0.0501f, 0.0398f, 0.f};

class SN76489Class
{
public:
    SN76489Class();
    ~SN76489Class();

    void SetClockSpeed(uint32_t clockspeed);
    void SetSampleRate(uint32_t samplerate);
    void WriteReg(uint8_t value);
	void CalcNextSample();
	void SetStereoControl(uint8_t stereo_control);
	void SetStereoStrength(float stereo_strength);
	float GetSampleLeft();
	float GetSampleRight();
	int GetVoiceCount();
	float GetSampleVoice(int voice);
	void MuteChannel(int voice, bool enable);
	void SoloChannel(int voice);

private:
    void CalcSubCounter();
    void CalcAddNoiseCounter();
    void StepNoiseGerator();

    uint32_t clockspeed;        // as Hz
    uint32_t samplerate;        // as Hz
    uint16_t sub_counter_tone;
    float add_counter_noise;

    int16_t tone1_counter;
    int16_t tone2_counter;
    int16_t tone3_counter;

    float noise_counter;

    uint16_t tone1_freq;
    uint16_t tone2_freq;
    uint16_t tone3_freq;

    float   tone1_output;
    float   tone2_output;
    float   tone3_output;
    float   noise_output;

    float   tone1_attenuation;
    float   tone2_attenuation;
    float   tone3_attenuation;
    float   noise_attenuation;

	uint8_t	stereo_control;
	float stereo_strength;			// 0.0f = maximal stereo effect 1.0f = no stereo effekt

    uint16_t shift_reg;
    bool     white_noise;
    uint8_t  noise_freq;

    uint16_t freq_latch;
    uint8_t is_wait_second_byte;   // 0=no / 1=yes - tone1 / 2=yes - tone2 / 3=yes - tone3

	bool channel_mute[VOICE_COUNT_SN76489];
};

#endif // SN76489CLASS_H
