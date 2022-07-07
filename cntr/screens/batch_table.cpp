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

#include "batch_table.hpp"
#include "batch_mode_driver.hpp"
#include <QHeaderView>
#include <QFile>
#include <QTextStream>

#include <iostream>

#include "centre.hpp"

using namespace std;
  
batch_table::batch_table(centre* c_p, batch_mode_driver* batch_mode_driver_p_s)
:QTableView(0)
{
  centre_p = c_p;
  batch_mode_driver_p = batch_mode_driver_p_s;
  model_row = 0;
  model_column = 0;
  filled_rows = 0;
//  latest_seed_lot_barcode_row = 0;
  model_p = new QStandardItemModel(1,4);
  for(int j=0; j<4; ++j)
  {
    QStandardItem* item_p = new QStandardItem("");
    model_p -> setItem(0, j, item_p);
  }
  setModel(model_p);
  model_p -> setHeaderData(0, Qt::Horizontal, "Lot");
  model_p -> setHeaderData(1, Qt::Horizontal, "Pack");
  model_p -> setHeaderData(2, Qt::Horizontal, "Count");
  model_p -> setHeaderData(3, Qt::Horizontal, "Subst.");

  setCurrentIndex(model_p->index(0,1));
  verticalHeader()->hide();
  
  resizeColumnsToContents();
  setFocusPolicy(Qt::NoFocus);
  setSelectionMode(QAbstractItemView::SingleSelection);
  
  setStyleSheet(
    //"QTableView {"
        //"max-height: 300px;}"
    
    "QTableView::QScrollBar::handle:vertical {"
     //background: white;
     "width: 100px;"
     "min-height: 100px;}"
     
    //"QTableView::QScrollBar:vertical {"
      //"width: 100px; }"
    "QStandardItem {"
        "font: 15px;}"
    
  );
  //QStandardItem::setFont
  //model_p.item_p->setFont(20);
//  hideColumn(3);
}

batch_table::~batch_table()
{
  delete model_p;
  model_p = 0;
}

void batch_table::set_visible_columns(int num)
{
  if(num>4) num = 4;
  if(num<0) num = 0;
  
  for(int i=0; i<4; ++i)
  {
    hideColumn(i);
  }
  for(int i=0; i<num; ++i)
  {
    showColumn(i);
  }
  
  centre_p->tm_barcode_columns = num;
}
/*
void batch_table::prepare_for_seed_lot_barcode()
{
  model_column = 0;
  QModelIndex next_index = model_p->index(model_row, model_column);
  setCurrentIndex(next_index);
  setFocus();
  resizeColumnsToContents();
}

void batch_table::prepare_for_pack_barcode()
{
  ++model_row;
  
  for(int j=0; j<4; ++j)
  {
    QStandardItem* item_p = new QStandardItem("");
    model_p -> setItem(model_row, j, item_p);
  }
  
  model_column = 1;
  QModelIndex next_index = model_p->index(model_row, model_column);
  setCurrentIndex(next_index);
  setFocus();
  resizeColumnsToContents();
}
*/
void batch_table::enter_seeds(int num)
{
//  QStandardItem* item_p = new QStandardItem(QString("%1").arg(num));
//  model_p -> setItem(model_row, 2, item_p);
  
//  cout<<"batch_table::enter_seeds("<<num<<")\n";
  
  model_p -> item(model_row, 2) -> setData(QVariant(num), Qt::DisplayRole);
  model_column = 0;
  QString seed_lot_barcode;
  if(batch_mode_driver_p->substitute_seed_lot)
  {
    seed_lot_barcode = batch_mode_driver_p->substitute_barcode;
    model_p -> item(model_row, 3) -> setData(QVariant(seed_lot_barcode), Qt::DisplayRole);
  }
  else
  {
    seed_lot_barcode = batch_mode_driver_p->seed_lot_barcode;
  }
  model_p -> item(model_row, model_column) -> setData(QVariant(seed_lot_barcode), Qt::DisplayRole);
  
  
  ++model_row;
  filled_rows = model_row;
  
  for(int j=0; j<4; ++j)
  {
    QStandardItem* item_p = new QStandardItem("");
    model_p -> setItem(model_row, j, item_p);
  }
  
  model_column = 1;
  QModelIndex next_index = model_p->index(model_row, model_column);
  setCurrentIndex(next_index);
  resizeColumnsToContents();
  emit focus_on_barcode();
}

