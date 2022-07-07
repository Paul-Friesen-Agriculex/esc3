/***********

Copyright 2021 Agriculex Inc.

This file is part of the Agriculex ESC-3 program.

The Agriculex ESC-3 program is free software: you can redistribute it 
and/or modify it under the terms of the GNU General Public License as 
published by the Free Software Foundation, GPL-3.0-or-later.

The Agriculex ESC-3 program is distributed in the hope that it will be 
useful, but, except as noted below, WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with The Agriculex ESC-3 program.  If not, see 
<https://www.gnu.org/licenses/>.

Agriculex Inc. warrants that executable files of the ESC-3 program 
compiled and installed by Agriculex Inc. will function as advertized in 
ESC-3 machines produced by Agriculex Inc.

**********/

#ifndef diagnostics_console_hpp
#define diagnostics_console_hpp

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QString>
#include <QLabel>
#include <QTimer>

class centre;

class diagnostics_console : public QWidget
{
  Q_OBJECT
  
  public:  
  diagnostics_console(centre* centre_p_s, QWidget* parent = 0);
  
  public slots:
  void receive_message1(QString message);
  void receive_message2(QString message);
  void receive_message3(QString message);
  void receive_message4(QString message);
  void receive_message5(QString message);
  void receive_message6(QString message);
  void receive_message7(QString message);
  
  private slots:
  void start_recording();
  void stop_recording();

  void run();
//  void reset_time_tests_button_clicked();
  
  signals:
  void reset_time_tests_signal();
  
  private:
  centre* centre_p;
  QPushButton* start_button_p;
  QPushButton* stop_button_p;
  QPushButton* reset_time_tests_button_p;

  QLabel* message1_p;
  QLabel* message2_p;
  QLabel* message3_p;
  QLabel* message4_p;
  QLabel* message5_p;
  QLabel* message6_p;
  QLabel* message7_p;
  QLabel* status_line_p;
  QVBoxLayout* layout_p;
  QTimer* timer_p;
};

#endif
