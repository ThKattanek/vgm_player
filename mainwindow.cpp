//////////////////////////////////////////////////
//                                              //
// VGM Player with QT                           //
// by thorsten kattanek                         //
//                                              //
// #file: mainwindow.cpp                        //
//                                              //
// last changes at 02-15-2020                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#include "mainwindow.h"
#include "ui_mainwindow.h"

#define SAMPLERATE 44100

void AudioMix(void *userdata, uint8_t *audio_buffer, int length);
void AudioMix(void *userdata, uint8_t *audio_buffer, int length)
{
    MainWindow *mw = static_cast<MainWindow*>(userdata);
    mw->FillAudioBuffer(audio_buffer,length);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    LogText("VGM Player - by Thorsten Kattanek\n");

    InitSDLAudio();

    /*
    if(!vgm_player.Open(":/vgm/vgm_demo13"))
    {
        LogText("- Fehler beim öffnen der Demo VGM Datei.");
    }
    else
    {
        LogText("- VGM Demo Datei wurde geöffnet. [" + QString::number(vgm_player.GetFileSize()) + " Bytes]");
        LogText("- EOF Offset: 0x" + QString::number(vgm_player.GetEOFOffset(),16).toUpper() + " | " + (QString::number(vgm_player.GetEOFOffset())));
        LogText("- VGM Version Nummer: " + vgm_player.GetVersion());
        LogText("- SN76489 Clock: " + QString::number(vgm_player.GetSN76489Clock()) + " Hz");
        LogText("- YM2413 Clock: " + QString::number(vgm_player.GetYM2413Clock()) + " Hz");
        LogText("- DG3 Tag Offset: 0x" + QString::number(vgm_player.GetGD3TagOffset(),16));
        LogText("- Total Samples: " + QString::number(vgm_player.GetTotalSamples()));
        LogText("- Loop Offset: " + QString::number(vgm_player.GetLoopOffset()));
        LogText("- Loop Samples: " + QString::number(vgm_player.GetLoopSamples()));
        LogText("- Rate: " + QString::number(vgm_player.GetRate()) + " Hz");
        LogText("- SN76489 Feedback: 0x" + QString::number(vgm_player.GetSN76489Feedback(),16));
        LogText("- SN76489 Shift Register Width: " + QString::number(vgm_player.GetSN76489ShiftRegWidth()));
        LogText("- SN76489 Flags: 0x" + QString::number(vgm_player.GetSN76489Flags(),16));
        LogText("- YM2612 Clock: " + QString::number(vgm_player.GetYM2612Clock()) + " Hz");
        LogText("- YM2151 Clock: " + QString::number(vgm_player.GetYM2151Clock()) + " Hz");
        LogText("- VGM Data Offset: 0x" + QString::number(vgm_player.GetVGMDataOffset(),16));
    }

    vgm_player.SetPlay(true);
    */
}

MainWindow::~MainWindow()
{
    SDL_PauseAudio(1);
    SDL_CloseAudio();

    delete ui;
}

void MainWindow::LogText(const QString text)
{
    ui->log_output->appendPlainText(text);
}

void MainWindow::InitSDLAudio()
{
    /// SDL Audio Installieren ///

     if(SDL_Init(SDL_INIT_AUDIO) < 0)
     {
          LogText("- ERROR: Fehler beim installieren von SDL2");
          return;
     }
     else
         LogText("- SDL2 wurde installiert");

     /// SLD Audio Installieren (C64 Emulation) ///

     //  SDL Audio Format
     //  +----------------------sample is signed if set
     //  |
     //  |        +----------sample is bigendian if set
     //  |        |
     //  |        |           +--sample is float if set
     //  |        |           |
     //  |        |           |  +--sample bit size---+
     //  |        |           |  |                    |
     // 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0

     // Wunschformat
     // Muss aber nicht sein, das dieses auch verwendet wird
     // Wichtig ... Es muss alles aus audio_spec_have verwendetet werden

     SDL_memset(&audio_spec_want, 0, sizeof(audio_spec_want));
     audio_spec_want.freq = SAMPLERATE;
     audio_spec_want.format = AUDIO_F32;
     audio_spec_want.channels = 2;
     audio_spec_want.samples = 882;
     audio_spec_want.callback = AudioMix;
     audio_spec_want.userdata = this;

     #ifdef _WIN32
         if(getenv("SDL_AUDIODRIVER") == nullptr)
             putenv("SDL_AUDIODRIVER=directsound");

     #endif

     if( SDL_OpenAudio(&audio_spec_want, &audio_spec_have) > 0 )
     {
         LogText("- ERROR: Fehler beim installieren von SDL_Audio");
         return;
     }

     LogText("- SDL_Audio wurde installiert");

     audio_channels = audio_spec_have.channels;
     audio_sample_bit_size = audio_spec_have.format & 0x00ff;
     is_audio_sample_little_endian = audio_spec_have.format & 0x8000;
     is_audio_sample_float = audio_spec_have.format & 0x0100;
     is_audio_sample_signed = audio_spec_have.format & 0x1000;

     char out_text[1024];
     sprintf(out_text, "-   Audio Driver: %s" , getenv("SDL_AUDIODRIVER"));
     LogText(out_text);
     sprintf(out_text, "-   Audio Channels: %d" ,audio_channels);
     LogText(out_text);
     sprintf(out_text, "-   Audio Buffersize: %d" ,audio_spec_have.samples);
     LogText(out_text);
     sprintf(out_text, "-   Audio Sample Bit Size: %d",audio_sample_bit_size);
     LogText(out_text);
     sprintf(out_text, "-   Audio Sample Is Float: %d",is_audio_sample_float);
     LogText(out_text);
     sprintf(out_text, "-   Audio Sample Is Signed: %d",is_audio_sample_signed);
     LogText(out_text);
     sprintf(out_text, "-   Audio Sample Is Little Endian: %d",is_audio_sample_little_endian);
     LogText(out_text);

     SDL_PauseAudio(0);
}

void MainWindow::FillAudioBuffer(uint8_t *stream, int size)
{
    float *buffer = reinterpret_cast<float*>(stream);

    if((audio_sample_bit_size == 32) && is_audio_sample_float)
    {
        for(int i=0; i<(size / 4); i+=2)
        {
            float sample = vgm_player.GetNextSample();
            buffer[i] = sample;
            buffer[i+1] = sample;
        }
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),0, tr("Video Game Music Files (*.vgm; *.vgz) (*.vgm *.vgz)"));

    vgm_player.SetPlay(false);
    vgm_player.Open(fileName);
    vgm_player.SetPlay(true);
}
