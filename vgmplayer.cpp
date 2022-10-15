//////////////////////////////////////////////////
//                                              //
// VGM Player with QT                           //
// by thorsten kattanek                         //
//                                              //
// #file: vgmplayer.cpp                         //
//                                              //
// last changes at 10-15-2022                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#include "vgmplayer.h"
#include "./gunzip.h"

// #define GB_DMG_TEST

VGMPlayer::VGMPlayer()
{
    SetSampleRate(44100);   // Default 44100

    is_file_open = false;
    is_playing = false;
    samples_waiting = false;

    streaming_data = nullptr;
    streaming_data_length = 0;
    streaming_pos = 0;

#ifdef GB_DMG_TEST
    gbdmg.SetClockSpeed(4194304);
    gbdmg.WriteReg(0x01, 2<<6);
    gbdmg.WriteReg(0x06, 2<<6);
    /*
    gbdmg.WriteReg(0x03, 0);
    gbdmg.WriteReg(0x04, 0);
    */
#endif
}

VGMPlayer::~VGMPlayer()
{
    if(streaming_data == nullptr)
    {
        delete [] streaming_data;
        streaming_data = nullptr;
    }
}

bool VGMPlayer::Open(QString filename)
{
    int64_t nbytes;

    // for gzip
    unsigned char *window;
    z_stream strm;
    QString temp_filename;

    is_playing = false;
    samples_waiting = false;

    file.setFileName(filename);

    if(!file.open(QIODevice::ReadOnly))
    {
        is_file_open = false;
        return false;
    }

    // File check for gzip compression
    uint16_t gzip_id;
    int ret;

    bool is_compressed = false;

    nbytes = file.read(reinterpret_cast<char*>(&gzip_id), 2);
    if(nbytes == 2)
    {
        if(gzip_id == 0x8b1f)
        {
            file.close();

            is_compressed = true;

            /* initialize inflateBack state for repeated use */
            window = match;                         /* reuse LZW match buffer */
            strm.zalloc = Z_NULL;
            strm.zfree = Z_NULL;
            strm.opaque = Z_NULL;
            ret = inflateBackInit(&strm, 15, window);

            if (ret != Z_OK)
            {
                fprintf(stderr, "gunzip out of memory error--aborting\n");
                return false;
            }

            temp_filename = QDir::tempPath() + "/vgmplayer.tmp";

            ret = gunzip(&strm, filename.toUtf8().data(), temp_filename.toUtf8().data(), 0);

            inflateBackEnd(&strm);
        }
        else file.seek(0);
    }
    else
    {
        // this File is too small
        return false;
    }

    if(is_compressed)
    {
        // temporary file open
        file.setFileName(temp_filename);
        if(!file.open(QIODevice::ReadOnly))
        {
            is_file_open = false;
            return false;
        }
    }

    nbytes = file.read(reinterpret_cast<char*>(&file_ident), 4);

    // 0x56 0x67 0x6d 0x20
    // "Vgm "
    if((file_ident != 0x206d6756) && (nbytes == 4))
    {
        is_file_open = false;
        file.close();
        return false;
    }

    // End Of File Offset
    nbytes = file.read(reinterpret_cast<char*>(&eof_offset), 4);
    eof_offset += 0x04; // offset from file begin

    // Version Number
    nbytes = file.read(reinterpret_cast<char*>(&version_number), 4);

    // SN76948 Clock
    nbytes = file.read(reinterpret_cast<char*>(&sn76489_clock), 4);

    // YM2413 Clock
    nbytes = file.read(reinterpret_cast<char*>(&ym2413_clock), 4);

    // GD3 Tag Offset
    nbytes = file.read(reinterpret_cast<char*>(&gd3_tag_offset), 4);
    gd3_tag_offset += 0x14; // offset from file begin

    // Total Samples
    nbytes = file.read(reinterpret_cast<char*>(&total_samples), 4);

    // Loop Offset
    nbytes = file.read(reinterpret_cast<char*>(&loop_offset), 4);

    // Loop Samples
    nbytes = file.read(reinterpret_cast<char*>(&loop_samples), 4);

    // Loop Rate
    nbytes = file.read(reinterpret_cast<char*>(&rate), 4);

    // SN76489 Feedback
    nbytes = file.read(reinterpret_cast<char*>(&sn76489_feedback), 2);

    // SN76489 Shift Register Width
    nbytes = file.read(reinterpret_cast<char*>(&sn76489_shift_reg_width), 1);

    // SN76489 Flags
    nbytes = file.read(reinterpret_cast<char*>(&sn76489_flags), 1);

    // YM2612 Clock
    nbytes = file.read(reinterpret_cast<char*>(&ym2612_clock), 4);

    // YM2151 Clock
    nbytes = file.read(reinterpret_cast<char*>(&ym2151_clock), 4);

    // VGM Data Offset
    nbytes = file.read(reinterpret_cast<char*>(&vgm_data_offset), 4);

    if(vgm_data_offset == 0)
        vgm_data_offset = 0x40;     // offset from file begin
    else
        vgm_data_offset += 0x34;    // offset from file begin

    if(version_number == 0x00000161 || version_number == 0x00000170)
    {
        // GameBoy DMG Clock
        file.seek(0x80);
        nbytes = file.read(reinterpret_cast<char*>(&gb_dmg_clock), 4);
    }

    file.seek(vgm_data_offset);

    if(streaming_data == nullptr)
    {
        delete [] streaming_data;
        streaming_data = nullptr;
    }

    if(gd3_tag_offset == 0)
    {
       streaming_data_length = eof_offset - vgm_data_offset;
    }
    else
    {
        streaming_data_length = gd3_tag_offset - vgm_data_offset;
    }

    streaming_data = new uint8_t [streaming_data_length];

    // VGM Data Offset
    nbytes = file.read(reinterpret_cast<char*>(streaming_data), streaming_data_length);

    if(nbytes != streaming_data_length)
        qDebug() << "Error: failed streaming length.";

    file.close();

    if(is_compressed)
    {
        // temporary file delete
        file.remove();
    }

    AnalyzingStreamForSoundchips();

    streaming_pos = 0;

    if(is_SN76489_enabled)
        InitSN76489();
    if(is_YM2612_enable)
        InitYM2612();
    if(is_GB_DMG_enable)
        InitGBDMG();

    is_file_open = true;
    return true;
}

