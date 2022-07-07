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

#ifndef macro_screen_hpp
#define macro_screen_hpp

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QString>
#include <QMessageBox>			//help menu popup//
#include <QInputDialog>			//TEST~~~ alternative to dialog window, retrieve user input

#include "table.hpp"
#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QVBoxLayout;
class QLabel;
class QRadioButton;
class QSpinBox;
class help_screen;

class macro_screen : public screen
{
  Q_OBJECT
  
  public:
  macro_screen(centre*set_centre_p);
  ~macro_screen();
  QString text;
  
  private slots:
  void back_button_clicked();
  void ok_button_clicked();
  void help_button_clicked();
  void initialize_macro_menu();
  void cellSelected(int nRow, int nCol);
  void load_macro_table();
  void toggle_macro(int macro_number);

  private:
  button* back_button_p;
  button* ok_button_p;
  button* help_button_p;
  help_screen* help_screen_p;
  QLabel* instruction_label_p;
  QLineEdit *lineEdit;
  bool serialusb_connected;

  QGridLayout* main_layout_p;

  QTableWidget* tableWidget_p;
  QGroupBox *formGroupBox;

  QTableWidget* m_pTableWidget;
  QStringList m_TableHeader;
  QTableWidgetItem* tableWidgetItem_p;
  QLabel* tableWidget_label_p;
  QLabel* screen_title_label_p;
  QLabel* description_p;
  QMessageBox msgBox;
  static const int macro_cols = 5;
  static const int macro_rows = 10;
  centre* centre_p;

  int current_row;
  int current_column;
  
  protected:
  QString* macro_name_string;
};

#endif
  
