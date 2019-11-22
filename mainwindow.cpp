#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    LogText("VGM Player - by Thorsten Kattanek");

    /// VGM Datei laden
    ///
    QFile file(":/vgm/vgm_demo");
    if(!file.open(QIODevice::ReadOnly))
    {
        LogText("- Fehler beim öffnen der Demo VGM Datei.");
    }
    else
    {
        LogText("- VGM Demo Datei wurde geöffnet. [" + QString::number(file.size()) + " Bytes]");
    }

    uint32_t file_ident;

    int nbytes = file.read(reinterpret_cast<char*>(&file_ident), 4);

    // 0x56 0x67 0x6d 0x20
    // "Vgm "
    if(file_ident == 0x206d6756)
    {
        LogText("- File ist eine VGM Datei.");
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

