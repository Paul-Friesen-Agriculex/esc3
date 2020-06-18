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

#include <QtGui>  //included to implement delay in serial output//
#include <QTime>  //
#include <QTcpSocket>
  //explore some databases that may be used and spreadsheet programs, may need to include more features 
  //such as moving to or creating another sheet/file

//#include <QSize>      //OMIT~~~
//#include <QCheckBox>  //OMIT~~~

using namespace std;

macro_screen::macro_screen(centre*set_centre_p)
	:screen(set_centre_p)
{
  centre_p=set_centre_p;
  
  cout<<"1 centre_p = "<<centre_p<<endl;
  
  screen_title_label_p = new QLabel;
  description_p = new QLabel;

  back_button_p = new button("Back");
  ok_button_p = new button("OK");
  help_button_p = new button("Help");
  disable_all_button_p = new button("Disable All");
  enable_all_button_p = new button("Enable All");
  
  communications_choice_box_p = new QGroupBox;
  communicate_by_keyboard_cable_button_p = new QRadioButton("Communicate By Keyboard Cable");
  communicate_by_keyboard_cable_button_p -> setChecked(centre_p->communicate_by_keyboard_cable);
  communicate_by_tcp_button_p = new QRadioButton("Communicate By TCP");
  communicate_by_tcp_button_p -> setChecked(centre_p->communicate_by_tcp);
  tcp_setup_button_p = new button("Set up TCP communications");
  communications_choice_box_layout_p = new QGridLayout;
  communications_choice_box_layout_p -> addWidget(communicate_by_keyboard_cable_button_p, 0, 0);
  communications_choice_box_layout_p -> addWidget(communicate_by_tcp_button_p, 1, 0);
  communications_choice_box_layout_p -> addWidget(tcp_setup_button_p, 1, 1);
  communications_choice_box_p -> setLayout(communications_choice_box_layout_p);
  
  tableWidget_p = new QTableWidget(this);
  tableWidget_p->setRowCount(macro_rows);
  tableWidget_p->setColumnCount(macro_cols);
  m_TableHeader<<"On/Off"<<"#"<<"Name"<<"Macro"<<"Functions";
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
  main_layout_p->addWidget(communications_choice_box_p, 1, 0, 1, 4);
  main_layout_p->addWidget(description_p, 2, 0, 1, 4);
  main_layout_p->addWidget(back_button_p, 0, 3);
  main_layout_p->addWidget(help_button_p, 5, 2);
  main_layout_p->addWidget(ok_button_p, 5, 3);
  main_layout_p->addWidget(disable_all_button_p, 5, 0);
//  main_layout_p->addWidget(enable_all_button_p, 4, 1);
  main_layout_p->addWidget(tableWidget_p,3,0,2,4);
  setLayout(main_layout_p);
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(ok_button_p, SIGNAL(clicked()), this, SLOT(ok_button_clicked()));
  connect(help_button_p, SIGNAL(clicked()), this, SLOT(help_button_clicked()));
  connect(disable_all_button_p, SIGNAL(clicked()), this, SLOT(disable_all_clicked()));
  connect(enable_all_button_p, SIGNAL(clicked()), this, SLOT(enable_all_clicked()));
  connect(tableWidget_p, SIGNAL(cellClicked(int, int)), this, SLOT(cellSelected(int, int)));
  
  connect(communicate_by_keyboard_cable_button_p, SIGNAL(toggled(bool)), this, SLOT(communications_choice_toggled(bool)));
//  connect(communicate_by_tcp_button_p, SIGNAL(toggled()), this, SLOT(communications_choice_toggled()));

  screen_title_label_p->setText("Communications Menu");
  screen_title_label_p->setStyleSheet("QLabel { font: bold }");
  description_p->setText("If the special USB Communications cable is connected to a computer, \
items listed in the selected macro will be sent to the computer, which will see \
them as keyboard input.");
  description_p->setWordWrap(true);
  initialize_macro_menu();

  load_macro_table();  
  check_serial_connection();	//checks connection with serial communication line//
  
  //tableWidget_p->setDisabled(true);  
  tableWidget_p->setStyleSheet("QTableWidget { font: 15pt;}");
  tableWidget_p->verticalHeader()->setDefaultSectionSize(45);
  
  connect(tcp_setup_button_p, SIGNAL(clicked()), this, SLOT(tcp_setup_button_clicked()));
}

void macro_screen::check_serial_connection()
{
  int filedesc = open("/dev/usb2serial", O_WRONLY);
  if(filedesc < 0)
  {
    cout<<"serial NOT available"<<endl;
    serialusb_connected = false;
  }
  else
  {
	cout<<"serial available"<<endl;
	serialusb_connected = true;
	if(write(filedesc,"", 0) != 0){
    int system_int; //TEST~~~
    system_int = system("stty -F /dev/usb2serial cs8 9600 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts");
    cout<<system_int<<endl; //TEST~~~
    
	  if(write(filedesc,"", 0) != 0)
	  {
		cout<<"check connection"<<endl;
	  }
	  else
      {
	    cout<<"initialization sent"<<endl;
      }
    }
    else
    {
	  cout<<"initialization sent"<<endl;
    }
  }
}

void macro_screen::communications_choice_toggled(bool)
{
  centre_p->communicate_by_keyboard_cable = communicate_by_keyboard_cable_button_p->isChecked();
  centre_p->communicate_by_tcp = communicate_by_tcp_button_p->isChecked();
  
  cout<<"centre_p->communicate_by_keyboard_cable = "<<centre_p->communicate_by_keyboard_cable<<endl;
  cout<<"centre_p->communicate_by_tcp = "<<centre_p->communicate_by_tcp<<endl;
  
  
}

void macro_screen::tcp_setup_button_clicked()
{
  centre_p->add_waiting_screen(40);//tcp_mode_choice
  centre_p->screen_done=true;
}

void macro_screen::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void macro_screen::ok_button_clicked()
{
  centre_p->screen_done = true;
  cout<<"ok clicked\n";
}

void macro_screen::disable_all_clicked()
{
  cout<<endl<<"Disable_ALL()"<<endl;
  for(int j=0; j<10; ++j)
  {
    if(tableWidget_p->item(j, 0)->checkState())
    {
      tableWidget_p->item(j, 0)->setCheckState ( Qt::Unchecked );

      QLabel *on_off_label_p = new QLabel("OFF");
      tableWidget_p->setCellWidget (j, 0, on_off_label_p);
      on_off_label_p->setAlignment(Qt::AlignCenter);
                                    
      tableWidget_p->item(j,0)->setBackground(QColor(255, 255, 255));
      tableWidget_p->item(j,1)->setBackground(QColor(255, 255, 255));
      tableWidget_p->item(j,2)->setBackground(QColor(255, 255, 255));
      tableWidget_p->item(j,3)->setBackground(QColor(255, 255, 255));
      tableWidget_p->item(j,4)->setBackground(QColor(255, 255, 255));
    }
    store_macro_table();
  }
}

