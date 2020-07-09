#include <iostream>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
//#include <QRadioButton>
//#include <QSpinBox>
#include "centre.hpp"
#include "macro_screen.hpp"
#include "button.hpp"
#include "help_screen.hpp"
#include "macro_builder.hpp"

//#include <QHeaderView>
//#include <fstream>
//#include <QFormLayout>
//#include <QDialogButtonBox>
//#include <QDialog>
#include <QSignalMapper>
#include <QTabWidget>
#include <QFormLayout>

using namespace std;

macro_builder::macro_builder(centre* centre_p_s)
:screen(centre_p_s)
{    
  
  cout<<"macro_builder::macro_builder 1\n";
  
  macro_function_string.clear();
  macro_function_string.append("-");

//  QDialog dialog(this);
//  dialog.setGeometry(0, 0, 800, 450);   //defines starting position and dimensions//
  
//  connect(this, SIGNAL(close_dialog(int)), &dialog, SLOT(done(int)));
  
  form_p = new QFormLayout(this);
//  dialog.setWindowTitle("USB Communications Macro Builder");

  QSignalMapper* signalMapper = new QSignalMapper (this);

//  instruction_label_p = new QLabel("If this entry is active and the special USB communication cable is "
//                                   "connected to a computer, items listed go to the computer as keyboard input");  
  instruction_label_p = new QLabel("If this entry is active, items listed will be sent out using the selected communications method.");  
  instruction_label_p -> setWordWrap(true);
  lineEdit = new QLineEdit;//(&dialog);
  lineEdit->setPlaceholderText("Click buttons to add preloaded values or type custom text here.");
  lineEdit->setAlignment(Qt::AlignCenter);
  lineEdit->setFixedWidth(650);
  lineEdit->setFixedHeight(50);
  /*
  if(!tableWidget_p->item(nRow,3)->text().startsWith("-"))
  {
    lineEdit->setText(tableWidget_p->item(nRow, 3)->text());
    macro_function_string.append(tableWidget_p->item(nRow, 4)->text());
  }
  */
  if((centre_p->macro_display_string).startsWith("-") == false)
  {
    lineEdit->setText(centre_p->macro_display_string);
    macro_function_string.append(centre_p->macro_function_string);
  }
  
  totalize_count_button = new QPushButton(tr("Count"), this);
  text_entry_button = new QPushButton(tr("Enter text"), this);
//    batch_count_button = new QPushButton(tr("Count"), this);
//    seed_button = new QPushButton(tr("Seed name"), this);
  barcode_button1 = new QPushButton(tr("Barcode 1"), this);
  barcode_button2 = new QPushButton(tr("Barcode 2"), this);
  barcode_button3 = new QPushButton(tr("Barcode 3"), this);
  barcode_button4 = new QPushButton(tr("Barcode 4"), this);
//    weight_button = new QPushButton(tr("Weight"), this);		      //yet to be implemented
//    lotcode_button = new QPushButton(tr("Lot Code"), this);
//    packcode_button = new QPushButton(tr("Pack Code"), this);
//    substitution_button = new QPushButton(tr("Substitution"), this);
//    dump_count_button = new QPushButton(tr("Dump Count"), this);
  newline = new QPushButton(tr("Enter ↵"), this);
  horizontal_tab = new QPushButton(tr("Tab ↹"), this);
  spacebar = new QPushButton(tr("Spacebar ␣"), this);
  arrow_up = new QPushButton(tr("Up ↑"), this);
  arrow_down = new QPushButton(tr("Down ↓"), this);
  arrow_left = new QPushButton(tr("Left ←"), this);
  arrow_right = new QPushButton(tr("Right →"), this);
  clear_entry = new QPushButton(tr("Clear All"), this);
  remove_last = new QPushButton(tr("Remove Last"), this);
  
  back_button_p = new button("Back");
  save_button_p = new button("Save");
  
  cout<<"macro_builder::macro_builder 2\n";
  

  totalize_count_button->setFixedSize(macro_button_width,macro_button_height);
  text_entry_button->setFixedSize(macro_button_width,macro_button_height);
//    batch_count_button->setFixedSize(macro_button_width,macro_button_height);
//    seed_button->setFixedSize(macro_button_width,macro_button_height);
  barcode_button1->setFixedSize(macro_button_width,macro_button_height);
  barcode_button2->setFixedSize(macro_button_width,macro_button_height);
  barcode_button3->setFixedSize(macro_button_width,macro_button_height);
  barcode_button4->setFixedSize(macro_button_width,macro_button_height);
  
  cout<<"macro_builder::macro_builder 2a\n";
  
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
  
  cout<<"macro_builder::macro_builder 2b\n";
  
  top_line_p = new QHBoxLayout(this);
  top_line_p->addWidget(instruction_label_p);
  top_line_p->addWidget(back_button_p);

  input_line_modify = new QHBoxLayout(this);
  input_line_modify->addWidget(lineEdit);
  input_line_modify->addWidget(remove_last);

//  form_p->addRow(instruction_label_p);
  form_p->addRow(top_line_p);
  form_p->addRow(input_line_modify);
//==================================================================================================================//	
  totalize_box = new QGroupBox();
  barcodes = new QVBoxLayout;
  barcodes_columns = new QHBoxLayout;
  barcodes_left = new QVBoxLayout;
  barcodes_right = new QVBoxLayout;
  totalize_box_label = new QLabel(tr("Data Items"));
  
  cout<<"macro_builder::macro_builder 2c\n";
  

  barcodes->addWidget(totalize_box_label);
  barcodes_left->addWidget(totalize_count_button);
  barcodes_left->addWidget(text_entry_button);
//    barcodes_left->addWidget(weight_button);		//yet to be implemented
  barcodes_right->addWidget(barcode_button1);
  barcodes_right->addWidget(barcode_button2);
  barcodes_right->addWidget(barcode_button3);
  barcodes_right->addWidget(barcode_button4);
  
  cout<<"macro_builder::macro_builder 2d\n";
  

  barcodes_columns->addLayout(barcodes_left);
  barcodes_columns->addLayout(barcodes_right);
  barcodes->addLayout(barcodes_columns);
  totalize_box->setLayout(barcodes);
//  totalize_box->show();
  
//    weight_button->setDisabled(true);
  barcode_button1->setDisabled(true);
  barcode_button2->setDisabled(true);
  barcode_button3->setDisabled(true);
  barcode_button4->setDisabled(true);

  
  cout<<"macro_builder::macro_builder 2e\n";
  
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
  
  
  cout<<"macro_builder::macro_builder 2f\n";
  
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

  
  cout<<"macro_builder::macro_builder 2g\n";
  
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
  form_p->addRow(macro_menu_main_horizontal);
  
  cout<<"macro_builder::macro_builder 2h\n";
  
  bottom_line_p = new QHBoxLayout(this);
  bottom_line_p->addWidget(save_button_p);
  form_p->addRow(bottom_line_p);
//==================================================================================================================//
  connect(totalize_count_button, SIGNAL(clicked()), signalMapper, SLOT(map()));	//totalize mode variables
  connect(text_entry_button, SIGNAL(clicked()), signalMapper, SLOT(map()));
//    connect(seed_button, SIGNAL(clicked()), signalMapper, SLOT(map()));
  connect(barcode_button1, SIGNAL(clicked()), signalMapper, SLOT(map()));
  connect(barcode_button2, SIGNAL(clicked()), signalMapper, SLOT(map()));
  connect(barcode_button3, SIGNAL(clicked()), signalMapper, SLOT(map()));
  connect(barcode_button4, SIGNAL(clicked()), signalMapper, SLOT(map()));
  
  
  cout<<"macro_builder::macro_builder 2i\n";
  
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
  
  
  cout<<"macro_builder::macro_builder 2j\n";
  
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
  
  cout<<"macro_builder::macro_builder 3\n";
  

  connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(dialogbox_buttons(int)));
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(save_button_p, SIGNAL(clicked()), this, SLOT(save_button_clicked()));

