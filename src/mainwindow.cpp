//////////////////////////////////////////////////
//                                              //
// VGM Player with QT                           //
// by thorsten kattanek                         //
//                                              //
// #file: mainwindow.cpp                        //
//                                              //
// last changes at 10-17-2022                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStyle>
#include <QDesktopWidget>

#define SAMPLERATE 44100

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	ui->gd3_tag_table->setColumnWidth(0,500);

	for(int i=0; i<7; i++)
		ui->gd3_tag_table->setItem(i,0, new QTableWidgetItem());

    this->setWindowTitle("VGM Player - by Thorsten Kattanek");

	setGeometry( QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter,size(),  qApp->desktop()->availableGeometry()));

    LogText("VGM Player - by Thorsten Kattanek\n");

    ui->oscilloscope->setFixedSize(10*40, 8*40);
    ui->oscilloscope->SetSamplerate(SAMPLERATE);
    ui->oscilloscope->SetVerticalPosition(0.5f);
    ui->oscilloscope->SetTriggerTyp(TRIGGER_TYP::RISING_EDGE);
    ui->oscilloscope->SetTriggerLevel(0);
    ui->oscilloscope->SetVoltPerDivision(0.25f);

	ui->sn76489_stereo_slider->setValue(75);

    InitAudio();
}

MainWindow::~MainWindow()
{
    ReleaseAudio();
    delete ui;
}

void MainWindow::LogText(const QString text)
{
    ui->log_output->appendPlainText(text);
}

void MainWindow::InitAudio()
{
    bufferSize = SOUND_BUFFER_SIZE * 2;

    m_format.setSampleRate(44100);
    m_format.setChannelCount(2);
    m_format.setSampleSize(32);
    m_format.setCodec("audio/pcm");
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setSampleType(QAudioFormat::SampleType::Float);

    bool is_supported_format = false;

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(m_format))
    {
        qWarning() << "Default format not supported (44100/Stereo/Float)";

        // Second Audioformat (44100/Stereo/Signed Int 16Bit)
        m_format.setSampleSize(16);
        m_format.setSampleType(QAudioFormat::SampleType::SignedInt);
        if (!info.isFormatSupported(m_format))
        {
            qWarning() << "Second format not supported (44100/Stereo/Signed Int 16Bit)";
        }
        else
        {
             qInfo() << "Current Audioformat: 44100/Stereo/Signed Int 16Bit";
             is_supported_format = true;
        }
    }
    else{
        qInfo() << "Current Audioformat: 44100/Stereo/Float";
        is_supported_format = true;
    }

    if(is_supported_format)
    {
        m_device = QAudioDeviceInfo::defaultOutputDevice();
        m_buffer = QByteArray(bufferSize*2, 0);

        m_audioOutput = new QAudioOutput(m_device, m_format, this);
        m_audioOutput->setBufferSize(bufferSize);
        m_audiogen = new AudioGenerator(m_format, this);

        connect(m_audiogen, SIGNAL(FillAudioData(char*, qint64)), this, SLOT(OnFillAudioData(char*, qint64)));

        m_audiogen->start();
        m_audioOutput->start(m_audiogen);

        m_audioOutput->setVolume(1);
    }
}

void MainWindow::ReleaseAudio()
{
    disconnect(m_audiogen, SIGNAL(FillAudioData(char*, qint64)), this, SLOT(OnFillAudioData(char*, qint64)));
    m_audioOutput->stop();
	delete m_audioOutput;
}

void MainWindow::FillGD3TagTable()
{
	if(vgm_player.isOpen())
	{
		ui->gd3_tag_table->item(0,0)->setText(vgm_player.gd3_trackname_en);
		ui->gd3_tag_table->item(1,0)->setText(vgm_player.gd3_gamename_en);
		ui->gd3_tag_table->item(2,0)->setText(vgm_player.gd3_systemname_en);
		ui->gd3_tag_table->item(3,0)->setText(vgm_player.gd3_trackautor_en);
		ui->gd3_tag_table->item(4,0)->setText(vgm_player.gd3_releasedate);
		ui->gd3_tag_table->item(5,0)->setText(vgm_player.gd3_convertername);
		ui->gd3_tag_table->item(6,0)->setText(vgm_player.gd3_notes);
	}
}

void MainWindow::OnFillAudioData(char *data, qint64 len)
{
    float *buffer = reinterpret_cast<float*>(data);

    float sample_left, sample_right;

    for(int i=0; i<(len / (m_format.sampleSize()/8)); i+=2)
    {
        vgm_player.GetNextSample(&sample_left, &sample_right);
        buffer[i] = sample_left;
        buffer[i+1] = sample_right;
    }

    ui->oscilloscope->NextAudioData(reinterpret_cast<float*>(data), len / (m_format.sampleSize()/8));
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),0, tr("Video Game Music Files (*.vgm; *.vgz) (*.vgm *.vgz)"));

    vgm_player.SetPlay(false);
    if(vgm_player.Open(fileName))
    {
        vgm_player.SetPlay(true);

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
        LogText("- GameBoy DMG Clock: " + QString::number(vgm_player.GetGB_DMGClock()) + " Hz");
        LogText("- VGM Data Offset: 0x" + QString::number(vgm_player.GetVGMDataOffset(),16));

		FillGD3TagTable();
    }
}

void MainWindow::on_actionE_xit_triggered()
{
    this->close();
}

void MainWindow::on_action_Export_Streaming_Data_triggered()
{
    if(vgm_player.isOpen())
    {
        QString filename = QFileDialog::getSaveFileName(this, tr("Save Streaming Data"),0, tr("RAW VGM Streaming Data (*.raw) (*.raw)"));
        if(filename != "")
        {
            vgm_player.ExportStreamingData(filename);
        }
    }
    else
        QMessageBox::information(this,tr("VGM Player Info"),tr("VGM File is not open!"));
}

void MainWindow::on_gb_write_reg_clicked()
{
    vgm_player.WriteGBDMGRegister(ui->gb_reg_num->value(), ui->gb_reg_value->value());
}

void MainWindow::on_sn76489_stereo_slider_valueChanged(int value)
{
	ui->sn76489_stereo_spin->setValue(value);
	vgm_player.Set_SN76489_StereoStrength((100-value)/100.0f);
}


void MainWindow::on_sn76489_stereo_spin_valueChanged(double value)
{
	ui->sn76489_stereo_slider->setValue(value);
	vgm_player.Set_SN76489_StereoStrength((100-value)/100.0f);
}