void macro_screen::enable_all_clicked()
{
  cout<<endl<<"Enable_ALL()"<<endl;
  for(int j=0; j<10; ++j)
  {
    if(!tableWidget_p->item(j, 0)->checkState())
    {
      tableWidget_p->item(j, 0)->setCheckState ( Qt::Checked );
      
      QLabel *on_off_label_p = new QLabel("ON");
      tableWidget_p->setCellWidget (j, 0, on_off_label_p);
      on_off_label_p->setAlignment(Qt::AlignCenter);
                                    
      tableWidget_p->item(j,0)->setBackground(QColor(154, 229, 154));
      tableWidget_p->item(j,1)->setBackground(QColor(154, 229, 154));
      tableWidget_p->item(j,2)->setBackground(QColor(154, 229, 154));
      tableWidget_p->item(j,3)->setBackground(QColor(154, 229, 154));
      tableWidget_p->item(j,4)->setBackground(QColor(154, 229, 154));
    }
    store_macro_table();
  }
}


/*
void macro_screen::help_button_clicked()
{ 
  QFormLayout *help_form = new QFormLayout;

  QTabWidget *tab_widget_box = new QTabWidget();
  tab_widget_box->setFixedWidth(800);
  tab_widget_box->setFixedHeight(300);
  tab_widget_box->show();
  
  QLabel *label_1 = new QLabel(this);
  /
  label_1->setText("* HELP BUTTON INTRODUCTION *\nThe macro builder will help export and format data "
				   "generated by the <CNTR Model> to an external database or data storage program.\n\n"
				   "1. Macros Menu table components:\nThis menu contains a table that will organize your "
				   "macros and provide information on how the exported data will be formatted. The four "
				   "columns headings include:\n\n"
				   "On/Off:\nThis column indicates whether the macro is active. When ON (indicated by a "
				   "green check mark in the check box), the macro in this row will be transmitted when a "
				   "seed count is to be transmitted. Simply click the cells in this column to toggle the "
				   "state of the macro.\n"
				   "#:\nThis column indicates the order the macros are transmitted when multiple macros "
				   "are active.\n"
				   "Name:\nThis column is used to create custom labels to identify each macro.\n"
				   "Macro:\nThis column is used to generate the macro commands. Macro commands contain a "
				   "sequence of keyboard keystrokes and specified variables for each counting mode.");
	*
  			   
  label_1->setText("The ESC-3 is able to send data to a computer through the USB Communications Cable provided.  "
           "The data sent will appear to the computer as keyboard input.  This allows flexible "
           "entry into spreadsheets or database programs.\n\n"
           
           "If you think this might work for you, try entering data of the sort you wish to use in your computer "
           "using the keyboard only (no use of the mouse).  Often, you can use TAB, ENTER, or ARROW characters to "
           "move from field to field.  If you can do this, the ESC-3 may be able to do it automatically.\n\n"
           
           "In totalize mode, the ESC-3 sends out data after each sample.  It happens when a new line "
           "starts in the table on the ESC-3 screen.");
           
  /         
  QLabel *label_2 = new QLabel(this);
  label_2->setText("Building your own macro:\nHardware Setup: Before using the macro builder, ensure the "
				   "end with the box compartment of the <USB keyboard emulator> is connected to the back "
				   "of the <CNTR Model> and the other end is connected to the target computer. (When connected, "
				   "an LED indicator on the <USB keyboard emulator> box compartment will turn on.)");
  *

  QLabel *label_2 = new QLabel(this);
  label_2->setText("Connect the communications cable between the ESC-3 and the computer.  The label on the cable "
           "indicates which end goes where.\n\n"
           
           "In totalize mode, select OPTIONS -> USB COMMUNICATIONS MENU.  Select a cell under NAME "
           "and enter a name for your macro.\n\n"
           
           "Select the corresponding cell under MACRO.  Build your maco by touching the buttons.  The corresponding "
           "items will be sent out in the order they appear.");


  /
  QLabel *label_3 = new QLabel(this);
  label_3->setText("General Notes/Tips:\nFor use with spreadsheets like Excel or LibreOffice Calc, using the arrow "
				   "keys is recommended for navigating to different cells in the spreadsheet.\n<note "
				   "implemented yet: You may also navigate between multiple sheets using Ctrl+PgUp/PgDn>\n"
				   "For use with databases with multiple input lines or buttons, using Tab and Ctrl+ Tab is "
				   "recommended for navigating to the next/previous element in the database.");
  *
  
  label_1->setWordWrap(true);
  label_2->setWordWrap(true);
//  label_3->setWordWrap(true);

  tab_widget_box->addTab(label_1,"Introduction");
  tab_widget_box->addTab(label_2,"Instructions");
//  tab_widget_box->addTab(label_3,"3");
  
  label_1->setAlignment(Qt::AlignTop);
  label_2->setAlignment(Qt::AlignTop);
//  label_3->setAlignment(Qt::AlignTop);

  //tab_widget_box->setStyleSheet("QTabBar::tab { height: 0px; width: 0px; }");	
  //tab_widget_box->tabBar()->setStyleSheet("QTabBar::tab:selected { font: bold;}");
  //tab_widget_box->tabBar()->hide();
  //tab_widget_box->tabBar()->setVisible(false);

  //help_form->addRow(msgBox);
  help_form->addRow(tab_widget_box);
  
  button* close_button_p = new button("Close");
  connect(close_button_p, SIGNAL(clicked()), help_form, SLOT(deleteLater()));
//  close_button_p -> show();
  help_form->addRow(close_button_p);
}
*/

void macro_screen::help_button_clicked()
{ 
  help_screen_p = new help_screen;
  			   
  help_screen_p->set_text("The ESC-3 is able to send data to a computer through the USB Communications Cable provided.  "
           "The data sent will appear to the computer as keyboard input.  This allows flexible "
           "entry into spreadsheets or database programs.\n\n"
           
           "If you think this might work for you, try entering data of the sort you wish to use in your computer "
           "using the keyboard only (no use of the mouse).  Often, you can use TAB, ENTER, or ARROW characters to "
           "move from field to field.  If you can do this, the ESC-3 may be able to do it automatically.\n\n"
           
           "In totalize mode, the ESC-3 sends out data after each sample.  It happens when a new line "
           "starts in the table on the ESC-3 screen.\n\n"
           
           "Connect the communications cable between the ESC-3 and the computer.  The label on the cable "
           "indicates which end goes where.\n\n"
           
           "In totalize mode, select OPTIONS -> USB COMMUNICATIONS MENU.  Select a cell under NAME "
           "and enter a name for your macro.\n\n"
           
           "Select the corresponding cell under MACRO.  Build your macro by touching the buttons.  The corresponding "
           "items will be sent out in the order they appear.");

  help_screen_p->show();
}


