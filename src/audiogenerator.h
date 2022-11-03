//////////////////////////////////////////////////
//                                              //
// VGM Player with QT                           //
// by thorsten kattanek                         //
//                                              //
// #file: audiogenerator.h                      //
//                                              //
// last changes at 06-21-2020                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#ifndef AUDIOGENERATOR_H
#define AUDIOGENERATOR_H

#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include <QByteArray>
#include <QIODevice>

class AudioGenerator : public QIODevice {
    Q_OBJECT

signals:
void FillAudioData(char *, qint64);

public:
    AudioGenerator(const QAudioFormat &_format, QObject *parent);
    ~AudioGenerator();

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
    qint64 bytesAvailable() const;

private:
    QAudioFormat format;
    QByteArray m_buffer;
};

#endif // AUDIOGENERATOR_H
