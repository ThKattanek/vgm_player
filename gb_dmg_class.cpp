//////////////////////////////////////////////////
//                                              //
// GB DMG emulation                             //
// by thorsten kattanek                         //
//                                              //
// #file: gb_dmg_class.cpp                      //
//                                              //
// last changes at 10-18-2022                   //
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
        break;
    case 0x11:
        NR11 = value;
		square1_length_counter = (64 - (value & 0x3f));
		if(square1_length_counter == 0)
			square1_enable = false;
		else
			square1_enable = true;
        square1_duty = value >> 6;
        break;
    case 0x12:
        NR12 = value;
        break;
    case 0x13:
        NR13 = value;
		square1_start_counter = 2048 - ( NR13 | ((NR14 & 0x07) << 8));
        break;
    case 0x14:
        NR14 = value;
		square1_start_counter = 2048 - ( NR13 | ((NR14 & 0x07) << 8));
        break;
    case 0x16:
        NR21 = value;
		square2_length_counter = (64 - (value & 0x3f));
		if(square2_length_counter == 0)
			square2_enable = true;
		else
			square2_enable = true;
        square2_duty = value >> 6;
        break;
    case 0x17:
        NR22 = value;
        break;
    case 0x18:
        NR23 = value;
        square2_start_counter = 2048-( NR23 | (NR24 & 0x07) << 8);
        break;
    case 0x19:
        NR24 = value;
        square2_start_counter = 2048-( NR23 | (NR24 & 0x07) << 8);
        break;
    case 0x1A:
        NR30 = value;
        break;
    case 0x1B:
        NR31 = value;
        break;
    case 0x1C:
        NR32 = value;
        break;
    case 0x1D:
        NR33 = value;
        break;
    case 0x1E:
        NR34 = value;
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
        WAVE_TABLE[(reg_nr - 0x20)] = value;
        break;
    default:
        qDebug() << "Not Supported register!";
        break;
    }
}

float GB_DMGClass::GetNextSample()
{
    //Sqaure1
	square1_counter -= sub_counter_square;
	if(square1_counter <= 0.0f)
    {
        square1_counter += square1_start_counter;

		if(square1_enable)
		{
			if(square_duty_table[square1_duty] & (1 << (7 - square1_wave_counter)))
				square1_out = 0.25f;
			else
				square1_out = -0.25f;
		}
		else
			square1_out = 0.0f;

        square1_wave_counter++;
        square1_wave_counter &= 0x07;
    }

	//Sqaure2
	square2_counter -= sub_counter_square;
	if(square2_counter <= 0.0f)
	{
		square2_counter += square2_start_counter;

		if(square2_enable)
		{
			if(square_duty_table[square2_duty] & (1 << (7 - square2_wave_counter)))
				square2_out = 0.25f;
			else
				square2_out = -0.25f;
		}
		else
			square2_out = 0.0f;

		square2_wave_counter++;
		square2_wave_counter &= 0x07;
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
				if(square1_length_counter > 0)
				{
					square1_length_counter--;
					if(square1_length_counter == 0)
						square1_enable = false;
				}
			}

			if(NR24 & 0x40) // Enable length counter 2
			{
				if(square2_length_counter > 0)
				{
					square2_length_counter--;
					if(square2_length_counter == 0)
						square2_enable = false;
				}
			}
		}

		if((frame_sequencer & 2) && !((frame_sequencer-1) & 2))
		{
			// 128 Hz
		}

		if((frame_sequencer & 4) && !((frame_sequencer-1) & 4))
		{
			// 64 Hz
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////

	return square1_out + square2_out;
}

void GB_DMGClass::Reset()
{
    /*
     * Initial Register Values
        $FF10(NR10) - 0x80
        $FF11(NR11) - 0xBF
        $FF12(NR12) - 0xF3
        $FF13(NR13) - No Change
        $FF14(NR14) - 0xBF
        $FF15(NR20) - No Change
        $FF16(NR21) - 0x3F
        $FF17(NR22) - 0x00
        $FF18(NR23) - No Change
        $FF19(NR24) - 0xBF
        $FF1A(NR30) - 0x7F
        $FF1B(NR31) - 0xFF
        $FF1C(NR32) - 0x9F
        $FF1D(NR33) - No Change
        $FF1E(NR34) - 0xBF
        $FF1F(NR40) - No Change
        $FF20(NR41) - 0xFF
        $FF21(NR42) - 0x00
        $FF22(NR43) - 0x00
        $FF23(NR44) - 0xBF
        $FF24(NR50) - 0x77
        $FF25(NR51) - 0xF3
        $FF26(NR52) - SEE BELOW
    */

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

//	WriteReg(0x00, 0x00);
//	WriteReg(0x01, 0x00);
//	WriteReg(0x02, 0x00);
//	WriteReg(0x04, 0x00);
//	WriteReg(0x06, 0x00);
//	WriteReg(0x07, 0x00);
//	WriteReg(0x09, 0x00);
//	WriteReg(0x0A, 0x00);
//	WriteReg(0x0B, 0x00);
//	WriteReg(0x0C, 0x00);
//	WriteReg(0x0E, 0x00);
//	WriteReg(0x10, 0x00);
//	WriteReg(0x11, 0x00);
//	WriteReg(0x12, 0x00);
//	WriteReg(0x13, 0x00);
//	WriteReg(0x14, 0x00);
//	WriteReg(0x15, 0x00);
}

void GB_DMGClass::CalcSubCounter()
{
	sub_counter_square = ((float)clockspeed / (float)samplerate) / 4.0f;
	sub_counter_frame_sequencer = (512.0f / (float)samplerate);
}
