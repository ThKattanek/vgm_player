//////////////////////////////////////////////////
//                                              //
// VGM Player with QT                           //
// by thorsten kattanek                         //
//                                              //
// #file: about_windows.h                       //
//                                              //
// last changes at 02-26-2023                   //
// https://github.com/ThKattanek/vgm_player     //
//                                              //
//////////////////////////////////////////////////

#ifndef ABOUT_WINDOW_H
#define ABOUT_WINDOW_H

#include <QDialog>

namespace Ui {
class AboutWindow;
}

class AboutWindow : public QDialog
{
	Q_OBJECT

public:
	explicit AboutWindow(QWidget *parent = nullptr);
	~AboutWindow();

private:
	Ui::AboutWindow *ui;
};

#endif // ABOUT_WINDOW_H
