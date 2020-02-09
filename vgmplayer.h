#ifndef VGMPLAYER_H
#define VGMPLAYER_H

#include <QtCore>

class VGMPlayer
{
public:
    VGMPlayer();
    bool Open(QString filename);

    int GetFileSize();
    uint32_t GetEOFOffset();
    QString GetVersion();
    uint32_t GetSN76489Clock();
    uint32_t GetYM2413Clock();
    uint32_t GetGD3TagOffset();
    uint32_t GetTotalSamples();
    uint32_t GetLoopOffset();
    uint32_t GetLoopSamples();
    uint32_t GetRate();
    uint16_t GetSN76489Feedback();
    uint8_t GetSN76489ShiftRegWidth();
    uint8_t GetSN76489Flags();
    uint32_t GetYM2612Clock();
    uint32_t GetYM2151Clock();
    uint32_t GetVGMDataOffset();

private:
    QFile file;
    uint32_t file_ident;
    uint32_t eof_offset;
    uint32_t version_number;
    uint32_t sn76489_clock;
    uint32_t ym2413_clock;
    uint32_t gd3_tag_offset;
    uint32_t total_samples;
    uint32_t loop_offset;
    uint32_t loop_samples;
    uint32_t rate;
    uint16_t sn76489_feedback;
    uint8_t sn76489_shift_reg_width;
    uint8_t sn76489_flags;
    uint32_t ym2612_clock;
    uint32_t ym2151_clock;
    uint32_t vgm_data_offset;
};

#endif // VGMPLAYER_H
