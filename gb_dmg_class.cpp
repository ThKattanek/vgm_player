//////////////////////////////////////////////////
//                                              //
// GB DMG emulation (Game Boy Sound)            //
// by thorsten kattanek                         //
//                                              //
// #file: gb_dmg_class.cpp                      //
//                                              //
// last changes at 10-22-2022                   //
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

	volume_out_table[0] = 0.0f;
	volume_out_table[1] = 0.07f;
	volume_out_table[2] = 0.13f;
	volume_out_table[3] = 0.2f;
	volume_out_table[4] = 0.27f;
	volume_out_table[5] = 0.33f;
	volume_out_table[6] = 0.4f;
	volume_out_table[7] = 0.47f;
	volume_out_table[8] = 0.53f;
	volume_out_table[9] = 0.6f;
	volume_out_table[10] = 0.67f;
	volume_out_table[11] = 0.73f;
	volume_out_table[12] = 0.8f;
	volume_out_table[13] = 0.87f;
	volume_out_table[14] = 0.93f;
	volume_out_table[15] = 1.0f;

	for(int i=0; i<15; i++)
		dac_sample_table[i] = i * 0.0357143f - 0.25f;

	volume_channel3_table[0] = 0.0f / 1.0f;
	volume_channel3_table[1] = 1.0f / 1.0f;
	volume_channel3_table[2] = 0.5f / 1.0f;
	volume_channel3_table[3] = 0.25 / 1.0f;

	counter_frame_sequencer = 1.0f;
	frame_sequencer = 0;

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
		sweep_period = (NR10 & 0x70) >> 4;
		sweep_negate = (NR10 & 0x08) >> 3;
		sweep_shift = NR10 & 0x07;
		qDebug() << "Sweep Period: " << sweep_period << ", Sweep Negate: " << sweep_negate << ", Sweep Shift: " << sweep_shift;
        break;
    case 0x11:
        NR11 = value;
		channel1_length_counter = (64 - (value & 0x3f));
		channel1_duty = value >> 6;
        break;
    case 0x12:
        NR12 = value;
        break;
    case 0x13:
        NR13 = value;
		channel1_frequency = (2048 - ( NR13 | ((NR14 & 0x07) << 8))) * 4;
        break;
    case 0x14:
        NR14 = value;
		channel1_frequency = (2048 - ( NR13 | ((NR14 & 0x07) << 8))) * 4;
		if(value & 0x80)
		{
			// Trigger Event
			channel1_enable = true;

			// Sweep
			sweep_shadow_frequency = (2048 - ( NR13 | ((NR14 & 0x07) << 8))) * 4;

			if(sweep_period == 0)
				sweep_timer = 8;
			else
				sweep_timer = sweep_period;

			if((sweep_period != 0) || (sweep_shift != 0))
				sweep_enable = true;
			else
				sweep_enable = false;

			if(sweep_shift != 0)
				CalcNewFrequenyCh1();
			//

			if(channel1_length_counter == 0) channel1_length_counter = 63;
			channel1_counter = channel1_frequency;
			channel1_current_volume = NR12 >> 4;
			channel1_volume_counter = NR12 & 0x07;
		}
        break;
    case 0x16:
        NR21 = value;
		channel2_length_counter = (64 - (value & 0x3f));
		channel2_duty = value >> 6;
        break;
    case 0x17:
        NR22 = value;
        break;
    case 0x18:
        NR23 = value;
		channel2_frequency = (2048 - ( NR23 | ((NR24 & 0x07) << 8))) * 4;
        break;
    case 0x19:
        NR24 = value;
		channel2_frequency = (2048 - ( NR23 | ((NR24 & 0x07) << 8))) * 4;
		if(value & 0x80)
		{
			// Trigger Event
			channel2_enable = true;
			if(channel2_length_counter == 0) channel2_length_counter = 63;
			channel2_counter = channel2_frequency;
			channel2_current_volume = NR22 >> 4;
			channel2_volume_counter = NR22 & 0x07;
		}
        break;
    case 0x1A:
		NR30 = value;
        break;
    case 0x1B:
        NR31 = value;
		channel3_length_counter = 256 - value;
        break;
    case 0x1C:
        NR32 = value;
		channel3_volume = volume_channel3_table[(NR32 >> 5) & 0x03];
        break;
    case 0x1D:
        NR33 = value;
		channel3_frequency = (2048 - ( NR33 | ((NR34 & 0x07) << 8))) * 2;
        break;
    case 0x1E:
        NR34 = value;
		channel3_frequency = (2048 - ( NR33 | ((NR34 & 0x07) << 8))) * 2;
		if(value & 0x80)
		{
			// Trigger Event
			channel3_enable = true;
			if(channel3_length_counter == 0) channel3_length_counter = 255;
			channel3_counter = channel3_frequency;
			sample_position_counter = 0;
		}
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
		sample_buffer[(reg_nr - 0x20)] = value;
        break;
    default:
        qDebug() << "Not Supported register!";
        break;
    }
}