void batch_table::enter_seed_lot_barcode(QString barcode)
{
  
  cout<<"start batch_table::enter_seed_lot_barcode\n";
  
  ++model_row;
  filled_rows = model_row;
  for(int j=0; j<4; ++j)
  {
    QStandardItem* item_p = new QStandardItem("");
    model_p -> setItem(model_row, j, item_p);
  }
  
  model_column = 0;
  model_p -> item(model_row, model_column) -> setData(QVariant(barcode), Qt::DisplayRole);
  model_column = 1;
  model_p -> item(model_row, model_column) -> setData(QVariant("LOAD"), Qt::DisplayRole);
  resizeColumnsToContents();
  
  ++model_row;  
  filled_rows = model_row;
  for(int j=0; j<4; ++j)
  {
    QStandardItem* item_p = new QStandardItem("");
    model_p -> setItem(model_row, j, item_p);
  }
  
  model_column = 1;
  QModelIndex next_index = model_p->index(model_row, model_column);
  setCurrentIndex(next_index);
  
  centre_p->lotcode_str = barcode;
  batch_mode_driver_p->substitute_seed_lot = false;

  emit focus_on_barcode();
  
}

void batch_table::enter_pack_barcode(QString barcode)
{
//  ++model_row;
//  for(int j=0; j<4; ++j)
//  {
//    QStandardItem* item_p = new QStandardItem("");
//    model_p -> setItem(model_row, j, item_p);
//  }
  
  model_column = 1;
  QModelIndex next_index = model_p->index(model_row, model_column);
  setCurrentIndex(next_index);
  model_p -> item(model_row, model_column) -> setData(QVariant(barcode), Qt::DisplayRole);
  resizeColumnsToContents();
  
  centre_p->bar_str_1 = barcode;
  
//  cout<<"batch_table::enter_pack_barcode.  substitute_seed_lot = "<<batch_mode_driver_p->substitute_seed_lot<<endl;
  
//  if(batch_mode_driver_p->substitute_seed_lot)
//  {
//    enter_substitution_barcode(batch_mode_driver_p->substitute_barcode);
//  }
  
  emit focus_on_barcode();

//  ++model_row;
//  for(int j=0; j<4; ++j)
//  {
//    QStandardItem* item_p = new QStandardItem("");
//    model_p -> setItem(model_row, j, item_p);
//  }
}

void batch_table::enter_substitution_barcode(QString barcode)
{
  model_column = 3;
  QModelIndex next_index = model_p->index(model_row, model_column);
  setCurrentIndex(next_index);
  model_p -> item(model_row, model_column) -> setData(QVariant(barcode), Qt::DisplayRole);
  resizeColumnsToContents();
  
  
  
//  centre_p->substitution_str = barcode;
//  cout<<"batch_table::enter_substitution_barcode.  substitution_str = "<<centre_p->substitution_str.toStdString()<<endl;
 
 
  emit focus_on_barcode();
}  

void batch_table::enter_dump_count(int num)
{
//  QStandardItem* item_p = new QStandardItem(QString("%1").arg(num));
//  model_p -> setItem(model_row, 2, item_p);
  
//  cout<<"batch_table::enter_seeds("<<num<<")\n";
  
  model_p -> item(model_row, 1) -> setData(QVariant("DUMP"), Qt::DisplayRole);
  model_p -> item(model_row, 2) -> setData(QVariant(num), Qt::DisplayRole);

  model_column = 0;
  QString seed_lot_barcode;
  if(batch_mode_driver_p->substitute_seed_lot)
  {
    seed_lot_barcode = batch_mode_driver_p->substitute_barcode;
  }
  else
  {
    seed_lot_barcode = batch_mode_driver_p->seed_lot_barcode;
  }
  model_p -> item(model_row, model_column) -> setData(QVariant(seed_lot_barcode), Qt::DisplayRole);
  
  ++model_row;
  filled_rows = model_row;
  
  for(int j=0; j<4; ++j)
  {
    QStandardItem* item_p = new QStandardItem("");
    model_p -> setItem(model_row, j, item_p);
  }
  
  model_column = 1;
  QModelIndex next_index = model_p->index(model_row, model_column);
  setCurrentIndex(next_index);
  resizeColumnsToContents();

//  batch_mode_driver_p->substitute_seed_lot = false;

  emit focus_on_barcode();
}

