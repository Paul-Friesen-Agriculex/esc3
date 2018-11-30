#include "table.hpp"
#include <QHeaderView>
#include <QFile>
#include <QTextStream>

#include <iostream>

#include "centre.hpp"

using namespace std;
  
table::table(centre* c_p)
:QTableView(0)
{
  centre_p = c_p;
  model_row = 0;
  model_column = 0;
  model_p = new QStandardItemModel(1,6);
  for(int j=0; j<6; ++j)
  {
    QStandardItem* item_p = new QStandardItem("");
    model_p -> setItem(0, j, item_p);
  }
  setModel(model_p);
  model_p -> setHeaderData(0, Qt::Horizontal, "barcode 1");
  model_p -> setHeaderData(1, Qt::Horizontal, "barcode 2");
  model_p -> setHeaderData(2, Qt::Horizontal, "barcode 3");
  model_p -> setHeaderData(3, Qt::Horizontal, "barcode 4");
  model_p -> setHeaderData(4, Qt::Horizontal, "count");
  model_p -> setHeaderData(5, Qt::Horizontal, "weight");

  setCurrentIndex(model_p->index(0,0));
//  cout<<"table::table svc ";
  set_visible_columns(centre_p->tm_barcode_columns);
  verticalHeader()->hide();
  
  resizeColumnsToContents();
  
}

void table::set_visible_columns(int num)
{
  
//  cout<<"set_visible_columns("<<num<<")\n";
  
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

void table::enter_seeds(int num)
{
  QStandardItem* item_p = new QStandardItem(QString("%1").arg(num));
  model_p -> setItem(model_row, 4, item_p);
  ++model_row;
  
  for(int j=0; j<6; ++j)
  {
    QStandardItem* item_p = new QStandardItem("");
    model_p -> setItem(model_row, j, item_p);
  }
  
  model_column = 0;
  QModelIndex next_index = model_p->index(model_row, model_column);
  setCurrentIndex(next_index);
  setFocus();
  resizeColumnsToContents();
}

void table::enter_weight(int num)
{
  QModelIndex current_index = currentIndex();
  int previous_row = model_row-1;	//weight will be optionally entered after count.  entry line will already have advanced.
  if(previous_row<0) return;		//not expected but could happen.  invalid.
  QStandardItem* item_p = new QStandardItem(QString("%1").arg(num));
  model_p -> setItem(previous_row, 5, item_p);
  setCurrentIndex(current_index);
  setFocus();
  resizeColumnsToContents();
}

void table::keyPressEvent(QKeyEvent* event)
{
  QTableView::keyPressEvent(event);
  if(event->key() == Qt::Key_Return)
  {
    if(model_column < (centre_p->tm_barcode_columns-1))
    {
      ++model_column;
    }
    
    QModelIndex next_index = model_p->index(model_row, model_column);
    setCurrentIndex(next_index);
    resizeColumnsToContents();
  }
}

void table::mouseReleaseEvent(QMouseEvent*)
{
  QModelIndex current_index = currentIndex();
  model_row = current_index.row();
  model_column = current_index.column();
}

void table::save_file(QString file_name)
{
  QFile file(file_name);
  if(file.open(QIODevice::WriteOnly | QIODevice::Text))
  { 
    
//    cout<<"saving file\n";
    
    QTextStream stream(&file);
    for(int i=0; i<model_p->rowCount()-1; ++i)
    {
      for(int j=0; j<6; ++j)
      {
        stream<<model_p->item(i,j)->text();
        if(j<5) stream<<", ";
        else stream<<"\n";
      }
    }
    file.close();
  }
  else cout<<"could not open save file\n";
  
}

void table::load_file(QString file_name)
{
  //cout<<"loading file\n";
  QFile file(file_name);
  if(file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    delete model_p;
    model_p = new QStandardItemModel(1,6);
    for(int j=0; j<6; ++j)
    {
      QStandardItem* item_p = new QStandardItem("");
      model_p -> setItem(0, j, item_p);
    }
    setModel(model_p);
    model_p -> setHeaderData(0, Qt::Horizontal, "barcode 1");
    model_p -> setHeaderData(1, Qt::Horizontal, "barcode 2");
    model_p -> setHeaderData(2, Qt::Horizontal, "barcode 3");
    model_p -> setHeaderData(3, Qt::Horizontal, "barcode 4");
    model_p -> setHeaderData(4, Qt::Horizontal, "count");
    model_p -> setHeaderData(5, Qt::Horizontal, "weight");
    
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
    set_visible_columns(centre_p->tm_barcode_columns);
    for(int j=0; j<6; ++j)
    {
      QStandardItem* item_p = new QStandardItem("");
      model_p -> setItem(model_row, j, item_p);
    }
    
    QModelIndex next_index = model_p->index(model_row, model_column);
    setCurrentIndex(next_index);
    resizeColumnsToContents();
    setFocus();
    
  }
  else cout<<"file failed to open for reading\n";
}

void table::clear()
{
  model_p->removeRows(0,model_p->rowCount());
  model_row = 0;
  model_column = 0;
  for(int j=0; j<6; ++j)
  {
    QStandardItem* item_p = new QStandardItem("");
    model_p -> setItem(0, j, item_p);
  }

  setCurrentIndex(model_p->index(0,0));
//  cout<<"table::clear svc ";
  set_visible_columns(centre_p->tm_barcode_columns);
  verticalHeader()->hide();
  
  resizeColumnsToContents();
  setFocus();
}