//  QDialogButtonBox buttonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel,	Qt::Horizontal, &dialog);  //standard buttons
//  QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
//  QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
//  form_p->addRow(&buttonBox);
  
  //buttonBox.button(QDialogButtonBox::Save)->setStyleSheet("QPushButton { font: bold;}");
  //buttonBox.button(QDialogButtonBox::Cancel)->setStyleSheet("QPushButton { font: bold;}");
//  buttonBox.button(QDialogButtonBox::Save)->setFixedSize(macro_button_width-20, macro_button_height);
//  buttonBox.button(QDialogButtonBox::Cancel)->setFixedSize(macro_button_width-20, macro_button_height);
  
  if(centre_p->new_keyboard_entry)//signals that a string was entered on the keyboard to add to the macro
  {          
    QString string = "\"";
    string += centre_p->keyboard_return_string;
    string += "\"";
    cout<<"\nadding string to lineEdit.  string = "<<string.toStdString()<<" end\n";
    
//    lineEdit->setText(centre_p->macro_display_string);
    if(centre_p->macro_display_string.size() > 1)
    {
      centre_p->macro_display_string.append(",");
    }
    lineEdit->setText(centre_p->macro_display_string + string);
    macro_function_string = centre_p->macro_function_string;
    macro_function_string.append("\\");
    macro_function_string.append(string);
    
    centre_p->new_keyboard_entry = false;
    centre_p->keyboard_message_string = "";
    centre_p->keyboard_return_string = "";
    
  }

  
  //=============================================================//
  //Style sheet specific for macro builder screen//
  setStyleSheet
  (
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
  /*
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
  */
  
  cout<<"macro_builder::macro_builder 4\n";
  
}

void macro_builder::dialogbox_buttons(int n)
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
      
      cout<<"clearing\n";
      
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
  	case 17: //remove_last
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
      centre_p->macro_display_string = lineEdit->text();
      centre_p->macro_function_string = macro_function_string;
//      centre_p->control_int[0] = current_row; //remember for return to this screen
//      centre_p->control_int[1] = current_column; //remember for return to this screen
      centre_p->add_waiting_screen(38);//come back here to macro_builder
      centre_p->add_waiting_screen(100);//keyboard
      centre_p->screen_done = true;
//      emit close_dialog(0);
      
      cout<<"end of case 20.  new_keyboard_entry = "<<centre_p->new_keyboard_entry<<endl;
      
      break;
    }  
	  default:
	  {
      //cout<<"default"<<endl;	//omit~~~
	  }
  }
}

void macro_builder::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void macro_builder::save_button_clicked()
{
  if(macro_function_string.startsWith("-") && macro_function_string.size() > 1)
  {
    macro_function_string.remove(0,1);
  }
  centre_p->macro_function_string = macro_function_string;
  centre_p->macro_display_string = lineEdit->text(); //macro_display_string;
  centre_p->add_waiting_screen(28);//macro_screen.  Will save macro table (see constructor)
  centre_p->screen_done = true;
}
