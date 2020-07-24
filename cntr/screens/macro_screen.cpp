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
  centre_p=set_centre_p;
  
  screen_title_label_p = new QLabel;
  description_p = new QLabel;

  back_button_p = new button("Back");
  ok_button_p = new button("OK");
  help_button_p = new button("Help");
  disable_all_button_p = new button("Disable All");
  enable_all_button_p = new button("Enable All");
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
  main_layout_p->addWidget(disable_all_button_p, 5, 0);
  main_layout_p->addWidget(tableWidget_p,3,0,2,4);
  setLayout(main_layout_p);
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(ok_button_p, SIGNAL(clicked()), this, SLOT(ok_button_clicked()));
  connect(help_button_p, SIGNAL(clicked()), this, SLOT(help_button_clicked()));
  connect(disable_all_button_p, SIGNAL(clicked()), this, SLOT(disable_all_clicked()));
  connect(enable_all_button_p, SIGNAL(clicked()), this, SLOT(enable_all_clicked()));
  connect(tableWidget_p, SIGNAL(cellClicked(int, int)), this, SLOT(cellSelected(int, int)));
  
  screen_title_label_p->setText("Totalize Mode Communication Macros");
  screen_title_label_p->setStyleSheet("QLabel { font: bold }");
  if(centre_p->communicate_by_keyboard_cable)
  {
    description_p->setText("If the special USB Communications cable is connected to a computer, \
items listed in the selected macro will be sent to the computer, which will see \
them as keyboard input.");
  }
  if(centre_p->tcp_link_established)
  {
    description_p->setText("A TCP connection has been established to another device. \
Items listed in the selected macro will be sent to that device.");
  }
  description_p->setWordWrap(true);
  initialize_macro_menu();

  load_macro_table();  
  tableWidget_p->setStyleSheet("QTableWidget { font: 15pt;}");
  tableWidget_p->verticalHeader()->setDefaultSectionSize(45);
  
  help_screen_p = 0;
  current_row = 0;
  current_column = 0;
  
  if(centre_p->build_macro) //returning from macro_builder screen with a new macro
  {
    centre_p->build_macro = false;
    int row = centre_p->macro_row;
    centre_p->macro_row = 0;
    if(centre_p->macro_display_string == "")
    {
      tableWidget_p->item(row, 3)->setText("-");
      tableWidget_p->item(row, 4)->setText("-");
    }
    else
    {
      tableWidget_p->item(row, 3)->setText(centre_p->macro_display_string);
      tableWidget_p->item(row, 4)->setText(centre_p->macro_function_string);
    }
    if(tableWidget_p->item(row, 0)->checkState() == Qt::Unchecked)
    {
      toggle_macro(row);
    }
  }
    
  
  if(centre_p->new_keyboard_entry == true) //returning after entering a macro name using a keyboard screen
  {        
    cout<<"centre_p->new_keyboard_entry == true\n";
    cout<<"centre_p->keyboard_return_string = "<<centre_p->keyboard_return_string.toStdString()<<endl;
    cout<<"centre_p->macro_row = "<<centre_p->macro_row<<endl;
    
    centre_p->new_keyboard_entry = false;
    
    if(centre_p->keyboard_return_string=="")
    {
      tableWidget_p->item(centre_p->macro_row, 2)->setText("-");
    }
    else 
    {
      tableWidget_p->item(centre_p->macro_row, 2)->setText(centre_p->keyboard_return_string);
    }

    cout<<"tableWidget_p->item(centre_p->macro_row, 2)->text().toStdString()  "<<tableWidget_p->item(centre_p->macro_row, 2)->text().toStdString()<<endl;
    
    centre_p->keyboard_message_string = "";
    centre_p->keyboard_return_string = "";
  }
}

