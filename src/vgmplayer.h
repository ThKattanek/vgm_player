//////////////////////////////////////////////////
//                                              //
// VGM Player with QT                           //
// by thorsten kattanek                         //
//                                              //
// #file: vgmplayer.h                           //
//                                              //
// last changes at 02-28-2023                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#ifndef VGMPLAYER_H
#define VGMPLAYER_H

#include <QtCore>
#include <zlib.h>
#include "./sn76489_class.h"
#include "./ym2612_class.h"
#include "./gb_dmg_class.h"
#include "./nes_apu_class.h"

class VGMPlayer
{
public:
    VGMPlayer();
    ~VGMPlayer();
    bool Open(QString filename);
    bool isOpen();
    void SetSampleRate(uint32_t samplerate);
    void GetNextSample(float *sample_left, float *sample_right);
	int GetVoiceCount();
	float GetSampleVoice(int voice);
    void SetPlay(bool playing);
	void Stop();
	void Set_SN76489_StereoStrength(float stereo_strength);
    bool ExportStreamingData(QString filename);
	void WriteGBDMGRegister(uint8_t reg_nr, uint8_t value);

	void SetAllChannelsMuteOff();
	void SetChannelMute(int voice, bool enable);

    int64_t GetFileSize();
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
    uint32_t GetGB_DMGClock();
    uint32_t GetCurrentSamplesCount();
    uint32_t GetStreamingPos();

	QString gd3_trackname_en;
	QString gd3_trackname_jp;
	QString gd3_gamename_en;
	QString gd3_gamename_jp;
	QString gd3_systemname_en;
	QString gd3_systemname_jp;
	QString gd3_trackautor_en;
	QString gd3_trackautor_jp;
	QString gd3_releasedate;
	QString gd3_convertername;
	QString gd3_notes;

private:
	void ReadGD3Tag(gzFile file, int gd3_tag_offset);
    void ExecuteNextStreamCommand();
    void AnalyzingStreamForSoundchips();
    void InitSN76489();
    void InitYM2612();
    void InitGBDMG();
	void InitNESAPU();

	QFile   file;
    bool    is_file_open;

    uint32_t samplerate;

    bool     is_playing;
    bool     is_analyze;
    bool     samples_waiting;
    uint32_t samples_wait_counter;

    float    current_left_output_sample;
    float    current_right_output_sample;

    uint32_t sample_counter;

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
    uint32_t gb_dmg_clock;
	uint32_t nes_apu_clock;
    uint32_t vgm_data_offset;

    uint8_t *streaming_data;
    uint32_t streaming_data_length;
    uint32_t streaming_pos;

    bool    is_SN76489_written;
    bool    is_YM2612_written;
    bool    is_GB_DMG_written;
	bool	is_NES_APU_written;

    bool    is_SN76489_enabled;
    bool    is_YM2612_enable;
    bool    is_GB_DMG_enable;
	bool	is_NES_APU_enable;

    int     current_soundchip_count;

    SN76489Class sn76489;
    YM2612Class ym2612;
    GB_DMGClass gbdmg;
	NES_APUClass nesapu;
};

#endif // VGMPLAYER_H