bool VGMPlayer::isOpen()
{
    return is_file_open;
}

void VGMPlayer::SetSampleRate(uint32_t samplerate)
{
    this->samplerate = samplerate;

    sn76489.SetSampleRate(samplerate);
    ym2612.SetSampleRate(samplerate);
    gbdmg.SetSampleRate(samplerate);
}

void VGMPlayer::GetNextSample(float *sample_left, float *sample_right)
{
    if(is_playing)
    {
        if(samples_waiting)
        {
            samples_wait_counter--;
            if(samples_wait_counter == 0)
                samples_waiting = false;
        }
        else
        {
            while(!samples_waiting && is_playing)
            {
                ExecuteNextStreamCommand();
            }
        }

        sample_counter++;

        current_left_output_sample = current_right_output_sample = 0.0f;

        if(is_SN76489_enabled)
        {
			sn76489.CalcNextSample();
			current_left_output_sample += sn76489.GetSampleLeft();
			current_right_output_sample += sn76489.GetSampleRight();
        }

        if(is_YM2612_enable)
        {
            float sample = 0.0f;
            current_left_output_sample += sample;
            current_right_output_sample += sample;
        }

        if(is_GB_DMG_enable)
        {
            float sample = gbdmg.GetNextSample();
            current_left_output_sample += sample;
            current_right_output_sample += sample;
        }

        if(current_soundchip_count > 0)
        {
            *sample_left = current_left_output_sample / static_cast<float>(current_soundchip_count);
            *sample_right = current_right_output_sample / static_cast<float>(current_soundchip_count);
        }
        else
        {
            *sample_left = 0.0f;
            *sample_right = 0.0f;
        }
    }
    else
    {
        *sample_left = *sample_right = 0.0f;

#ifdef GB_DMG_TEST
        float sample = gbdmg.GetNextSample();
        *sample_left = sample;
        *sample_left = sample;
#endif
    }
}

