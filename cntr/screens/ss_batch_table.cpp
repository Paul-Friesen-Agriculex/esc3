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

#include "ss_batch_table.hpp"
#include "batch_mode_driver.hpp"
#include <QHeaderView>
#include <QFile>
#include <QTextStream>

#include <iostream>

#include "centre.hpp"

using namespace std;
  
ss_batch_table::ss_batch_table(centre* c_p, batch_mode_driver* batch_mode_driver_p_s)
:QTableView(0)
{
  centre_p = c_p;
  batch_mode_driver_p = batch_mode_driver_p_s;
  model_row = 0;
  model_column = 0;
  current_row = batch_mode_driver_p->spreadsheet_line_number;
  int number_of_columns = batch_mode_driver_p->display_column_numbers.size();
  model_p = new QStandardItemModel(1,number_of_columns);
  setModel(model_p);
  for(int j=0; j<number_of_columns; ++j)
  {
    QStandardItem* item_p = new QStandardItem("");
    model_p -> setItem(0, j, item_p);
    int col_number = batch_mode_driver_p->display_column_numbers[j];
    spreadsheet_column* col_p = batch_mode_driver_p->get_spreadsheet_column_pointer(col_number);
    display_column_pointers.append(col_p);
    model_p -> setHeaderData(j, Qt::Horizontal, col_p->heading);
  }
  
  for(int i=0; i<batch_mode_driver_p->spreadsheet_number_of_lines; ++i)
  {
    for(int j=0; j<number_of_columns; ++j)
    {
      QString qs = display_column_pointers[j]->data_list[i];
      QStandardItem* item_p = new QStandardItem(qs);
      model_p -> setItem(i, j, item_p);
    }
  }
      
  setCurrentIndex(model_p->index(0,1));
  verticalHeader()->hide();
  
  resizeColumnsToContents();
  setFocusPolicy(Qt::NoFocus);
  setSelectionMode(QAbstractItemView::SingleSelection);
  
  setStyleSheet(
    "QTableView::QScrollBar::handle:vertical {"
     "width: 100px;"
     "min-height: 100px;}"
     
    "QStandardItem {"
        "font: 20px;}"
    
  );
  
  refresh();
}

ss_batch_table::~ss_batch_table()
{
  delete model_p;
  model_p = 0;
}

void ss_batch_table::set_current_row(int row)
{
  current_row = row;
}

void ss_batch_table::refresh()
{
  if(model_p!=0) delete model_p;
  model_p = 0;
  int number_of_columns = batch_mode_driver_p->display_column_numbers.size();
  model_p = new QStandardItemModel(1,number_of_columns);
  setModel(model_p);
  for(int j=0; j<number_of_columns; ++j)
  {
    QStandardItem* item_p = new QStandardItem("");
    model_p -> setItem(0, j, item_p);
    int col_number = batch_mode_driver_p->display_column_numbers[j];
    spreadsheet_column* col_p = batch_mode_driver_p->get_spreadsheet_column_pointer(col_number);
    display_column_pointers.append(col_p);
    model_p -> setHeaderData(j, Qt::Horizontal, col_p->heading);
  }

  for(int i=0; i<batch_mode_driver_p->spreadsheet_number_of_lines; ++i)
  {
    QColor row_colour(255,255,255);
//    if(   (batch_mode_driver_p->ss_first_column_p->data_list[i] == "Y")  ||  (batch_mode_driver_p->ss_first_column_p->data_list[i] == "R"))//packet for this row is filled.  colour dark green
    if(batch_mode_driver_p->ss_first_column_p->data_list[i] == "Y")//packet for this row is filled.  colour dark green
    {
      row_colour = QColor(100,230,100);
    }
    if(batch_mode_driver_p->ss_first_column_p->data_list[i] == "R")
    {
      row_colour = QColor(255,255,0);
    }
    if(i==current_row)//colour light green
    {
      row_colour = QColor(180,255,180);//light green
    }
    for(int j=0; j<number_of_columns; ++j)
    {
      QString qs = display_column_pointers[j]->data_list[i];
      QStandardItem* item_p = new QStandardItem(qs);
      item_p -> setBackground(QBrush(row_colour));
      model_p -> setItem(i, j, item_p);
    }
  }
  
  //to position current row near middle of screen, set current row first to row_back, then to row_forward, then to current_row, which is between them
  int row_back = current_row-4;
  if(row_back < 0) row_back = 0;
  int row_forward = current_row+4;
  if(row_forward >= model_p->rowCount()) row_forward = model_p->rowCount()-1;
  QModelIndex back_index = model_p->index(row_back,0);
  QModelIndex forward_index = model_p->index(row_forward,0);
  setCurrentIndex(back_index);
  setCurrentIndex(forward_index);
  selectionModel()->setCurrentIndex(model_p->index(current_row, 0), QItemSelectionModel::NoUpdate);
  setCurrentIndex(model_p->index(current_row, 0));

  resizeColumnsToContents();

}