macro_screen::~macro_screen()
{
  store_macro_table();
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

void macro_screen::help_button_clicked()
{ 
  help_screen_p = new help_screen();
  			   
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

void macro_screen::initialize_macro_menu()
{
  
//  cout<<"macro_screen::initialize_macro_menu\n";
  
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
  
//  cout<<"macro_screen::load_macro_table()\n";
  
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
      
      if(QString(temp_name).contains("_"))                                     //TEST~~~ 11_19_2018//
      {                                                                             //TEST~~~
        for(int mn = 0; mn<300; ++mn) if(temp_name[mn] == '_') temp_name[mn] = ' '; //TEST~~~
      }                                                                             //TEST~~~
      
      tableWidget_p->item(i, 2)->setText(temp_name);
      tableWidget_p->item(i, 3)->setText(temp_mask);
        
      tableWidget_p->item(i, 4)->setText(temp_func);
      if(macros.eof()) break;
    }
    macros.close();
  }
  else//macro_table file did not open.  probably does not exist
  {
    for(int i=0; i<10; ++i)
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

      tableWidget_p->item(i, 1)->setText(QString("%1").arg(i));
      
      tableWidget_p->item(i, 2)->setText("-");
      tableWidget_p->item(i, 3)->setText("-");
        
      QLabel* placement_holder_text_label = new QLabel(this);
      placement_holder_text_label->setText("< Click to edit/create macro >");
      tableWidget_p->setCellWidget (i, 3, placement_holder_text_label);
      placement_holder_text_label->setAlignment(Qt::AlignCenter);
      placement_holder_text_label->setStyleSheet("font: italic; color: grey; background-color: white;");
      tableWidget_p->item(i, 4)->setText("-");
    }
  }
}

void macro_screen::store_macro_table()
{  
  
//  cout<<"macro_screen::store_macro_table()\n";
  
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
    macros<<endl<<(tableWidget_p->item(i, 3)->text()).toUtf8().constData();		//macro mask
    macros<<endl<<(tableWidget_p->item(i, 4)->text()).toUtf8().constData();		//macro function
  }
  macros.close();
  macros.clear();
  centre_p->tm_macro_updated = false;
}

void macro_screen::cellSelected(int nRow, int nCol)	
{
  
  cout<<"macro_screen::cellSelected(int nRow, int nCol)	\n";
  
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
    centre_p -> macro_row = nRow;
	  centre_p -> screen_done=true;
  }
  else if(nCol == 3)
  {    
    centre_p->macro_row = nRow;
    centre_p->build_macro = true;
    centre_p->macro_display_string = tableWidget_p->item(nRow, 3)->text();
    centre_p->macro_function_string = tableWidget_p->item(nRow, 4)->text();
    centre_p->add_waiting_screen(38);//macro_builder
    centre_p->screen_done = true;
  }
}

void macro_screen::toggle_macro(int macro_number)
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
  if(tableWidget_p->item(macro_number, 0)->checkState())
  {
    tableWidget_p->item(macro_number, 0)->setCheckState ( Qt::Unchecked );
    store_macro_table();
    
    QLabel *on_off_label_p = new QLabel("OFF");
    tableWidget_p->setCellWidget (macro_number, 0, on_off_label_p);
    on_off_label_p->setAlignment(Qt::AlignCenter);
                                  
    tableWidget_p->item(macro_number,0)->setBackground(QColor(255, 255, 255));
    tableWidget_p->item(macro_number,1)->setBackground(QColor(255, 255, 255));
    tableWidget_p->item(macro_number,2)->setBackground(QColor(255, 255, 255));
    tableWidget_p->item(macro_number,3)->setBackground(QColor(255, 255, 255));
    tableWidget_p->item(macro_number,4)->setBackground(QColor(255, 255, 255));
  }
  else
  {
    tableWidget_p->item(macro_number, 0)->setCheckState ( Qt::Checked );
    store_macro_table();
    
    QLabel *on_off_label_p = new QLabel("ON");
    tableWidget_p->setCellWidget (macro_number, 0, on_off_label_p);
    on_off_label_p->setAlignment(Qt::AlignCenter);
                                  
    tableWidget_p->item(macro_number,0)->setBackground(QColor(154, 229, 154));
    tableWidget_p->item(macro_number,1)->setBackground(QColor(154, 229, 154));
    tableWidget_p->item(macro_number,2)->setBackground(QColor(154, 229, 154));
    tableWidget_p->item(macro_number,3)->setBackground(QColor(154, 229, 154));
    tableWidget_p->item(macro_number,4)->setBackground(QColor(154, 229, 154));
  }
}    
