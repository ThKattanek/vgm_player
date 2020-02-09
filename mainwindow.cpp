#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    LogText("VGM Player - by Thorsten Kattanek");

    if(!vgm_player.Open(":/vgm/vgm_demo"))
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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::LogText(const QString text)
{
    ui->log_output->appendPlainText(text);
}

