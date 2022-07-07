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

#ifndef keypad_hpp
#define keypad_hpp

#include <QWidget>

class QPushButton;
class QString;
class QLabel;
class QGridLayout;

class keypad:public QWidget
{
  Q_OBJECT
  
  public:
  keypad();
  ~keypad();
  void clear();
  
  public slots:
  void set_number(int num);
  
  private slots:
  void n0_clicked();
  void n1_clicked();
  void n2_clicked();
  void n3_clicked();
  void n4_clicked();
  void n5_clicked();
  void n6_clicked();
  void n7_clicked();
  void n8_clicked();
  void n9_clicked();
  void enter_clicked();
  void backspace_clicked();
  
  signals:
  void number_entered(int value);
  
  private:
  QLabel* display_p;
  QPushButton* n0_button_p;
  QPushButton* n1_button_p;
  QPushButton* n2_button_p;
  QPushButton* n3_button_p;
  QPushButton* n4_button_p;
  QPushButton* n5_button_p;
  QPushButton* n6_button_p;
  QPushButton* n7_button_p;
  QPushButton* n8_button_p;
  QPushButton* n9_button_p;
  QPushButton* enter_button_p;
  QPushButton* backspace_button_p;
  
  QGridLayout* layout_p;
  
  QString* entry_line_p;
};


#endif