//modified to handle barcodes as characters instead of integers// 11_02_2018~~~//
void macro_screen::usb_serial_out(QString bar_str_1, QString bar_str_2, QString bar_str_3, QString bar_str_4, QString totalize_count_str, QString weight_str)
{
  
  cout<<"2 centre_p = "<<centre_p<<endl;
  
  cout<<endl<<"USB2SERIAL QStringVariant"<<endl;                                        //OMIT~~~
  cout<<"bar_1: "<<bar_str_1.toUtf8().constData();                                      //OMIT~~~
  cout<<"\tbar_2: "<<bar_str_2.toUtf8().constData();                                    //
  cout<<"\tbar_3: "<<bar_str_3.toUtf8().constData();                                    //
  cout<<"\tbar_4: "<<bar_str_4.toUtf8().constData()<<endl;                              //
//  cout<<endl<<"totalize_str_count: "<<totalize_count_str.toUtf8().constData()<<endl;    //
//  cout<<endl<<"totalize_str_weight: "<<weight_str.toUtf8().constData()<<endl;           //
  //----------------------------------------------------------------------------------------//
  
  bool macro_status_bool;			      //temporary variable to transfer ifstream to tablewidget
  int macro_numb_int;				        //
  char macro_name_char[30];			    //
  char macro_mask_char[30];			    //
  char macro_function_char[30];		  //
 
  //QString combined_macro_functions;	//new char to combine all macros 
  QString macro_string;
  QString combined_macro_functions;
  QString count_string;
  QString barcode_string;
  
  cout<<"3 centre_p = "<<centre_p<<endl;
  
        
  {
    ifstream macros("macro_table");
    for(int i=0; i<10; ++i)		//searches all 10 macros available
    {
  
      cout<<"usb_serial_out 1\n";
  
      cout<<"4 centre_p = "<<centre_p<<endl;
  
  
	    macros>>macro_status_bool;
	    macros>>macro_numb_int;
	    macros>>macro_name_char;
	    macros>>macro_mask_char;
	    macros>>macro_function_char;

	    if(macro_status_bool != 0)
      {
        QString macro_string;
	      for(unsigned int j=0; j<strlen(macro_function_char); ++j)
	      {
		      if(macro_function_char[j] == '\\')
		      {
            if(macro_function_char[j+1] == 'C')
	          {
	            macro_string = macro_string + totalize_count_str;
	          }
  	        else if(macro_function_char[j+1] == '1')		//Barcodes
	          {
              macro_string = macro_string + bar_str_1;
              cout<<"bar_1: "<<bar_str_1.toUtf8().constData()<<endl;  //OMIT~~~
  	        }
	          else if(macro_function_char[j+1] == '2')
	          {
              macro_string = macro_string + bar_str_2;
              cout<<"bar_2: "<<bar_str_2.toUtf8().constData()<<endl;  //OMIT~~~
	          }
	          else if(macro_function_char[j+1] == '3')
	          {
              macro_string = macro_string + bar_str_3;
              cout<<"bar_3: "<<bar_str_3.toUtf8().constData()<<endl;  //OMIT~~~
	          }
	          else if(macro_function_char[j+1] == '4')
	          {
              macro_string = macro_string + bar_str_4;
              cout<<"bar_4: "<<bar_str_4.toUtf8().constData()<<endl;  //OMIT~~~
	          }
  	        /*else if(macro_function_char[j+1] == 'T')		//Lot code
 	          {
	   		      //macro_string = macro_string + lotcode;
            }
	          else if(macro_function_char[j+1] == 'P')		//Pack code
	          {
			        //macro_string = macro_string + packcode;
	          }*/
	          else
	          {
			        macro_string = macro_string + macro_function_char[j] + macro_function_char[j+1];
	          }
	        }
	      }
        /*for(int j=0; j<strlen(macro_function_char); ++j)
        {
          macro_string.append(macro_function_char[j]);
        }*/
        combined_macro_functions = combined_macro_functions + macro_string;
	    }
	    if(macros.eof()) break;
    }
  
  cout<<"usb_serial_out 2\n";
  
    cout<<endl<<combined_macro_functions.toUtf8().constData()<<endl;  //OMIT~~~
  
  cout<<"usb_serial_out 3\n";
  
    macros.close();
  }
//--------------------------------------------------------------OUTPUT TO SERIAL--------------------------------------------------------------//
  
  cout<<"usb_serial_out 4\n";
  
  int size_string_macros = combined_macro_functions.size();
  
  cout<<"usb_serial_out 5\n";
  
  cout<<"2 centre_p = "<<centre_p<<endl;
  
  cout<<"centre_p->communicate_by_keyboard_cable = "<<centre_p->communicate_by_keyboard_cable<<endl;
  
  if(centre_p->communicate_by_keyboard_cable==true)
  {
    //int filedesc = open("/dev/TTL232RG", O_WRONLY);
    int filedesc = open("/dev/usb2serial", O_WRONLY);
    //int filedesc = open("/dev/ttyUSB0", O_WRONLY);  //if udev rules are not applied//
    //write(filedesc,(combined_macro_functions.toUtf8().constData()), size_string_macros) != size_string_macros;
  
    cout<<endl<<"serial string buffer length: "<<write(filedesc,(combined_macro_functions.toUtf8().constData()), size_string_macros)<<endl;
  }
  
  cout<<"usb_serial_out 6\n";
  
  if(centre_p->communicate_by_tcp==true)
  {
    cout<<"centre_p->tcp_socket_p = "<<centre_p->tcp_socket_p<<endl;
//    centre_p->tcp_write(combined_macro_functions);
//    centre_p->tcp_socket_p->write("test2\n");
//    cout<<"after write\n";
  }
  
  
  
//--------------------------------------------------------------------------------------------------------------------------------------------//
  //serial terminal write function with slight delay between commands//
  /*int write_output = 0;         //TEST~~~
  QString current_char_string;  //TEST~~~
  
  for(int xy = 0; xy< size_string_macros; ++xy)
  {
    if(combined_macro_functions.at(xy) == '\\')
    {
      QTime dieTime= QTime::currentTime().addMSecs(250);
      while (QTime::currentTime() < dieTime)
      {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
      }
      ++xy;
      if(combined_macro_functions.at(xy) == 'Q')
      {
        QTime dieTime= QTime::currentTime().addMSecs(350);
        while (QTime::currentTime() < dieTime)
        {
          QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
      }
      current_char_string = '\\' + combined_macro_functions.at(xy);
      write_output = write(filedesc, current_char_string.toUtf8().constData(), 2);
    }
    else
    {
      current_char_string = combined_macro_functions.at(xy);
      write_output = write(filedesc, current_char_string.toUtf8().constData(), 2);
    }
  }
  cout<<current_char_string.toUtf8().constData();  //OMIT~~~
  cout<<write_output<<endl; //TEST~~~*/
//--------------------------------------------------------------------------------------------------------------------------------------------//
}

