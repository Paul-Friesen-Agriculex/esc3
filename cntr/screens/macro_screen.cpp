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

#include <iostream>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include "centre.hpp"
#include "macro_screen.hpp"
#include "button.hpp"
#include "help_screen.hpp"

#include <QHeaderView>
#include <fstream>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDialog>
#include <QSignalMapper>
#include <QTabWidget>

#include <fcntl.h>	//library used to use system call command "open()" used to check available serial
#include <unistd.h>	//library to enable write() function

#include <QTime>  //
#include <QTcpSocket>
  //explore some databases that may be used and spreadsheet programs, may need to include more features 
  //such as moving to or creating another sheet/file

using namespace std;

macro_screen::macro_screen(centre*set_centre_p)
	:screen(set_centre_p)
{
  
  cout<<"macro_screen::macro_screen 1\n";
  
  centre_p=set_centre_p;
  
  screen_title_label_p = new QLabel;
  description_p = new QLabel;

  back_button_p = new button("Back");
  ok_button_p = new button("OK");
  help_button_p = new button("Help");
  tableWidget_p = new QTableWidget(this);
  tableWidget_p->setRowCount(macro_rows);
  tableWidget_p->setColumnCount(macro_cols);
  m_TableHeader<<"On/Off"<<"#"<<"Name\nTouch cell to edit."<<"Macro\nTouch cell to edit."<<"Functions";
  tableWidget_p->setHorizontalHeaderLabels(m_TableHeader);
  tableWidget_p->verticalHeader()->setVisible(false);
  tableWidget_p->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tableWidget_label_p = new QLabel("Keyboard Output Macro(s)");
  
  tableWidget_p->setColumnWidth(1, 50);
  tableWidget_p->setColumnWidth(2,200);
  tableWidget_p->setColumnWidth(3, 355);		//change width of macro column if hiding function column
  tableWidget_p->setColumnHidden(4, true);	//hide macro name column
  
  main_layout_p = new QGridLayout;
  main_layout_p->addWidget(screen_title_label_p, 0, 0);
  main_layout_p->addWidget(description_p, 2, 0, 1, 4);
  main_layout_p->addWidget(back_button_p, 0, 3);
  main_layout_p->addWidget(help_button_p, 5, 2);
  main_layout_p->addWidget(ok_button_p, 5, 3);
  main_layout_p->addWidget(tableWidget_p,3,0,2,4);
  setLayout(main_layout_p);
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(ok_button_p, SIGNAL(clicked()), this, SLOT(ok_button_clicked()));
  connect(help_button_p, SIGNAL(clicked()), this, SLOT(help_button_clicked()));
  connect(tableWidget_p, SIGNAL(cellClicked(int, int)), this, SLOT(cellSelected(int, int)));
  
  if(centre_p->macro_type_for_entry == 0)
  {
    screen_title_label_p->setText("Totalize mode communication macros. Selected macro issued after each pack.");
  }
  if(centre_p->macro_type_for_entry == 1)
  {
    screen_title_label_p->setText("Batch mode macro issued after each pack, if there is no substitution.");
  }
  if(centre_p->macro_type_for_entry == 2)
  {
    screen_title_label_p->setText("Batch mode macro issued after seed lot is dumped.");
  }
  if(centre_p->macro_type_for_entry == 3)
  {
    screen_title_label_p->setText("Batch mode macro issued after each pack if a seed lot substitution is made.");
  }
  screen_title_label_p->setStyleSheet("QLabel { font: bold }");
  if(centre_p->communicate_by_keyboard_cable)
  {
    description_p->setText("If the special USB Communications cable is connected to a computer, \
items listed in the selected macro will be sent to the computer, which will see \
them as keyboard input.");
  }
//  if(centre_p->tcp_link_established)
  if(centre_p->communicate_by_tcp)
  {
    description_p->setText("A TCP connection has been established to another device. \
Items listed in the selected macro will be sent to that device.");
  }
  description_p->setWordWrap(true);
  
  if(centre_p->new_keyboard_entry == true) //returning after entering a macro name using a keyboard screen
  {        
    centre_p->new_keyboard_entry = false;
    centre_p->set_macro_name(centre_p->keyboard_return_string);
  }

  initialize_macro_menu();
  load_macro_table(); 
  cout<<"tableWidget_p->styleSheet = "<<(tableWidget_p->styleSheet()).toStdString()<<endl;  
//  tableWidget_p->setStyleSheet("QTableWidget { font: 15pt;}");
  tableWidget_p->setStyleSheet("QTableWidget { font: 10pt;}");
  cout<<"tableWidget_p->styleSheet = "<<(tableWidget_p->styleSheet()).toStdString()<<endl;
//  tableWidget_p->verticalHeader()->setDefaultSectionSize(45);
  tableWidget_p->verticalHeader()->setDefaultSectionSize(80);
  
  help_screen_p = 0;
  current_row = 0;
  current_column = 0;
}