void VGMPlayer::SetPlay(bool playing)
{
    if(is_file_open)
    {
        this->is_playing = playing;

        if(is_playing)
        {
            sample_counter = 0;
        }
	}
}

void VGMPlayer::Set_SN76489_StereoStrength(float stereo_strength)
{
	sn76489.SetStereoStrength(stereo_strength);
}

bool VGMPlayer::ExportStreamingData(QString filename)
{
    QFile file;

    file.setFileName(filename);

    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Could not open file!";
        return false;
    }

    int ret = file.write((const char*)(streaming_data), streaming_data_length);
    file.close();

    if(ret == -1)
        return  false;

    return true;
}

int64_t VGMPlayer::GetFileSize()
{
    return file.size();
}

uint32_t VGMPlayer::GetEOFOffset()
{
    return  eof_offset;
}

QString VGMPlayer::GetVersion()
{
    QString version_string = "";

    version_string += QString::number((version_number >> 8) & 0xf) + ".";
    version_string += QString::number((version_number >> 4) & 0xf);
    version_string += QString::number(version_number & 0xf);

    return version_string;
}

uint32_t VGMPlayer::GetSN76489Clock()
{
    return sn76489_clock;
}

uint32_t VGMPlayer::GetYM2413Clock()
{
    return ym2413_clock;
}

uint32_t VGMPlayer::GetGD3TagOffset()
{
    return gd3_tag_offset;
}

uint32_t VGMPlayer::GetTotalSamples()
{
    return total_samples;
}

uint32_t VGMPlayer::GetLoopOffset()
{
    return loop_offset;
}

uint32_t VGMPlayer::GetLoopSamples()
{
    return loop_samples;
}

uint32_t VGMPlayer::GetRate()
{
    return rate;
}

uint16_t VGMPlayer::GetSN76489Feedback()
{
    return sn76489_feedback;
}

uint8_t VGMPlayer::GetSN76489ShiftRegWidth()
{
    return  sn76489_shift_reg_width;
}

uint8_t VGMPlayer::GetSN76489Flags()
{
    return  sn76489_flags;
}

uint32_t VGMPlayer::GetYM2612Clock()
{
    return ym2612_clock;
}

uint32_t VGMPlayer::GetYM2151Clock()
{
    return ym2151_clock;
}

uint32_t VGMPlayer::GetVGMDataOffset()
{
    return vgm_data_offset;
}

uint32_t VGMPlayer::GetGB_DMGClock()
{
    return gb_dmg_clock;
}

uint32_t VGMPlayer::GetCurrentSamplesCount()
{
    return sample_counter;
}

uint32_t VGMPlayer::GetStreamingPos()
{
    return streaming_pos;
}

void VGMPlayer::WriteGBDMGRegister(uint8_t reg_nr, uint8_t value)
{
    gbdmg.WriteReg(reg_nr, value);
}

