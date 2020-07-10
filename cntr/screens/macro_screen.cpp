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
  
  
  cout<<"start macro_screen::macro_screen.  macro_display_string = "<<centre_p->macro_display_string.toStdString()<<"    macro_function_string = "<<centre_p->macro_function_string.toStdString()<<endl;
  
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
//  connect(this, SIGNAL(return_with_keyboard_entry(int, int)), this, SLOT(cellSelected(int, int)));
  
  screen_title_label_p->setText("Communications Menu");
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
    tableWidget_p->item(row, 3)->setText(centre_p->macro_display_string);
    tableWidget_p->item(row, 4)->setText(centre_p->macro_function_string);
    if(tableWidget_p->item(row, 0)->checkState() == Qt::Unchecked)
    {
      toggle_macro(row);
    }
//    store_macro_table();
  }
    
  
  if(centre_p->new_keyboard_entry == true) //returning after entering a macro name using a keyboard screen
  {        
    cout<<"centre_p->new_keyboard_entry == true\n";
    cout<<"centre_p->keyboard_return_string = "<<centre_p->keyboard_return_string.toStdString()<<endl;
    cout<<"centre_p->macro_row = "<<centre_p->macro_row<<endl;
    
    centre_p->new_keyboard_entry = false;
    
//    QWidget* cell_widget_p = tableWidget_p->cellWidget(centre_p->macro_row, 2
    tableWidget_p->item(centre_p->macro_row, 2)->setText(centre_p->keyboard_return_string);
    
//    QTableWidgetItem *item = new QTableWidgetItem;
//    item->setText(centre_p->keyboard_return_string);
//    item->setTextAlignment (Qt::AlignCenter);
//    tableWidget_p->setItem(centre_p->macro_row, 2, item);

//    QLabel *placement_holder_text_label = new QLabel(this);
//    placement_holder_text_label->setText(centre_p->keyboard_return_string);
//    tableWidget_p->setCellWidget (centre_p->macro_row, 2, placement_holder_text_label);

    
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
//  cout<<"ok clicked\n";
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
      
//      if(QString(temp_name) == "-")
//      {
//        QLabel *placement_holder_text_label = new QLabel(this);
//        placement_holder_text_label->setText("< Click to edit name >");
//        tableWidget_p->setCellWidget (i, 2, placement_holder_text_label);
//        placement_holder_text_label->setAlignment(Qt::AlignCenter);
//        placement_holder_text_label->setStyleSheet("font: italic; color: grey; background-color: white;");
//      }
//      else if(QString(temp_name).contains("_"))                                     //TEST~~~ 11_19_2018//
      if(QString(temp_name).contains("_"))                                     //TEST~~~ 11_19_2018//
      {                                                                             //TEST~~~
        for(int mn = 0; mn<300; ++mn) if(temp_name[mn] == '_') temp_name[mn] = ' '; //TEST~~~
      }                                                                             //TEST~~~
      
      tableWidget_p->item(i, 2)->setText(temp_name);
      tableWidget_p->item(i, 3)->setText(temp_mask);
        
//      if(QString(temp_mask) == "-")
//      {
//        QLabel *placement_holder_text_label = new QLabel(this);
//        placement_holder_text_label->setText("< Click to edit/create macro >");
//        tableWidget_p->setCellWidget (i, 3, placement_holder_text_label);
//        placement_holder_text_label->setAlignment(Qt::AlignCenter);
//        placement_holder_text_label->setStyleSheet("font: italic; color: grey; background-color: white;");
//      }
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
      
//      QLabel *placement_holder_text_label = new QLabel(this);
//      placement_holder_text_label->setText("< Click to edit name >");
//      tableWidget_p->setCellWidget (i, 2, placement_holder_text_label);
//      placement_holder_text_label->setAlignment(Qt::AlignCenter);
//      placement_holder_text_label->setStyleSheet("font: italic; color: grey; background-color: white;");
      
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
    /*
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
    */ 
  }
  else if(nCol == 2)
  { 
//    centre_p -> macro_name_cell(nRow, nCol);
//    centre_p -> add_waiting_screen(27);

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
    /*
    macro_function_string.clear();
    macro_function_string.append("-");
	
    QDialog dialog(this);
    dialog.setGeometry(0, 0, 800, 450);   //defines starting position and dimensions//
    
    connect(this, SIGNAL(close_dialog(int)), &dialog, SLOT(done(int)));
    
    QFormLayout form(&dialog);
    dialog.setWindowTitle("USB Communications Macro Builder");
	
    QSignalMapper* signalMapper = new QSignalMapper (this);

    instruction_label_p = new QLabel("If this entry is active and the special USB communication cable is "
                                     "connected to a computer, items listed go to the computer as keyboard input");  
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
    QPushButton *text_entry_button = new QPushButton(tr("Enter text"), this);
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
    text_entry_button->setFixedSize(macro_button_width,macro_button_height);
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
    barcodes_left->addWidget(text_entry_button);
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
    connect(text_entry_button, SIGNAL(clicked()), signalMapper, SLOT(map()));
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
    signalMapper -> setMapping (text_entry_button, 20);		//totalize mode variables
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
    
    if(centre_p->new_keyboard_entry)
    {          
      QString string = "\"";
      string += centre_p->keyboard_return_string;
      string += "\"";
      cout<<"\nadding string to lineEdit.  string = "<<string.toStdString()<<" end\n";
      lineEdit->setText(lineEdit->text() + "," + string);
      macro_function_string.append("\\");
      macro_function_string.append(string);
    }

    
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
    case 20://exit this screen for keyboard entry
    {
      cout<<"case 20\n";
      
      centre_p->new_keyboard_entry = true;//This will tell this screen's constructor that it is returning from keyboard entry
      centre_p->keyboard_message_string = "Enter characters to add to the macro.";
      centre_p->control_int[0] = current_row; //remember for return to this screen
      centre_p->control_int[1] = current_column; //remember for return to this screen
      centre_p->add_waiting_screen(28);//come back here to macro_screen
      centre_p->add_waiting_screen(100);//keyboard
      centre_p->screen_done = true;
      emit close_dialog(0);
      
      cout<<"end of case 20.  new_keyboard_entry = "<<centre_p->new_keyboard_entry<<endl;
      
      break;
    }  
	  default:
	  {
      //cout<<"default"<<endl;	//omit~~~
	  }
  }
}
*/
