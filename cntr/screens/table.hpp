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

#ifndef table_view_hpp
#define table_view_hpp

#include <QStandardItemModel>
#include <QTableView>
#include <QKeyEvent>
#include <QString>

class centre;

class table:public QTableView
{
  Q_OBJECT
  
  private:
  int model_row, model_column;
  QStandardItemModel* model_p;
  centre* centre_p;

  friend class totalize;  //TEST~~~

  public:
  table(centre* centre_p_s);
  void set_visible_columns(int num);
  bool saved;//false will cause prompt to save table on exit or clear
  
  public slots:
  void enter_seeds(int num);
  void enter_weight(int num);
  void save_file(QString file_name);
  void load_file(QString file_name);
  void clear();
  
  protected:
  void keyPressEvent(QKeyEvent* event);
  void mouseReleaseEvent(QMouseEvent* e);
};
  
#endif
