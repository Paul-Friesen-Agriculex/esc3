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
  
//  cout<<"bt1\n";
  
  centre_p = c_p;
  batch_mode_driver_p = batch_mode_driver_p_s;
  model_row = 0;
  model_column = 0;
  current_row = 0;
  int number_of_columns = batch_mode_driver_p->display_column_numbers.size();
  model_p = new QStandardItemModel(1,number_of_columns);
  setModel(model_p);
  
//  cout<<"bt2\n";
  
  for(int j=0; j<number_of_columns; ++j)
  {
    QStandardItem* item_p = new QStandardItem("");
    model_p -> setItem(0, j, item_p);
    int col_number = batch_mode_driver_p->display_column_numbers[j];
    spreadsheet_column* col_p = batch_mode_driver_p->get_spreadsheet_column_pointer(col_number);
    display_column_pointers.append(col_p);
    model_p -> setHeaderData(j, Qt::Horizontal, col_p->heading);
  }
  
//  cout<<"bt3\n";
  
  
  for(int i=0; i<batch_mode_driver_p->spreadsheet_number_of_lines; ++i)
  {
    for(int j=0; j<number_of_columns; ++j)
    {
      QString qs = display_column_pointers[j]->data_list[i];
      QStandardItem* item_p = new QStandardItem(qs);
      model_p -> setItem(i, j, item_p);
    }
  }
  
//  cout<<"bt4\n";
  
      
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
        "font: 20px;}"
    
  );
  
//  refresh();
  
//  cout<<"bt5\n";
  
  //QStandardItem::setFont
  //model_p.item_p->setFont(20);
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

/*
void ss_batch_table::enter_data(int column, QString data)
{
}
*/
void ss_batch_table::refresh()
{
  cout<<"ss_batch_table::refresh() 0\n";
  
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

//  cout<<"ss_batch_table::refresh() 1\n";
//  cout<<"batch_mode_driver_p->spreadsheet_number_of_lines = "<<batch_mode_driver_p->spreadsheet_number_of_lines<<endl;
  
  for(int i=0; i<batch_mode_driver_p->spreadsheet_number_of_lines; ++i)
  {
//    cout<<"p1\n";
    QColor row_colour(255,255,255);
//    cout<<"p2\n";
//    cout<<"display_column_pointers[0]->data_list[i] "<<display_column_pointers[0]->data_list[i].toStdString()<<endl;
//    if(display_column_pointers[0]->data_list[i] == "Y")//packet for this row is filled.  colour red
    if(batch_mode_driver_p->ss_first_column_p->data_list[i] == "Y")//packet for this row is filled.  colour red
    {
      row_colour = QColor(255,100,100);
    }
//    cout<<"p3\n";
    if(i==current_row)//colour yellow
    {
      row_colour = QColor(255,255,100);
    }
//    cout<<"p4\n";
//    cout<<"number_of_columns "<<number_of_columns<<endl;
    
    for(int j=0; j<number_of_columns; ++j)
    {
      QString qs = display_column_pointers[j]->data_list[i];
      QStandardItem* item_p = new QStandardItem(qs);
      item_p -> setBackground(QBrush(row_colour));
      model_p -> setItem(i, j, item_p);
    }
  }

//  cout<<"ss_batch_table::refresh() 2\n";

  
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


//  cout<<"ss_batch_table::refresh() 3\n";
  
  /*
  for(int i=0; i<batch_mode_driver_p->spreadsheet_number_of_lines; ++i)
  {
    showRow(i);
  }
  for(int i=0; i<current_row-5; ++i)
  {
    hideRow(i);
  }
  */
}

/*
void ss_batch_table::save_file(QString file_name)
{
  
//  cout<<"start ss_batch_table::save_file("<<file_name.toStdString()<<")\n";
  
  QFile file(file_name);
  if(file.open(QIODevice::WriteOnly | QIODevice::Text))
  { 
    QTextStream stream(&file);
    
//    cout<<"ss_batch_table::save_file.  filled_rows = "<<filled_rows<<endl;
    
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
  
  
//  cout<<"end ss_batch_table::save_file("<<file_name.toStdString()<<")\n";
  
}

void ss_batch_table::load_file(QString file_name)
{
  
  cout<<"start ss_batch_table::load_file("<<file_name.toStdString()<<")\n";
  
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
    model_p -> setHeaderData(0, Qt::Horizontal, "Lot code");
    model_p -> setHeaderData(1, Qt::Horizontal, "Pack code");
    model_p -> setHeaderData(2, Qt::Horizontal, "Count");
    model_p -> setHeaderData(3, Qt::Horizontal, "Substitution");
    
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
    
    cout<<"ss_batch_table::load_file.  Setting current index to ("<<model_row<<", "<<model_column<<").\n";
    
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
  }
  else cout<<"file failed to open for reading\n";
  
  cout<<"end ss_batch_table::load_file("<<file_name.toStdString()<<")\n";
  
}

void ss_batch_table::clear()
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
}
*/