void macro_screen::usb_serial_out(QString lotcode_str, QString packcode_str, QString batch_count_str, QString substitution_str, QString dump_count_str)
{
  
  /*
  cout<<endl<<"QString Variant BATCHMODE"<<endl;                         //OMIT~~~//
  cout<<"lotcode: "<<lotcode_str.toUtf8().constData();                   //OMIT~~~
  cout<<endl<<"packcode: "<<packcode_str.toUtf8().constData();           //
  cout<<endl<<"batch count: "<<batch_count_str.toUtf8().constData();     //
  cout<<endl<<"substitution: "<<substitution_str.toUtf8().constData();   //
  cout<<endl<<"dump count: "<<dump_count_str.toUtf8().constData();       //
  cout<<endl<<endl;                                                      //
  */
//----------------------------------------------------------------------------------------//
    
  bool macro_status_bool;			      //temporary variable to transfer ifstream to tablewidget
  int macro_numb_int;				        //
  char macro_name_char[300];			  //
  char macro_mask_char[300];			  //
  char macro_function_char[300];		//
 
  QString macro_string;
  QString combined_macro_functions;
  QString count_string;
  QString barcode_string;
        
  {
    ifstream macros("macro_table");
    for(int i=0; i<10; ++i)
    {
	    macros>>macro_status_bool;
	    macros>>macro_numb_int;
	    macros>>macro_name_char;
	    macros>>macro_mask_char;
	    macros>>macro_function_char;
    
	    if(macro_status_bool != 0)
      {
        QString macro_string;
	      for(unsigned int j=0; j<strlen(macro_function_char); ++j)
	      {
		      if(macro_function_char[j] == '\\')
		      {
            if(macro_function_char[j+1] == 'C')
	          {
	            macro_string = macro_string + batch_count_str;
	          }
  	        else if(macro_function_char[j+1] == 'T')		  //Lot code
 	          {
	   		      macro_string = macro_string + lotcode_str;
            }
	          else if(macro_function_char[j+1] == 'P')		  //Pack code
	          {
			        macro_string = macro_string + packcode_str;
	          }
            else if(macro_function_char[j+1] == 'Q')		  //Dump count
	          {
			        macro_string = macro_string + dump_count_str;
	          }
            //else if(macro_function_char[j+1] == '1') {}		//Barcodes
	          //else if(macro_function_char[j+1] == '2') {}
	          //else if(macro_function_char[j+1] == '3') {}
	          //else if(macro_function_char[j+1] == '4') {}
	          else
	          {
			        macro_string = macro_string + macro_function_char[j] + macro_function_char[j+1];
	          }
	        }
	      }
        /*for(int j=0; j<strlen(macro_function_char); ++j)
        {
          macro_string.append(macro_function_char[j]);
        }*/
        combined_macro_functions = combined_macro_functions + macro_string;
	    }
	    if(macros.eof()) break;
    }
//    cout<<endl<<combined_macro_functions.toUtf8().constData()<<endl;  //OMIT~~~
    macros.close();
  }  
//--------------------------------------------------------------OUTPUT TO SERIAL--------------------------------------------------------------//
  //cout<<"Macros loaded"<<endl;  //OMIT~~~
//  int size_string_macros = combined_macro_functions.size();
  //int filedesc = open("/dev/TTL232RG", O_WRONLY);
//  int filedesc = open("/dev/usb2serial", O_WRONLY);
  //int filedesc = open("/dev/ttyUSB0", O_WRONLY);  //if udev rules are not applied//
  //write(filedesc,(combined_macro_functions.toUtf8().constData()), size_string_macros) != size_string_macros;
  //write(filedesc,(combined_macro_functions.toUtf8().constData()), size_string_macros);
//  cout<<endl<<"serial string buffer length: "<<write(filedesc,(combined_macro_functions.toUtf8().constData()), size_string_macros)<<endl; //ORIGINAL~~~
//--------------------------------------------------------------------------------------------------------------------------------------------//
  //serial terminal write function with slight delay between commands//
  /*int write_output = 0; //TEST~~~
  QString current_char_string;
  
  for(int xy = 0; xy< size_string_macros; ++xy)
  {
    if(combined_macro_functions.at(xy) == '\\')
    {
      QTime dieTime= QTime::currentTime().addMSecs(250);
      while (QTime::currentTime() < dieTime)
      {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
      }
      ++xy;
      if(combined_macro_functions.at(xy) == 'Q')
      {
        QTime dieTime= QTime::currentTime().addMSecs(350);
        while (QTime::currentTime() < dieTime)
        {
          QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
      }
      current_char_string = '\\' + combined_macro_functions.at(xy);
      write_output = write(filedesc, current_char_string.toUtf8().constData(), 2);
    }
    else
    {
      current_char_string = combined_macro_functions.at(xy);
      write_output = write(filedesc, current_char_string.toUtf8().constData(), 2);
    }
  }
  //cout<<current_char_string.toUtf8().constData();  //OMIT~~~
  cout<<write_output<<endl; //TEST~~~*/
//--------------------------------------------------------------------------------------------------------------------------------------------//
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
        tableWidget_p->setCellWidget (i, j, on_off_label_p);
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
  //QTableView class, considering hiding actual function code
  //hideRow(), hideColumn(), showRow(), and showColumn()
  //tableWidget_p->setColumnWidth(0, (tableWidget_p->item(0, 0)->sizeHint()).width());  //TEST~~~    
}
 
