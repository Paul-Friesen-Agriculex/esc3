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

#ifndef batch_table_hpp
#define batch_table_hpp

#include <QStandardItemModel>
#include <QTableView>
#include <QKeyEvent>
#include <QString>

class centre;
class batch_mode_driver;

class batch_table:public QTableView
{
  Q_OBJECT
  
  private:
  int model_row, model_column;
//  int latest_seed_lot_barcode_row;
  int filled_rows;
  QStandardItemModel* model_p;
  centre* centre_p;
  batch_mode_driver* batch_mode_driver_p;
  friend class batch;  //TEST~~~

  public:
  batch_table(centre* centre_p_s, batch_mode_driver* batch_mode_driver_p_s);
  ~batch_table();
  void set_visible_columns(int num);
//  void prepare_for_seed_lot_barcode();
//  void prepare_for_pack_barcode();
  
  public slots:
  void enter_seeds(int num);
  void enter_seed_lot_barcode(QString barcode);
  void enter_pack_barcode(QString barcode);
  void enter_substitution_barcode(QString barcode);
  void enter_dump_count(int num);
  void save_file(QString file_name);
  void load_file(QString file_name);
  void clear();
//  void re_enter_seed_lot_barcode(QString barcode);//will enter new barcode in line latest_seed_lot_barcode_row
  
  signals:
  void focus_on_barcode();//signal to batch to return keyboard focus to barcode
  
//  void barcode_entered(QString value);
  /*
  protected:
  void keyPressEvent(QKeyEvent* event);
  void mouseReleaseEvent(QMouseEvent* e);
  */
};
  
#endif
