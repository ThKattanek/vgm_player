//////////////////////////////////////////////////
//                                              //
// VGM Player with QT                           //
// by thorsten kattanek                         //
//                                              //
// #file: mainwindow.h                          //
//                                              //
// last changes at 02-15-2020                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <SDL2/SDL.h>
#include "./vgmplayer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void FillAudioBuffer(uint8_t *stream, int size);

private slots:
    void on_actionOpen_triggered();

    void on_actionE_xit_triggered();

    void on_action_Export_Streaming_Data_triggered();

private:
    Ui::MainWindow *ui;

    VGMPlayer vgm_player;
    void LogText(const QString text);
    void InitSDLAudio();


    SDL_AudioSpec   audio_spec_want;
    SDL_AudioSpec   audio_spec_have;

    uint16_t        audio_sample_bit_size;
    uint16_t        audio_channels;
    bool            is_audio_sample_little_endian;
    bool            is_audio_sample_float;
    bool            is_audio_sample_signed;

};
#endif // MAINWINDOW_H
