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

#ifndef ss_batch_table_hpp
#define ss_batch_table_hpp

#include <QStandardItemModel>
#include <QTableView>
#include <QKeyEvent>
#include <QString>
#include <QList>

class centre;
class batch_mode_driver;
struct spreadsheet_column;

class ss_batch_table:public QTableView
{
  Q_OBJECT
  
  private:
  int model_row, model_column;
  int current_row;
  QStandardItemModel* model_p;
  centre* centre_p;
  batch_mode_driver* batch_mode_driver_p;
//  friend class batch;  //TEST~~~
  QList<spreadsheet_column*> display_column_pointers;

  public:
  ss_batch_table(centre* centre_p_s, batch_mode_driver* batch_mode_driver_p_s);
  ~ss_batch_table();
//  void set_visible_columns(int num);
  
  public slots:
  void set_current_row(int row);
//  void enter_data(int column, QString data);
  void refresh();
//  void enter_seeds(int num);
//  void enter_seed_lot_barcode(QString barcode);
//  void enter_pack_barcode(QString barcode);
//  void save_file(QString file_name);
//  void load_file(QString file_name);
//  void clear();
  
  signals:
  void focus_on_barcode();//signal to batch to return keyboard focus to barcode
  
};
  
#endif
