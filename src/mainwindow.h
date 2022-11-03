//////////////////////////////////////////////////
//                                              //
// VGM Player with QT                           //
// by thorsten kattanek                         //
//                                              //
// #file: mainwindow.h                          //
//                                              //
// last changes at 10-17-2022                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>

#include "./audiogenerator.h"
#include "./vgmplayer.h"

#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioOutput>

#define SOUND_BUFFER_SIZE 8192

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();
    void on_actionE_xit_triggered();
    void on_action_Export_Streaming_Data_triggered();
    void OnFillAudioData(char *data, qint64 len);
    void on_gb_write_reg_clicked();
	void on_sn76489_stereo_slider_valueChanged(int value);
	void on_sn76489_stereo_spin_valueChanged(double arg1);

private:
    Ui::MainWindow *ui;

    VGMPlayer vgm_player;
    void LogText(const QString text);
    void InitAudio();
    void ReleaseAudio();
	void FillGD3TagTable();

    unsigned int bufferSize;
    QAudioDeviceInfo m_device;
    QAudioFormat     m_format;
    QByteArray       m_buffer;
    QAudioOutput    *m_audioOutput;

    AudioGenerator *m_audiogen;
};
#endif // MAINWINDOW_H