void macro_screen::load_macro_table()
{  
  bool temp_state;
  int temp_num;
  char temp_name[300];
  char temp_mask[300];
  char temp_func[300];

  ifstream macros("macro_table");  
  
  if(macros)
  {
    for(int i=0; i<10; ++i)
    {
      macros>>temp_state;
      macros>>temp_num;
      macros>>temp_name;
      macros>>temp_mask;
      macros>>temp_func;
      
      if(temp_state == 0)
      {
        tableWidget_p->item(i, 0)->setCheckState ( Qt::Unchecked );
        QLabel *on_off_label_p = new QLabel("OFF");
        tableWidget_p->setCellWidget (i, 0, on_off_label_p);
        on_off_label_p->setAlignment(Qt::AlignCenter);

        tableWidget_p->item(i,0)->setBackground(QColor(255, 255, 255));
        tableWidget_p->item(i,1)->setBackground(QColor(255, 255, 255));
        tableWidget_p->item(i,2)->setBackground(QColor(255, 255, 255));
        tableWidget_p->item(i,3)->setBackground(QColor(255, 255, 255));
        tableWidget_p->item(i,4)->setBackground(QColor(255, 255, 255));
      }
      else
      {
        tableWidget_p->item(i, 0)->setCheckState ( Qt::Checked );
        QLabel *on_off_label_p = new QLabel("ON");
        tableWidget_p->setCellWidget (i, 0, on_off_label_p);
        on_off_label_p->setAlignment(Qt::AlignCenter);
                                      
        tableWidget_p->item(i,0)->setBackground(QColor(154, 229, 154));
        tableWidget_p->item(i,1)->setBackground(QColor(154, 229, 154));
        tableWidget_p->item(i,2)->setBackground(QColor(154, 229, 154));
        tableWidget_p->item(i,3)->setBackground(QColor(154, 229, 154));
        tableWidget_p->item(i,4)->setBackground(QColor(154, 229, 154));
      }
      tableWidget_p->item(i, 1)->setText(QString("%1").arg(temp_num));
      
      if(QString(temp_name) == "-")
      {
        QLabel *placement_holder_text_label = new QLabel(this);
        placement_holder_text_label->setText("< Click to edit name >");
        tableWidget_p->setCellWidget (i, 2, placement_holder_text_label);
        placement_holder_text_label->setAlignment(Qt::AlignCenter);
        placement_holder_text_label->setStyleSheet("font: italic; color: grey; background-color: white;");
      }
      else if(QString(temp_name).contains("_"))                                     //TEST~~~ 11_19_2018//
      {                                                                             //TEST~~~
        for(int mn = 0; mn<300; ++mn) if(temp_name[mn] == '_') temp_name[mn] = ' '; //TEST~~~
      }                                                                             //TEST~~~
      
      tableWidget_p->item(i, 2)->setText(temp_name);
      tableWidget_p->item(i, 3)->setText(temp_mask);
        
      if(QString(temp_mask) == "-")
      {
        QLabel *placement_holder_text_label = new QLabel(this);
        placement_holder_text_label->setText("< Click to edit/create macro >");
        tableWidget_p->setCellWidget (i, 3, placement_holder_text_label);
        placement_holder_text_label->setAlignment(Qt::AlignCenter);
        placement_holder_text_label->setStyleSheet("font: italic; color: grey; background-color: white;");
      }
      tableWidget_p->item(i, 4)->setText(temp_func);
      if(macros.eof()) break;
    }
    macros.close();
  }
}

void macro_screen::store_macro_table()
{  
  std::ofstream macros("macro_table",(std::ofstream::out));
  for(int i=0; i<10; ++i)
  {  
    macros<<endl;
    if(tableWidget_p->item(i, 0)->checkState())			 //macro state
    {
      macros<<1<<" ";
    }
    else
    {
      macros<<0<<" ";
    }
    macros<<i+1;																                              //macro number
    
    QString macro_temp_name_str = (tableWidget_p->item(i, 2)->text().toUtf8().constData());
    if(macro_temp_name_str.contains(" "))
    {
      macro_temp_name_str.replace(QString(" "), QString("_"));
      macros<<endl<<macro_temp_name_str.toLatin1().constData();
    }
    else macros<<endl<<(tableWidget_p->item(i, 2)->text()).toUtf8().constData();		//macro name  //TEST~~~//
    
    
    //macros<<endl<<(tableWidget_p->item(i, 2)->text()).toUtf8().constData();		//macro name  //ORIGINAL~~~//
    macros<<endl<<(tableWidget_p->item(i, 3)->text()).toUtf8().constData();		//macro mask
    macros<<endl<<(tableWidget_p->item(i, 4)->text()).toUtf8().constData();		//macro function
  }
  macros.close();
  macros.clear();
  centre_p->tm_macro_updated = false;
}