float GB_DMGClass::GetNextSample()
{
	// channel1 [square]
	channel1_counter -= sub_counter_square;
	if(channel1_counter <= 0.0f)
    {
		channel1_counter += channel1_frequency;

		if(channel1_enable)
		{
			if(square_duty_table[channel1_duty] & (1 << (7 - channel1_wave_counter)))
				channel1_out = 0.25f;
			else
				channel1_out = -0.25f;
		}
		else
			channel1_out = 0.0f;

		channel1_wave_counter++;
		channel1_wave_counter &= 0x07;
    }

	// channel2 [square]
	channel2_counter -= sub_counter_square;
	if(channel2_counter <= 0.0f)
	{
		channel2_counter += channel2_frequency;

		if(channel2_enable)
		{
			if(square_duty_table[channel2_duty] & (1 << (7 - channel2_wave_counter)))
				channel2_out = 0.25f;
			else
				channel2_out = -0.25f;
		}
		else
			channel2_out = 0.0f;

		channel2_wave_counter++;
		channel2_wave_counter &= 0x07;
	}

	// channle3 [wave]
	channel3_counter -= sub_counter_square;
	if(channel3_counter <= 0.0f)
	{
		channel3_counter += channel3_frequency;

		if(channel3_enable)
		{
			// next sample from
			if(sample_position_counter & 1)
				channel3_out = dac_sample_table[sample_buffer[sample_position_counter >> 1] & 0x0f];
			else
				channel3_out = dac_sample_table[sample_buffer[sample_position_counter >> 1] >> 4];
		}
		else
			channel3_out = 0.0f;

		if(~NR30 & 0x80)
			channel3_out = 0.0f;

		sample_position_counter++;
		sample_position_counter &= 0x1f;
	}

	// Framesequenzer 512 Hz
	///////////////////////////////////////////////////////////////////////////////////////////////
	counter_frame_sequencer -= sub_counter_frame_sequencer;
	if(counter_frame_sequencer <= 0.0f)
	{
		counter_frame_sequencer += 1.0f;

		frame_sequencer++;

		if(frame_sequencer & 1)
		{
			// 256 Hz

			if(NR14 & 0x40) // Enable length counter 1
			{
				if(channel1_length_counter > 0)
				{
					channel1_length_counter--;
					if(channel1_length_counter >= 0)
						channel1_enable = false;
				}
			}

			if(NR24 & 0x40) // Enable length counter 2
			{
				if(channel2_length_counter > 0)
				{
					channel2_length_counter--;
					if(channel2_length_counter >= 0)
						channel2_enable = false;
				}
			}

			if(NR34 & 0x40) // Enable length counter 3
			{
				if(channel3_length_counter > 0)
				{
					channel3_length_counter--;
					if(channel3_length_counter >= 0)
						channel3_enable = false;
				}
			}
		}

		if((frame_sequencer & 2) && !((frame_sequencer-1) & 2))
		{
			// 128 Hz
			if(sweep_timer > 0)
				sweep_timer--;

			if(sweep_timer == 0)
			{
				if(sweep_period > 0)
					sweep_timer = sweep_period;
				else
					sweep_timer = 8;

				if(sweep_enable && (sweep_period > 0))
				{
					new_frequency = CalcNewFrequenyCh1();

					if((new_frequency <= 2047) && (sweep_shift > 0))
					{
						channel1_frequency = new_frequency;
						sweep_shadow_frequency = new_frequency;

						CalcNewFrequenyCh1();
					}
				}
			}
		}

		if((frame_sequencer & 4) && !((frame_sequencer-1) & 4))
		{
			// 64 Hz
			if((NR12 & 0x07) > 0)
			{
				// Volume sweep for channel 1 is enalable
				channel1_volume_counter--;
				if(channel1_volume_counter == 0)
				{
					channel1_volume_counter = NR12 & 0x07;

					if((NR12 & 0x08) && channel1_current_volume < 15)
					{
						channel1_current_volume++;
					}

					if(!(NR12 & 0x08) && channel1_current_volume > 0)
					{
						channel1_current_volume--;
					}
				}
			}

			if((NR22 & 0x07) > 0)
			{
				// Volume sweep for channel 2 is enalable
				channel2_volume_counter--;
				if(channel2_volume_counter == 0)
				{
					channel2_volume_counter = NR22 & 0x07;

					if((NR22 & 0x08) && channel2_current_volume < 15)
					{
						channel2_current_volume++;
					}

					if(!(NR22 & 0x08) && channel2_current_volume > 0)
					{
						channel2_current_volume--;
					}
				}
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////

	float sound_mix = channel1_out * volume_out_table[channel1_current_volume & 0x0f] + channel2_out * volume_out_table[channel2_current_volume & 0x0f] + channel3_out * channel3_volume;

	static float capacitor1 = 0.5f;

	float sound_out = sound_mix - capacitor1;
	capacitor1 = sound_mix - sound_out * 0.999958;

	return sound_out;
}

void GB_DMGClass::Reset()
{
	channel1_enable = channel2_enable = channel3_enable = channel4_enable = false;
	channel1_out = channel2_out = channel3_out = 0.0f;

	sweep_timer = 0;
	sweep_enable = false;
	sweep_shadow_frequency = 0;

	WriteReg(0x00, 0x80);
	WriteReg(0x01, 0xBF);
	WriteReg(0x02, 0xF3);
	WriteReg(0x04, 0xBF);
	WriteReg(0x06, 0x3F);
	WriteReg(0x07, 0x00);
	WriteReg(0x09, 0xBF);
	WriteReg(0x0A, 0x7F);
	WriteReg(0x0B, 0xFF);
	WriteReg(0x0C, 0x9F);
	WriteReg(0x0E, 0xBF);
	WriteReg(0x10, 0xFF);
	WriteReg(0x11, 0x00);
	WriteReg(0x12, 0x00);
	WriteReg(0x13, 0xBF);
	WriteReg(0x14, 0x77);
	WriteReg(0x15, 0xF3);
}

void GB_DMGClass::CalcSubCounter()
{
	sub_counter_square = ((float)clockspeed / (float)samplerate);
	sub_counter_frame_sequencer = (512.0f / (float)samplerate);
}

uint16_t GB_DMGClass::CalcNewFrequenyCh1()
{
	uint16_t new_frequency = sweep_shadow_frequency >> sweep_shift;

	if(sweep_negate)
		new_frequency = sweep_shadow_frequency + new_frequency;
	else
		new_frequency = sweep_shadow_frequency - new_frequency;

	if(new_frequency > 2047)
		channel1_enable = false;

	return new_frequency;
}
