//////////////////////////////////////////////////
//                                              //
// SN76489 emulation                            //
// by thorsten kattanek                         //
//                                              //
// #file: sn76489_class.cpp                     //
//                                              //
// last changes at 02-21-2023                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#define  EMU_ATTENUATION

#include "sn76489_class.h"

SN76489Class::SN76489Class()
{
    clockspeed = 4000000;
    samplerate = 44100;

    CalcSubCounter();

    tone1_output = 0.25f;
    tone2_output = 0.25f;
    tone3_output = 0.25f;
    noise_output = 0.25f;

    tone1_freq = 0;
    tone2_freq = 0;
    tone3_freq = 0;

    tone1_attenuation = 0.0f;
    tone2_attenuation = 0.0f;
    tone3_attenuation = 0.0f;

	stereo_control = 0xff;
	stereo_strength = 0.75f;

    white_noise = false;
    noise_counter = 0.0f;
    shift_reg = 0x8000;

    is_wait_second_byte = 0;

	for(int i=0; i<VOICE_COUNT_SN76489; i++)
		channel_mute[i] = false;
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
        white_noise = value & 0x04;
        noise_freq = value & 0x03;
        CalcAddNoiseCounter();
        break;
    case 0xf0:  // Noise Attenuation
        noise_attenuation = volume_table[value & 0x0f];
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

void SN76489Class::CalcNextSample()
{
    // tone channels
    tone1_counter -= sub_counter_tone;
    if(tone1_counter <= 0)
    {
        tone1_counter += tone1_freq;
        tone1_output *= -1.0f;
    }

    tone2_counter -= sub_counter_tone;
    if(tone2_counter <= 0)
    {
        tone2_counter += tone2_freq;
        tone2_output *= -1.0f;
    }

    bool toggle_tone3 = false;

    tone3_counter -= sub_counter_tone;
    if(tone3_counter <= 0)
    {
        tone3_counter += tone3_freq;
        tone3_output *= -1.0f;
        toggle_tone3 = true;
    }

    // noise channel
    if(noise_freq == 3)
    {
        if(toggle_tone3)
            StepNoiseGerator();
    }
    else
    {
        noise_counter += add_counter_noise;
        if(noise_counter >= 1.0f)
            StepNoiseGerator();
    }
/*
#ifndef EMU_ATTENUATION
    return  tone1_output + tone2_output + tone3_output + noise_output;
#else
    return  tone1_output * tone1_attenuation + tone2_output * tone2_attenuation + tone3_output * tone3_attenuation + noise_output * noise_attenuation;
#endif
*/
}

void SN76489Class::SetStereoControl(uint8_t stereo_control)
{
	this->stereo_control = stereo_control;
}

void SN76489Class::SetStereoStrength(float stereo_strength)
{
	if(stereo_strength < 0.0f)
		stereo_strength = 0.0f;
	if(stereo_strength > 1.0f)
		stereo_strength = 1.0f;

	this->stereo_strength = stereo_strength;
}

float SN76489Class::GetSampleLeft()
{
	uint8_t channels = stereo_control >> 4;

	#ifndef EMU_ATTENUATION
	return  ((channel_mute[0]) ? 0.0f : ((channels & 0x08) ? tone1_output : tone1_output * stereo_strength)) + \
			((channel_mute[1]) ? 0.0f : ((channels & 0x04) ? tone2_output : tone2_output * stereo_strength)) + \
			((channel_mute[2]) ? 0.0f : ((channels & 0x02) ? tone3_output : tone3_output * stereo_strength)) + \
			((channel_mute[3]) ? 0.0f : ((channels & 0x01) ? noise_output : noise_output * stereo_strength));
	#else
		return  ((channel_mute[0]) ? 0.0f : ((channels & 0x08) ? tone1_output * tone1_attenuation : tone1_output * tone1_attenuation * stereo_strength)) + \
				((channel_mute[1]) ? 0.0f : ((channels & 0x04) ? tone2_output * tone2_attenuation : tone2_output * tone2_attenuation * stereo_strength)) + \
				((channel_mute[2]) ? 0.0f : ((channels & 0x02) ? tone3_output * tone3_attenuation : tone3_output * tone3_attenuation * stereo_strength)) + \
				((channel_mute[3]) ? 0.0f : ((channels & 0x01) ? noise_output * noise_attenuation : noise_output * noise_attenuation * stereo_strength));
	#endif
}

float SN76489Class::GetSampleRight()
{
	uint8_t channels = stereo_control;
	#ifndef EMU_ATTENUATION
	return  ((channel_mute[0]) ? 0.0f : ((channels & 0x08) ? tone1_output : tone1_output * stereo_strength)) + \
			((channel_mute[1]) ? 0.0f : ((channels & 0x04) ? tone2_output : tone2_output * stereo_strength)) + \
			((channel_mute[2]) ? 0.0f : ((channels & 0x02) ? tone3_output : tone3_output * stereo_strength)) + \
			((channel_mute[3]) ? 0.0f : ((channels & 0x01) ? noise_output : noise_output * stereo_strength));
	#else
		return  ((channel_mute[0]) ? 0.0f : ((channels & 0x08) ? tone1_output * tone1_attenuation : tone1_output * tone1_attenuation * stereo_strength)) + \
				((channel_mute[1]) ? 0.0f : ((channels & 0x04) ? tone2_output * tone2_attenuation : tone2_output * tone2_attenuation * stereo_strength)) + \
				((channel_mute[2]) ? 0.0f : ((channels & 0x02) ? tone3_output * tone3_attenuation : tone3_output * tone3_attenuation * stereo_strength)) + \
				((channel_mute[3]) ? 0.0f : ((channels & 0x01) ? noise_output * noise_attenuation : noise_output * noise_attenuation * stereo_strength));
#endif
}

int SN76489Class::GetVoiceCount()
{
	return VOICE_COUNT_SN76489;
}

float SN76489Class::GetSampleVoice(int voice)
{
		switch (voice) {
		case 0:
			return (channel_mute[0]) ? 0.0f : tone1_output * tone1_attenuation;
			break;
		case 1:
			return (channel_mute[1]) ? 0.0f : tone2_output * tone2_attenuation;
			break;
		case 2:
			return (channel_mute[2]) ? 0.0f : tone3_output * tone3_attenuation;
			break;
		case 3:
			return (channel_mute[3]) ? 0.0f : noise_output * noise_attenuation;
			break;
		default:
			return 0.0f;
			break;
		}
}

void SN76489Class::MuteChannel(int voice, bool enable)
{
	if(voice >= VOICE_COUNT_SN76489)
		return;

	channel_mute[voice] = enable;
}

void SN76489Class::CalcSubCounter()
{
    sub_counter_tone = (clockspeed >> 4) / samplerate;
    CalcAddNoiseCounter();
}

void SN76489Class::CalcAddNoiseCounter()
{
    uint32_t noise_input_clock = clockspeed >> 8;

    if(noise_freq < 3)
        noise_input_clock >>= noise_freq+1;

    add_counter_noise = (float)noise_input_clock / (float)samplerate;
}

void SN76489Class::StepNoiseGerator()
{
    noise_counter -= 1.0f;

    if(shift_reg & 1)
        noise_output = 0.25f;
    else
        noise_output = -0.25f;

    uint16_t tmp;

    if(white_noise)
        tmp = (((shift_reg & 1) ^ ((shift_reg >> 3) & 1)) & 1) << 15;
    else
        tmp = (shift_reg & 1) << 15;

    shift_reg = shift_reg >> 1;
    shift_reg |= tmp;
}