macro_screen::~macro_screen()
{
  centre_p->save_macros();
}

void macro_screen::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void macro_screen::ok_button_clicked()
{
  centre_p->screen_done = true;//go to waiting screen
}

void macro_screen::help_button_clicked()
{ 
  help_screen_p = new help_screen();
  help_screen_p -> setGeometry(geometry());			   
  if(centre_p->macro_type_for_entry == 0)
  {
    help_screen_p->set_text("The ESC-3 is able to send data to a computer through the USB Communications Cable provided, "
           "or via a TCP link over an ethernet cable.  "
           "The method of communication is chosen in another screen.  \n\n"
           
           "If the USB communications cable is used, the data sent will appear to the computer as keyboard input.  This allows flexible "
           "entry into spreadsheets or database programs.\n\n"
           
           "If you think this might work for you, try entering data of the sort you wish to use in your computer "
           "using the keyboard only (no use of the mouse).  Often, you can use TAB, ENTER, or ARROW characters to "
           "move from field to field.  If you can do this, the ESC-3 may be able to do it automatically.\n\n"
           
           "Connect the communications cable between the ESC-3 and the computer.  The label on the cable "
           "indicates which end goes where.\n\n"
           
           "Communicating using a TCP link requires a program on the other computer capable of communication by this method.\n\n"
           
           "Select a cell under NAME "
           "and enter a name for your macro.\n\n"
           
           "Select the corresponding cell under MACRO.  Build your macro by touching the buttons.  The corresponding "
           "items will be sent out in the order they appear.\n\n"

           "In totalize mode, the ESC-3 sends out data after each sample.  It happens when a new line "
           "starts in the table on the ESC-3 screen.");
           
  }
  if(centre_p->macro_type_for_entry == 1)//batch pack
  {
    help_screen_p->set_text("The ESC-3 is able to send data to a computer through the USB Communications Cable provided, "
           "or via a TCP link over an ethernet cable.  "
           "The method of communication is chosen in another screen.  \n\n"
           
           "If the USB communications cable is used, the data sent will appear to the computer as keyboard input.  This allows flexible "
           "entry into spreadsheets or database programs.\n\n"
           
           "If you think this might work for you, try entering data of the sort you wish to use in your computer "
           "using the keyboard only (no use of the mouse).  Often, you can use TAB, ENTER, or ARROW characters to "
           "move from field to field.  If you can do this, the ESC-3 may be able to do it automatically.\n\n"
           
           "Connect the communications cable between the ESC-3 and the computer.  The label on the cable "
           "indicates which end goes where.\n\n"
           
           "Communicating using a TCP link requires a program on the other computer capable of communication by this method.\n\n"
           
           "Select a cell under NAME "
           "and enter a name for your macro.\n\n"
           
           "Select the corresponding cell under MACRO.  Build your macro by touching the buttons.  The corresponding "
           "items will be sent out in the order they appear.\n\n"

           "The ESC-3 sends out the data entered here after each pack in batch mode, as long as there is no substitution.  "
           "If you use a substitute seed lot because of lack of seed, the alternative message set up in another screen will be sent instead.  ");
           
  }
  if(centre_p->macro_type_for_entry == 2)//batch dump
  {
    help_screen_p->set_text("The ESC-3 is able to send data to a computer through the USB Communications Cable provided, "
           "or via a TCP link over an ethernet cable.  "
           "The method of communication is chosen in another screen.  \n\n"
           
           "If the USB communications cable is used, the data sent will appear to the computer as keyboard input.  This allows flexible "
           "entry into spreadsheets or database programs.\n\n"
           
           "If you think this might work for you, try entering data of the sort you wish to use in your computer "
           "using the keyboard only (no use of the mouse).  Often, you can use TAB, ENTER, or ARROW characters to "
           "move from field to field.  If you can do this, the ESC-3 may be able to do it automatically.\n\n"
           
           "Connect the communications cable between the ESC-3 and the computer.  The label on the cable "
           "indicates which end goes where.\n\n"
           
           "Communicating using a TCP link requires a program on the other computer capable of communication by this method.\n\n"
           
           "Select a cell under NAME "
           "and enter a name for your macro.\n\n"
           
           "Select the corresponding cell under MACRO.  Build your macro by touching the buttons.  The corresponding "
           "items will be sent out in the order they appear.\n\n"
           
           "The data entered here will be sent after each seed lot is dumped.  "
           "You can include, for instance, the number of seeds that are dumped, for inventory purposes.");
           
  }
  if(centre_p->macro_type_for_entry == 3)//batch substitution
  {
    help_screen_p->set_text("The ESC-3 is able to send data to a computer through the USB Communications Cable provided, "
           "or via a TCP link over an ethernet cable.  "
           "The method of communication is chosen in another screen.  \n\n"
           
           "If the USB communications cable is used, the data sent will appear to the computer as keyboard input.  This allows flexible "
           "entry into spreadsheets or database programs.\n\n"
           
           "If you think this might work for you, try entering data of the sort you wish to use in your computer "
           "using the keyboard only (no use of the mouse).  Often, you can use TAB, ENTER, or ARROW characters to "
           "move from field to field.  If you can do this, the ESC-3 may be able to do it automatically.\n\n"
           
           "Connect the communications cable between the ESC-3 and the computer.  The label on the cable "
           "indicates which end goes where.\n\n"
           
           "Communicating using a TCP link requires a program on the other computer capable of communication by this method.\n\n"
           
           "Select a cell under NAME "
           "and enter a name for your macro.\n\n"
           
           "Select the corresponding cell under MACRO.  Build your macro by touching the buttons.  The corresponding "
           "items will be sent out in the order they appear.\n\n"
           
           "This allows you to establish what the ESC-3 should send out after each packet, if a substitution has been made.");
           
  }
  help_screen_p->show();
}

