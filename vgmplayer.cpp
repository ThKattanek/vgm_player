#include "vgmplayer.h"

VGMPlayer::VGMPlayer()
{
    sample_rate = 44100;   // Default 44100
    is_playing = false;

    streaming_data = nullptr;
    streaming_data_length = 0;
    streaming_pos = 0;
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

    is_playing = false;
    streaming_pos = 0;

    file.setFileName(filename);

    if(!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    nbytes = file.read(reinterpret_cast<char*>(&file_ident), 4);

    // 0x56 0x67 0x6d 0x20
    // "Vgm "
    if((file_ident != 0x206d6756) && (nbytes == 4))
    {
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

    return true;
}

void VGMPlayer::SetSampleRate(uint32_t sample_rate)
{
    this->sample_rate = sample_rate;
}

float VGMPlayer::GetNextSample()
{
    if(is_playing)
    {
        return 0.0f;
    }
    else
    {
        return 0.0f;
    }
}

void VGMPlayer::SetPlay(bool playing)
{
    this->is_playing = playing;
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
