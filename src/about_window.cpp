//////////////////////////////////////////////////
//                                              //
// VGM Player with QT                           //
// by thorsten kattanek                         //
//                                              //
// #file: about_windows.cpp                     //
//                                              //
// last changes at 02-26-2023                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#include "about_window.h"
#include "ui_about_window.h"

AboutWindow::AboutWindow(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AboutWindow)
{
	ui->setupUi(this);

	setWindowTitle("About");

#ifdef _WIN32
	ui->architecture_label->setText("- Windows " + QString(ARCHITECTURE_STRING) + " -");
#else
# ifdef __linux__
	ui->architecture_label->setText("- Linux " + QString(ARCHITECTURE_STRING) + " -");
# else
#  ifdef __FreeBSD__
	ui->architecture_label->setText("- FreeBSD " + QString(ARCHITECTURE_STRING) + " -");
#  else
	ui->architecture_label->setText("- POSIX (unknown) " + QString(ARCHITECTURE_STRING) + " -");
#  endif
# endif
#endif

	ui->version_label->setText("Version: " + QString(VERSION_STRING));

}

AboutWindow::~AboutWindow()
{
	delete ui;
}