void macro_screen::cellSelected(int nRow, int nCol)	
{
  cout<<endl<<"Row: "<<nRow<<"	Column: "<<nCol;	//OMIT~~~//
  if(nCol == 0)
  {
    for(int i=0; i<macro_rows; ++i)//clear selection in all rows, then set selected row.  At most 1 row selected.
    {
      tableWidget_p->item(i, 0)->setCheckState ( Qt::Unchecked );

      QLabel *on_off_label_p = new QLabel("OFF");
      tableWidget_p->setCellWidget (i, 0, on_off_label_p);
      on_off_label_p->setAlignment(Qt::AlignCenter);
                                    
      tableWidget_p->item(i,0)->setBackground(QColor(255, 255, 255));
      tableWidget_p->item(i,1)->setBackground(QColor(255, 255, 255));
      tableWidget_p->item(i,2)->setBackground(QColor(255, 255, 255));
      tableWidget_p->item(i,3)->setBackground(QColor(255, 255, 255));
      tableWidget_p->item(i,4)->setBackground(QColor(255, 255, 255));
    }
    if(tableWidget_p->item(nRow, nCol)->checkState())
    {
      tableWidget_p->item(nRow, 0)->setCheckState ( Qt::Unchecked );
      store_macro_table();
      
      QLabel *on_off_label_p = new QLabel("OFF");
      tableWidget_p->setCellWidget (nRow, nCol, on_off_label_p);
      on_off_label_p->setAlignment(Qt::AlignCenter);
                                    
      tableWidget_p->item(nRow,0)->setBackground(QColor(255, 255, 255));
      tableWidget_p->item(nRow,1)->setBackground(QColor(255, 255, 255));
      tableWidget_p->item(nRow,2)->setBackground(QColor(255, 255, 255));
      tableWidget_p->item(nRow,3)->setBackground(QColor(255, 255, 255));
      tableWidget_p->item(nRow,4)->setBackground(QColor(255, 255, 255));
    }
    else
    {
      tableWidget_p->item(nRow, 0)->setCheckState ( Qt::Checked );
      store_macro_table();
      
      QLabel *on_off_label_p = new QLabel("ON");
      tableWidget_p->setCellWidget (nRow, nCol, on_off_label_p);
      on_off_label_p->setAlignment(Qt::AlignCenter);
                                    
      tableWidget_p->item(nRow,0)->setBackground(QColor(154, 229, 154));
      tableWidget_p->item(nRow,1)->setBackground(QColor(154, 229, 154));
      tableWidget_p->item(nRow,2)->setBackground(QColor(154, 229, 154));
      tableWidget_p->item(nRow,3)->setBackground(QColor(154, 229, 154));
      tableWidget_p->item(nRow,4)->setBackground(QColor(154, 229, 154));
    }
  }
  else if(nCol == 2)
  { 
    centre_p -> macro_name_cell(nRow, nCol);
    centre_p -> add_waiting_screen(27);
	  centre_p -> screen_done=true;
  }
  else if(nCol == 3)
  {    
    macro_function_string.clear();
    macro_function_string.append("-");
	
    QDialog dialog(this);
//    dialog.setMaximumSize(800, 360);      //defines size macro builder pop-up window//
//    dialog.setGeometry(0, 0, 800, 360);   //defines starting position and dimensions//
//    dialog.setMaximumSize(800, 480);      //defines size macro builder pop-up window//
    dialog.setGeometry(0, 0, 800, 450);   //defines starting position and dimensions//
    
    QFormLayout form(&dialog);
    dialog.setWindowTitle("USB Communications Macro Builder");
	
    QSignalMapper* signalMapper = new QSignalMapper (this);

    instruction_label_p = new QLabel("If this entry is active and the special USB communication cable is "
                                     "connected to a computer, items listed go to the computer as keyboard input");  
/*    
attached, the items you list will be sent out on the cable after each sample.  \
If the cable is attached to a computer, the items sent will appear on the computer \
as if typed on a keyboard.  For example, \"bar 1, right, count, down, left\" would enter \
barcode 1 and the count in a spreadsheet on the computer.  Touch the buttons to add items to the list.");
*/
    instruction_label_p -> setWordWrap(true);
    lineEdit = new QLineEdit(&dialog);
    lineEdit->setPlaceholderText("Click buttons to add preloaded values or type custom text here.");
    lineEdit->setAlignment(Qt::AlignCenter);
    lineEdit->setFixedWidth(650);
    lineEdit->setFixedHeight(50);
	
    if(!tableWidget_p->item(nRow,3)->text().startsWith("-"))
    {
      lineEdit->setText(tableWidget_p->item(nRow, 3)->text());
      macro_function_string.append(tableWidget_p->item(nRow, 4)->text());
    }
		
    QPushButton *totalize_count_button = new QPushButton(tr("Count"), this);
//    QPushButton *batch_count_button = new QPushButton(tr("Count"), this);
//    QPushButton *seed_button = new QPushButton(tr("Seed name"), this);
    QPushButton *barcode_button1 = new QPushButton(tr("Barcode 1"), this);
    QPushButton *barcode_button2 = new QPushButton(tr("Barcode 2"), this);
    QPushButton *barcode_button3 = new QPushButton(tr("Barcode 3"), this);
    QPushButton *barcode_button4 = new QPushButton(tr("Barcode 4"), this);
//    QPushButton *weight_button = new QPushButton(tr("Weight"), this);		      //yet to be implemented
//    QPushButton *lotcode_button = new QPushButton(tr("Lot Code"), this);
//    QPushButton *packcode_button = new QPushButton(tr("Pack Code"), this);
//    QPushButton *substitution_button = new QPushButton(tr("Substitution"), this);
//    QPushButton *dump_count_button = new QPushButton(tr("Dump Count"), this);
    QPushButton *newline = new QPushButton(tr("Enter ↵"), this);
    QPushButton *horizontal_tab = new QPushButton(tr("Tab ↹"), this);
    QPushButton *spacebar = new QPushButton(tr("Spacebar ␣"), this);
    QPushButton *arrow_up = new QPushButton(tr("Up ↑"), this);
    QPushButton *arrow_down = new QPushButton(tr("Down ↓"), this);
    QPushButton *arrow_left = new QPushButton(tr("Left ←"), this);
    QPushButton *arrow_right = new QPushButton(tr("Right →"), this);
    QPushButton *clear_entry = new QPushButton(tr("Clear All"), this);
    QPushButton *remove_last = new QPushButton(tr("Remove Last"), this);

    totalize_count_button->setFixedSize(macro_button_width,macro_button_height);
//    batch_count_button->setFixedSize(macro_button_width,macro_button_height);
//    seed_button->setFixedSize(macro_button_width,macro_button_height);
    barcode_button1->setFixedSize(macro_button_width,macro_button_height);
    barcode_button2->setFixedSize(macro_button_width,macro_button_height);
    barcode_button3->setFixedSize(macro_button_width,macro_button_height);
    barcode_button4->setFixedSize(macro_button_width,macro_button_height);
//    weight_button->setFixedSize(macro_button_width,macro_button_height);
//    lotcode_button->setFixedSize(macro_button_width,macro_button_height);
//    packcode_button->setFixedSize(macro_button_width,macro_button_height);
//    substitution_button->setFixedSize(macro_button_width,macro_button_height);
//    dump_count_button->setFixedSize(macro_button_width,macro_button_height);
    newline->setFixedSize(macro_button_width,macro_button_height);
    horizontal_tab->setFixedSize(macro_button_width,macro_button_height);
    spacebar->setFixedSize(macro_button_width,macro_button_height);
    arrow_up->setFixedSize(macro_button_width,macro_button_height);
    arrow_down->setFixedSize(macro_button_width,macro_button_height);
    arrow_left->setFixedSize(macro_button_width,macro_button_height);
    arrow_right->setFixedSize(macro_button_width,macro_button_height);
    clear_entry->setFixedSize(2*macro_button_width+6,macro_button_height);
    remove_last->setFixedHeight(macro_button_height+35);  //ORIGINAL~~~
    remove_last->setFixedHeight(macro_button_height);     //TEST~~~
	
    QHBoxLayout *input_line_modify = new QHBoxLayout;
    input_line_modify->addWidget(lineEdit);
    input_line_modify->addWidget(remove_last);
	
    form.addRow(instruction_label_p);
    form.addRow(input_line_modify);
//==================================================================================================================//	
    QGroupBox *totalize_box = new QGroupBox();
    QVBoxLayout *barcodes = new QVBoxLayout;
    QHBoxLayout *barcodes_columns = new QHBoxLayout;
    QVBoxLayout *barcodes_left = new QVBoxLayout;
    QVBoxLayout *barcodes_right = new QVBoxLayout;
    QLabel *totalize_box_label = new QLabel(tr("Data Items"));
	
    barcodes->addWidget(totalize_box_label);
    barcodes_left->addWidget(totalize_count_button);
//    barcodes_left->addWidget(weight_button);		//yet to be implemented
    barcodes_right->addWidget(barcode_button1);
    barcodes_right->addWidget(barcode_button2);
    barcodes_right->addWidget(barcode_button3);
    barcodes_right->addWidget(barcode_button4);

    barcodes_columns->addLayout(barcodes_left);
    barcodes_columns->addLayout(barcodes_right);
    barcodes->addLayout(barcodes_columns);
    totalize_box->setLayout(barcodes);
    totalize_box->show();
    
//    weight_button->setDisabled(true);
    barcode_button1->setDisabled(true);
    barcode_button2->setDisabled(true);
    barcode_button3->setDisabled(true);
    barcode_button4->setDisabled(true);

    if(centre_p->tm_barcode_columns > 0) barcode_button1->setEnabled(true);
    if(centre_p->tm_barcode_columns > 1) barcode_button2->setEnabled(true);
    if(centre_p->tm_barcode_columns > 2) barcode_button3->setEnabled(true);
    if(centre_p->tm_barcode_columns > 3) barcode_button4->setEnabled(true);
    
    barcodes_left->setAlignment(Qt::AlignTop);
    barcodes->setAlignment(Qt::AlignTop);
    totalize_box_label->setStyleSheet("QLabel { font: bold;}");
    totalize_box->setStyleSheet("QGroupBox {  border: 2px solid gray;"
											 "border-radius: 4px;"
											 "background-color: transparent;}");
//==================================================================================================================//	
//    QGroupBox *batchmode_button_box = new QGroupBox();
//    QVBoxLayout *batchcodes = new QVBoxLayout;
//    QLabel *batchmode_box_label = new QLabel(tr("Batch Mode"));
        
//    batchcodes->addWidget(batchmode_box_label);
//    batchcodes->addWidget(batch_count_button);
//    batchcodes->addWidget(lotcode_button);
//    batchcodes->addWidget(packcode_button);
//    batchcodes->addWidget(substitution_button);	//yet to be implemented
//    batchcodes->addWidget(dump_count_button);
    
//    batchmode_button_box->setLayout(batchcodes);
//    batchmode_button_box->show();
    
//    substitution_button->setDisabled(true);
    
//    batchcodes->setAlignment(Qt::AlignTop);
//    batchmode_box_label->setStyleSheet("QLabel { font: bold;}");
//    batchmode_button_box->setStyleSheet("QGroupBox {  border: 2px solid gray;"
//                                        "border-radius: 4px;"
//                                        "background-color: transparent;}");
//==================================================================================================================//
    QGroupBox *accessibility_button_box = new QGroupBox();
    QVBoxLayout *accessibility_keys_container = new QVBoxLayout;
    QHBoxLayout *accessibility_keys = new QHBoxLayout;
    QVBoxLayout *accessibility_keys_left = new QVBoxLayout;
    QVBoxLayout *accessibility_keys_right = new QVBoxLayout;
    QLabel *accessibility_box_label = new QLabel(tr("Navigation and Formatting"));
    
//    accessibility_keys_left->addWidget(seed_button);
    accessibility_keys_left->addWidget(newline);
    accessibility_keys_left->addWidget(horizontal_tab);
    accessibility_keys_left->addWidget(spacebar);
    accessibility_keys_right->addWidget(arrow_up);
    accessibility_keys_right->addWidget(arrow_down);
    accessibility_keys_right->addWidget(arrow_left);
    accessibility_keys_right->addWidget(arrow_right);

    accessibility_keys_container->addWidget(accessibility_box_label);
    accessibility_keys->addLayout(accessibility_keys_left);
    accessibility_keys->addLayout(accessibility_keys_right);
    accessibility_keys_container->addWidget(clear_entry);
    accessibility_keys_container->setAlignment(Qt::AlignHCenter);
    accessibility_keys_container->addLayout(accessibility_keys);
    accessibility_button_box->setLayout(accessibility_keys_container);
	
//    seed_button->setDisabled(true);
    
    accessibility_keys_container->setAlignment(Qt::AlignTop);
    accessibility_box_label->setStyleSheet("QLabel { font: bold;}");
    accessibility_button_box->setStyleSheet("QGroupBox {  border: 2px solid gray;"
											 "border-radius: 4px;"
											 "background-color: transparent;}");
//==================================================================================================================//
    QHBoxLayout *macro_menu_main_horizontal = new QHBoxLayout;
    macro_menu_main_horizontal->addWidget(totalize_box);
//    macro_menu_main_horizontal->addWidget(batchmode_button_box);
    macro_menu_main_horizontal->addWidget(accessibility_button_box);
    form.addRow(macro_menu_main_horizontal);
//==================================================================================================================//
    connect(totalize_count_button, SIGNAL(clicked()), signalMapper, SLOT(map()));	//totalize mode variables
//    connect(seed_button, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(barcode_button1, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(barcode_button2, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(barcode_button3, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(barcode_button4, SIGNAL(clicked()), signalMapper, SLOT(map()));
    
//    connect(batch_count_button, SIGNAL(clicked()), signalMapper, SLOT(map()));		//batch mode variables
//    connect(lotcode_button, SIGNAL(clicked()), signalMapper, SLOT(map()));
//    connect(packcode_button, SIGNAL(clicked()), signalMapper, SLOT(map()));
    //connect(subsitution_button, SIGNAL(clicked()), signalMapper, SLOT(map()));  //not yet implemented~~~~
//    connect(dump_count_button, SIGNAL(clicked()), signalMapper, SLOT(map()));
    
    connect(newline, SIGNAL(clicked()), signalMapper, SLOT(map()));					      //accessibility keys
    connect(horizontal_tab, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(spacebar, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(arrow_up, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(arrow_down, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(arrow_left, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(arrow_right, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(clear_entry, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(remove_last, SIGNAL(clicked()), signalMapper, SLOT(map()));
    
    signalMapper -> setMapping (totalize_count_button, 1);		//totalize mode variables
//    signalMapper -> setMapping (seed_button, 2);
    signalMapper -> setMapping (barcode_button1, 3);
    signalMapper -> setMapping (barcode_button2, 4);
    signalMapper -> setMapping (barcode_button3, 5);
    signalMapper -> setMapping (barcode_button4, 6);
    
//    signalMapper -> setMapping (batch_count_button, 1);			  //batch mode variables
//    signalMapper -> setMapping (lotcode_button, 15);
//    signalMapper -> setMapping (packcode_button, 16);	
//    signalMapper -> setMapping (dump_count_button, 18);	  
    
    signalMapper -> setMapping (newline, 7);					        //accessibility keys
    signalMapper -> setMapping (horizontal_tab, 8);
    signalMapper -> setMapping (spacebar, 9);
    signalMapper -> setMapping (arrow_up, 10);
    signalMapper -> setMapping (arrow_down, 11);
    signalMapper -> setMapping (arrow_left, 12);
    signalMapper -> setMapping (arrow_right, 13);
    signalMapper -> setMapping (clear_entry, 14);
    signalMapper -> setMapping (remove_last, 17);
	
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(dialogbox_buttons(int)));

    QDialogButtonBox buttonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel,	Qt::Horizontal, &dialog);  //standard buttons
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
    form.addRow(&buttonBox);
    
    //buttonBox.button(QDialogButtonBox::Save)->setStyleSheet("QPushButton { font: bold;}");
    //buttonBox.button(QDialogButtonBox::Cancel)->setStyleSheet("QPushButton { font: bold;}");
    buttonBox.button(QDialogButtonBox::Save)->setFixedSize(macro_button_width-20, macro_button_height);
    buttonBox.button(QDialogButtonBox::Cancel)->setFixedSize(macro_button_width-20, macro_button_height);
    
    //=============================================================//
    //Style sheet specific for macro builder screen//
      setStyleSheet(
        "QPushButton {"
            "border: 2px solid #8f8f91;"
            "height: 25px;"
            "font: 16px;"
            "border-radius: 8px;"
            "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
            "stop: 0 #f6f7fa, stop: 1 #dadbde);}"

        "QLineEdit {"
            "font-size: 14pt;}"
        
        "QTabWidget {"                    //macro-screen//
            "font-size: 13pt;}"

        "QTableView {"
            "font-size: 13pt;"
            "border: 2px solid #8f8f91;}"
            
        "QLabel {"
            "font-size: 13pt;}"
          
    //==================================================================//            
        "QScrollBar {"
            "width: 60px;"
            "border: 1px solid #404040;"
            "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
            "stop: 0 #f6f7fa, stop: 1 #dadbde);}"
        
        "QScrollBar::handle {"
            "border: 3px solid #595959;"
              "min-height: 40px;"
            "border-radius: 12px;"
            "background: white;}"
      
        "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {"
            "width: 0;"
            "height: 0;"
            "border: none;"
            "background: none;"
            "color: none;}"
        
        "QScrollBar::add-line:vertical {"
            "width: 0;"
            "height: 0;"      
            "border: none;"
            "background: none;}"

        "QScrollBar::sub-line:vertical {"
            "width: 0;"
            "height: 0;"
            "border: none;"
            "background: none;}"
      );
    
    //=============================================================//
    
    if (dialog.exec() == QDialog::Accepted)
    {
      if(lineEdit->text().isEmpty())
      {
        lineEdit->setText("-");
        macro_function_string.clear();
        macro_function_string.append("-");
        
        QLabel *placement_holder_text_label = new QLabel(this);                                            //TEST~~~
        placement_holder_text_label->setText("< Click to edit/create macro >");                            //TEST~~~
        tableWidget_p->setCellWidget (nRow, nCol, placement_holder_text_label);                            //TEST~~~
        placement_holder_text_label->setAlignment(Qt::AlignCenter);                                        //TEST~~~
        placement_holder_text_label->setStyleSheet("font: italic; color: grey; background-color: white;"); //TEST~~~       
      }
      else
      {
        QLabel *placement_holder_text_label = new QLabel(this);                   //TEST~~~
        placement_holder_text_label->setText("");                                 //TEST~~~
        tableWidget_p->setCellWidget (nRow, nCol, placement_holder_text_label);   //TEST~~~ 
      }
      if(macro_function_string.startsWith("-") && lineEdit->text().size() > 1)
      {
        macro_function_string.remove(0,1);
      }
      tableWidget_p->item(nRow, 3)->setText(lineEdit->text());
      tableWidget_p->item(nRow, 4)->setText(macro_function_string);
      store_macro_table();
    }
  }
}

void macro_screen::dialogbox_buttons(int n)
{
  QString macro_creation_string = lineEdit->text();

  if(macro_creation_string.size() > 1)
  {
    macro_creation_string.append(",");
  }
  
  switch(n){	  //Generate macro key display sequence and output sequence
	  case 1:
	  {
      lineEdit->setText(macro_creation_string + "count");
      macro_function_string.append("\\C");
      break;
	  }
	  case 2:
	  {
      lineEdit->setText(macro_creation_string + "seed");
      macro_function_string.append("\\S");
      break;
	  }
	  case 3:
	  {
      lineEdit->setText(macro_creation_string + "bar1");
      macro_function_string.append("\\1");
      break;
	  }
	  case 4:
	  {
      lineEdit->setText(macro_creation_string + "bar2");
      macro_function_string.append("\\2");
      break;
	  }
	  case 5:
	  {
      lineEdit->setText(macro_creation_string + "bar3");
      macro_function_string.append("\\3");
      break;
	  }
	  case 6:
	  {
      lineEdit->setText(macro_creation_string + "bar4");
      macro_function_string.append("\\4");
      break;
	  }
	  case 7:
	  {
      lineEdit->setText(macro_creation_string + "newline");
      macro_function_string.append("\\n");
      break;
	  }
	  case 8:
	  {
      lineEdit->setText(macro_creation_string + "tab");
      macro_function_string.append("\\t");
      break;
	  }
	  case 9:
	  {
      lineEdit->setText(macro_creation_string + "spacebar");
      macro_function_string.append("\\s");
      break;
	  }
	  case 10:
	  {
      lineEdit->setText(macro_creation_string + "up");
      macro_function_string.append("\\U");
      break;
	  }
	  case 11:
	  {
      lineEdit->setText(macro_creation_string + "down");
      macro_function_string.append("\\D");
      break;
	  }
	  case 12:
	  {
      lineEdit->setText(macro_creation_string + "left");
      macro_function_string.append("\\L");
      break;
	  }
	  case 13:
	  {
      lineEdit->setText(macro_creation_string + "right");
      macro_function_string.append("\\R");
      break;
	  }
	  case 14:
	  {
      lineEdit->setText("");
      macro_function_string.clear();
      macro_function_string.append("-");
      break;
	  }
	  case 15:
	  {
      lineEdit->setText(macro_creation_string + "lotcode");
      macro_function_string.append("\\T");
      break;
	  }
  	case 16:
  	{
      lineEdit->setText(macro_creation_string + "packcode");
      macro_function_string.append("\\P");
      break;
	  }
  	case 17:
  	{
      int last_comma_position;
      int size_of_macro_string = 0;
      QString new_test_string;
		  
      last_comma_position = lineEdit->text().lastIndexOf(",");
		  
      if(last_comma_position > -1)
      {
        size_of_macro_string = lineEdit->text().size();
        new_test_string = lineEdit->text().remove(last_comma_position, (size_of_macro_string - last_comma_position));
		  
        last_comma_position = macro_function_string.lastIndexOf("\\");
        size_of_macro_string = macro_function_string.size();
		  
        macro_function_string.remove(last_comma_position, (size_of_macro_string - last_comma_position));
        lineEdit->setText(new_test_string);
	    }
	    else if(last_comma_position == -1)
	    {
        macro_function_string.remove(0, size_of_macro_string);
        lineEdit->setText("");  
      }
      break;
	  }
    case 18:
    {
      lineEdit->setText(macro_creation_string + "dump");
		  macro_function_string.append("\\Q");
      
      break;
    }
    
	  default:
	  {
      //cout<<"default"<<endl;	//omit~~~
	  }
  }
}