/*
void batch_table::keyPressEvent(QKeyEvent* event)
{
  cout<<"batch_table::keyPressEvent\n";
  QTableView::keyPressEvent(event);
  if(event->key() == Qt::Key_Return)
  {
    cout<<"about to emit barcode_entered\n";
    emit barcode_entered(model_p->item(model_row, model_column)->text());
    if(model_column < (centre_p->tm_barcode_columns-1))
    {
      ++model_column;
    }
    QModelIndex next_index = model_p->index(model_row, model_column);
    setCurrentIndex(next_index);
    resizeColumnsToContents();
  }
}

void batch_table::mouseReleaseEvent(QMouseEvent*)
{
  QModelIndex current_index = currentIndex();
  model_row = current_index.row();
  model_column = current_index.column();
}
*/
void batch_table::save_file(QString file_name)
{
  
//  cout<<"start batch_table::save_file("<<file_name.toStdString()<<")\n";
  
  QFile file(file_name);
  if(file.open(QIODevice::WriteOnly | QIODevice::Text))
  { 
    QTextStream stream(&file);
    
//    cout<<"batch_table::save_file.  filled_rows = "<<filled_rows<<endl;
    
    for(int i=0; i<filled_rows; ++i)
    {
//      cout<<"i="<<i<<endl;
      for(int j=0; j<4; ++j)
      {
//        cout<<"   item "<<model_p->item(i,j)->text().toStdString();
        
        stream<<model_p->item(i,j)->text();
        if(j<3) stream<<", ";
        else stream<<"\n";
      }
    }
    file.close();
  }
  else cout<<"could not open save file\n";
  
  
//  cout<<"end batch_table::save_file("<<file_name.toStdString()<<")\n";
  
}

void batch_table::load_file(QString file_name)
{
  
  cout<<"start batch_table::load_file("<<file_name.toStdString()<<")\n";
  
  QFile file(file_name);
  if(file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    delete model_p;
    model_p = new QStandardItemModel(1,4);
    for(int j=0; j<4; ++j)
    {
      QStandardItem* item_p = new QStandardItem("");
      model_p -> setItem(0, j, item_p);
    }
    setModel(model_p);
    model_p -> setHeaderData(0, Qt::Horizontal, "Lot");
    model_p -> setHeaderData(1, Qt::Horizontal, "Pack");
    model_p -> setHeaderData(2, Qt::Horizontal, "Count");
    model_p -> setHeaderData(3, Qt::Horizontal, "Subst.");
    
    char c;
    QString qs = "";
    model_row = 0;
    model_column = 0;
    while(!file.atEnd())
    {
      file.getChar(&c);
      if(c == ',')
      {
        QStandardItem* item_p = new QStandardItem(qs);
        model_p -> setItem(model_row, model_column, item_p);
        qs = "";
        ++model_column;
      }
      else if(c == '\n')
      {
        QStandardItem* item_p = new QStandardItem(qs);
        model_p -> setItem(model_row, model_column, item_p);
        qs = "";
        ++model_row;
        model_column = 0;
      }
      else if(c == ' ') continue;//ignore spaces
      else
      {
        qs += c;
      }
    }
    verticalHeader()->hide();
//    cout<<"table::load_file svc ";
//    set_visible_columns(centre_p->tm_barcode_columns);
//    for(int j=0; j<4; ++j)
//    {
//      QStandardItem* item_p = new QStandardItem("");
//      model_p -> setItem(model_row, j, item_p);
//    }
    
    cout<<"batch_table::load_file.  Setting current index to ("<<model_row<<", "<<model_column<<").\n";
    
    filled_rows = model_row;
    
    for(int j=0; j<4; ++j)
    {
      QStandardItem* item_p = new QStandardItem("");
      model_p -> setItem(model_row, j, item_p);
    }

    QModelIndex next_index = model_p->index(model_row, model_column);
    setCurrentIndex(next_index);

    emit focus_on_barcode();
    
    
    
    resizeColumnsToContents();
//    setFocus();
    
  }
  else cout<<"file failed to open for reading\n";
  
//  hideColumn(3);
  
  cout<<"end batch_table::load_file("<<file_name.toStdString()<<")\n";
  
}

void batch_table::clear()
{
  model_p->removeRows(0,model_p->rowCount());
  model_row = 0;
  model_column = 0;
  for(int j=0; j<4; ++j)
  {
    QStandardItem* item_p = new QStandardItem("");
    model_p -> setItem(0, j, item_p);
  }

  setCurrentIndex(model_p->index(0,0));
  cout<<"table::clear svc ";
  verticalHeader()->hide();
  filled_rows = 0;
  
  resizeColumnsToContents();
//  setFocus();
}
/*
void batch_table::re_enter_seed_lot_barcode(QString barcode)//will enter new barcode in line latest_seed_lot_barcode_row
{
  barcode = barcode.trimmed();//remove whitespace at ends
  model_p -> item(latest_seed_lot_barcode_row, 0) -> setData(QVariant(barcode), Qt::DisplayRole);
  resizeColumnsToContents();
  emit focus_on_barcode();
}
*/  