void VGMPlayer::ExecuteNextStreamCommand()
{
    uint8_t command = streaming_data[streaming_pos++];
    uint16_t value16;
    uint8_t value8;

	QString channelControl;

    switch(command)
    {
    // 0x4F dd    : Game Gear PSG stereo, write dd to port 0x06
    case 0x4f:
		value8 = streaming_data[streaming_pos];
		sn76489.SetStereoControl(value8);
        streaming_pos += 1;
		channelControl = (value8 & 0x80) ? "0" : " ";
		channelControl += (value8 & 0x40) ? "1" : " ";
		channelControl += (value8 & 0x20) ? "2" : " ";
		channelControl += (value8 & 0x10) ? "N" : " ";
		channelControl += (value8 & 0x08) ? "0" : " ";
		channelControl += (value8 & 0x04) ? "1" : " ";
		channelControl += (value8 & 0x02) ? "2" : " ";
		channelControl += (value8 & 0x01) ? "N" : " ";
		qDebug() << "Game Gear PSG Stereo Control - " << channelControl;
        break;
    // 0x50 dd    : PSG (SN76489/SN76496) write value dd
    case 0x50:
        sn76489.WriteReg(streaming_data[streaming_pos]);
        streaming_pos += 1;
        is_SN76489_written = true;
        break;
    // 0x51 aa dd : YM2413, write value dd to register aa
    case 0x51:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0x52 aa dd : YM2612 port 0, write value dd to register aa
    case 0x52:
        ym2612.WriteRegPort0(streaming_data[streaming_pos], streaming_data[streaming_pos+1]);
        streaming_pos += 2;
        is_YM2612_written = true;
        qDebug() << "Command 0x" << QString::number(command,16) << QString::number(streaming_data[streaming_pos-2],16) << QString::number(streaming_data[streaming_pos-1],16) << " - not supported.";
        break;
    // 0x53 aa dd : YM2612 port 1, write value dd to register aa
    case 0x53:
        ym2612.WriteRegPort1(streaming_data[streaming_pos], streaming_data[streaming_pos+1]);
        streaming_pos += 2;
        is_YM2612_written = true;
        qDebug() << "Command 0x" << QString::number(command,16) << QString::number(streaming_data[streaming_pos-2],16) << QString::number(streaming_data[streaming_pos-1],16) << " - not supported.";
        break;
    // 0x54 aa dd : YM2151, write value dd to register aa
    case 0x54:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0x55 aa dd : YM2203, write value dd to register aa
    case 0x55:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0x56 aa dd : YM2608 port 0, write value dd to register aa
    case 0x56:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0x57 aa dd : YM2608 port 1, write value dd to register aa
    case 0x57:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0x58 aa dd : YM2610 port 0, write value dd to register aa
    case 0x58:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0x59 aa dd : YM2610 port 1, write value dd to register aa
    case 0x59:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0x5A aa dd : YM3812, write value dd to register aa
    case 0x5a:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0x5B aa dd : YM3526, write value dd to register aa
    case 0x5b:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0x5C aa dd : Y8950, write value dd to register aa
    case 0x5c:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0x5D aa dd : YMZ280B, write value dd to register aa
    case 0x5d:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0x5E aa dd : YMF262 port 0, write value dd to register aa
    case 0x5e:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0x5F aa dd : YMF262 port 1, write value dd to register aa
    case 0x5f:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0x61 nn nn : Wait n samples, n can range from 0 to 65535 (approx 1.49 seconds).
    //              Longer pauses than this are represented by multiple wait commands.
    case 0x61:
        value16 = static_cast<uint16_t>(streaming_data[streaming_pos+1] << 8);
        value16 |= static_cast<uint16_t>(streaming_data[streaming_pos]);
        samples_waiting = true;
        samples_wait_counter = value16;
        samples_wait_counter--;
        streaming_pos += 2;
        break;
    // 0x62       : wait 735 samples (60th of a second), a shortcut for 0x61 0xdf 0x02
    case 0x62:
        samples_waiting = true;
        samples_wait_counter = 735;
        samples_wait_counter--;
        break;
    // 0x63       : wait 882 samples (50th of a second), a shortcut for 0x61 0x72 0x03
    case 0x63:
        samples_waiting = true;
        samples_wait_counter = 882;
        samples_wait_counter--;
        break;
    // 0x64 cc nn nn : override length of 0x62/0x63 cc - command (0x62/0x63) nn - delay in samples
    //                 [Note: Not yet implemented. Am I really sure about this?]
    case 0x64:
        streaming_pos += 3;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0x66       : end of sound data
    case 0x66:
        is_analyze = is_playing = false;
        break;
    // 0x67 ...   : data block: see below
    case 0x67:
        qDebug() << "Data Block";
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0x68 ...   : PCM RAM write: see below
    case 0x68:
        qDebug() << "PCM RAM Write";
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0x7n       : wait n+1 samples, n can range from 0 to 15.
    case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x76: case 0x77: case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7e: case 0x7f:
        value8 = (command & 0x0f) + 1;
        samples_waiting = true;
        samples_wait_counter = value8;
        break;
    // 0x8n       : YM2612 port 0 address 2A write from the data bank, then wait n samples; n can range from 0 to 15. Note that the wait is n, NOT n+1. (Note: Written to first chip instance only.)
    case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: case 0x87: case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8e: case 0x8f:
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0x90-0x95  : DAC Stream Control Write: see below
    case 0x90:

    case 0x91:

    case 0x92:

    case 0x93:

    case 0x94:

    case 0x95:
        qDebug() << "DAC Stream Control Write:";
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xA0 aa dd : AY8910, write value dd to register aa
    case 0xa0:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xB0 aa dd : RF5C68, write value dd to register aa
    case 0xb0:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xB1 aa dd : RF5C164, write value dd to register aa
    case 0xb1:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xB2 ad dd : PWM, write value ddd to register a (d is MSB, dd is LSB)
    case 0xb2:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xB3 aa dd : GameBoy DMG, write value dd to register aa
    case 0xb3:
        gbdmg.WriteReg(streaming_data[streaming_pos], streaming_data[streaming_pos+1]);
        streaming_pos += 2;
        is_GB_DMG_written = true;
        break;
    // 0xB4 aa dd : NES APU, write value dd to register aa
    case 0xb4:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xB5 aa dd : MultiPCM, write value dd to register aa
    case 0xb5:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xB6 aa dd : uPD7759, write value dd to register aa
    case 0xb6:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xB7 aa dd : OKIM6258, write value dd to register aa
    case 0xb7:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xB8 aa dd : OKIM6295, write value dd to register aa
    case 0xb8:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xB9 aa dd : HuC6280, write value dd to register aa
    case 0xb9:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xBA aa dd : K053260, write value dd to register aa
    case 0xba:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xBB aa dd : Pokey, write value dd to register aa
    case 0xbb:
        streaming_pos += 2;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xC0 aaaa dd : Sega PCM, write value dd to memory offset aaaa
    case 0xc0:
        streaming_pos += 3;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xC1 aaaa dd : RF5C68, write value dd to memory offset aaaa
    case 0xc1:
        streaming_pos += 3;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xC2 aaaa dd : RF5C164, write value dd to memory offset aaaa
    case 0xc2:
        streaming_pos += 3;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xC3 cc aaaa : MultiPCM, write set bank offset aaaa to channel cc
    case 0xc3:
        streaming_pos += 3;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xC4 mmll rr : QSound, write value mmll to register rr (mm - data MSB, ll - data LSB)
    case 0xc4:
        streaming_pos += 3;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xD0 pp aa dd : YMF278B port pp, write value dd to register aa
    case 0xd0:
        streaming_pos += 3;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xD1 pp aa dd : YMF271 port pp, write value dd to register aa
    case 0xd1:
        streaming_pos += 3;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xD2 pp aa dd : SCC1 port pp, write value dd to register aa
    case 0xd2:
        streaming_pos += 3;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xD3 pp aa dd : K054539 write value dd to register ppaa
    case 0xd3:
        streaming_pos += 3;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xD4 pp aa dd : C140 write value dd to register ppaa
    case 0xd4:
        streaming_pos += 3;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    // 0xE0 dddddddd : seek to offset dddddddd (Intel byte order) in PCM data bank
    case 0xe0:
        streaming_pos += 4;
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    default:
        qDebug() << "Command 0x" << QString::number(command,16) << " - not supported.";
        break;
    }
}

void VGMPlayer::AnalyzingStreamForSoundchips()
{
    is_SN76489_enabled = is_SN76489_written = false;
    is_YM2612_enable = is_YM2612_written = false;
    is_GB_DMG_enable = is_GB_DMG_written = false;

    current_soundchip_count = 0;

    is_analyze = true;
    streaming_pos = 0;

    while(is_analyze || streaming_pos < streaming_data_length)
    {
        ExecuteNextStreamCommand();
    }

    if(is_SN76489_written)
    {
        is_SN76489_enabled = true;
        current_soundchip_count++;
    }

    if(is_YM2612_written)
    {
        is_YM2612_enable = true;
        current_soundchip_count++;
    }

    if(is_GB_DMG_written)
    {
        is_GB_DMG_enable = true;
        current_soundchip_count++;
    }
}

void VGMPlayer::InitSN76489()
{
    sn76489.SetClockSpeed(sn76489_clock);
	sn76489.SetStereoControl(0xff);
}

void VGMPlayer::InitYM2612()
{
    ym2612.SetClockSpeed(ym2612_clock);
}

void VGMPlayer::InitGBDMG()
{
    gbdmg.SetClockSpeed(gb_dmg_clock);
}