void macro_screen::initialize_macro_menu()
{
  for (int i=0; i < macro_rows; ++i)
  {
    for (int j=0; j < macro_cols; ++j)
    {
      QTableWidgetItem *item = new QTableWidgetItem;
      item->setText("");
      item->setTextAlignment (Qt::AlignCenter);
      tableWidget_p->setItem(i, j, item);
            
      if(j==0)
      {
        tableWidget_p->item(i, 0)->setCheckState ( Qt::Unchecked );
        QLabel *on_off_label_p = new QLabel("OFF");
        tableWidget_p->setCellWidget (i, j, on_off_label_p);//QTableWidget automatically deletes any previous label
        on_off_label_p->setAlignment(Qt::AlignCenter);
      }
      if(j==1)
      {
        tableWidget_p->item(i, 1)->setText(QString("%1").arg(i+1));
      }
      if(j==2)
      {
        tableWidget_p->item(i, 2)->setText(QString("-"));
      }
      if(j==3)
      {
        tableWidget_p->item(i, 3)->setText(QString("-"));
      }
      if(j==4)
      {
        tableWidget_p->item(i, 4)->setText(QString("-"));
      }
    }
  }
}
 
void macro_screen::load_macro_table()
{
  for(int i=0; i<macro_rows && i<centre_p->number_of_macros(); ++i)
  {
    centre_p->macro_row_for_entry = i;
    bool selected = centre_p->get_macro_selection();
    QString name = centre_p->get_macro_name();
    QString display = centre_p->get_macro_display_string();
    if(selected) 
    {
      tableWidget_p->item(i, 0)->setCheckState (Qt::Checked);
      QLabel *on_off_label_p = new QLabel("ON");
      on_off_label_p->setAlignment(Qt::AlignCenter);
      tableWidget_p->setCellWidget (i, 0, on_off_label_p);//QTableWidget automatically deletes any previous label
    }
    else 
    {
      tableWidget_p->item(i, 0)->setCheckState (Qt::Unchecked);
      QLabel *on_off_label_p = new QLabel("OFF");
      on_off_label_p->setAlignment(Qt::AlignCenter);
      tableWidget_p->setCellWidget (i, 0, on_off_label_p);//QTableWidget automatically deletes any previous label
    }
    tableWidget_p->item(i,2)->setText(name);
    tableWidget_p->item(i,3)->setText(display);
  }
}

void macro_screen::cellSelected(int nRow, int nCol)	
{
  current_row = nRow;
  current_column = nCol;
  if(nCol == 0)
  {
    toggle_macro(nRow);
  }
  else if(nCol == 2)
  { 
    centre_p->new_keyboard_entry = true;
    centre_p -> keyboard_message_string = "Enter Macro Name";
    centre_p -> add_waiting_screen(28);//come back here
    centre_p -> add_waiting_screen(100);//keyboard
    centre_p -> macro_row_for_entry = nRow;
	  centre_p -> screen_done=true;
  }
  else if(nCol == 3)
  {    
    centre_p->macro_row_for_entry = nRow;
    centre_p->add_waiting_screen(38);//macro_builder
    centre_p->screen_done = true;
  }
}

void macro_screen::toggle_macro(int macro_number)
{
  centre_p->macro_row_for_entry = macro_number;
  bool original_state = centre_p->get_macro_selection();
  //de-select all macros of current type
  for(int i=0; i<centre_p->number_of_macros(); ++i)
  {
    centre_p->macro_row_for_entry = i;
    centre_p->set_macro_selection(false);
  }
  centre_p->macro_row_for_entry = macro_number;
  if(original_state == false) centre_p->set_macro_selection(true);
  load_macro_table();
}
  

