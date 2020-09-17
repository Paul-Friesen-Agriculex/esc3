#include <iostream>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include "centre.hpp"
#include "macro_screen.hpp"
#include "button.hpp"
#include "help_screen.hpp"
#include "macro_builder.hpp"

#include <QSignalMapper>
#include <QTabWidget>
#include <QFormLayout>

using namespace std;

macro_builder::macro_builder(centre* centre_p_s)
:screen(centre_p_s)
{    
  form_p = new QVBoxLayout(this);
  QSignalMapper* signalMapper = new QSignalMapper (this);
  instruction_label_p = new QLabel("If this entry is active, items listed will be sent out using the selected communications method.");  
  instruction_label_p -> setWordWrap(true);
  lineEdit = new QLineEdit;//(&dialog);
  lineEdit->setPlaceholderText("Click buttons to add preloaded values or type custom text here.");
  lineEdit->setAlignment(Qt::AlignCenter);
  lineEdit->setFixedWidth(650);
  lineEdit->setFixedHeight(50);
  lineEdit->setText(centre_p->get_macro_display_string());
  
  totalize_count_button = new QPushButton(tr("Count"), this);
  text_entry_button = new QPushButton(tr("Enter text"), this);
  batch_count_button = new QPushButton(tr("Batch count"), this);
  seed_button = new QPushButton(tr("Seed type"), this);
  barcode_button1 = new QPushButton(tr("Barcode 1"), this);
  barcode_button2 = new QPushButton(tr("Barcode 2"), this);
  barcode_button3 = new QPushButton(tr("Barcode 3"), this);
  barcode_button4 = new QPushButton(tr("Barcode 4"), this);
  weight_button = new QPushButton(tr("Weight"), this);
  lotcode_button = new QPushButton(tr("Seed lot barcode"), this);
  packcode_button = new QPushButton(tr("Pack barcode"), this);
  substitution_button = new QPushButton(tr("Substitution barcode"), this);
  dump_count_button = new QPushButton(tr("Dump count"), this);
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
  
  totalize_count_button->setFixedSize(macro_button_width,macro_button_height);
  text_entry_button->setFixedSize(macro_button_width,macro_button_height);
  seed_button->setFixedSize(macro_button_width,macro_button_height);
  barcode_button1->setFixedSize(macro_button_width,macro_button_height);
  barcode_button2->setFixedSize(macro_button_width,macro_button_height);
  barcode_button3->setFixedSize(macro_button_width,macro_button_height);
  barcode_button4->setFixedSize(macro_button_width,macro_button_height);
  weight_button->setFixedSize(macro_button_width,macro_button_height);
  lotcode_button->setFixedSize(macro_button_width,macro_button_height);
  packcode_button->setFixedSize(macro_button_width,macro_button_height);
  substitution_button->setFixedSize(macro_button_width,macro_button_height);
  dump_count_button->setFixedSize(macro_button_width,macro_button_height);
  batch_count_button->setFixedSize(macro_button_width,macro_button_height);
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
  
  top_line_p = new QHBoxLayout(this);
  top_line_p->addWidget(instruction_label_p);
  top_line_p->addWidget(back_button_p);

  input_line_modify = new QHBoxLayout(this);
  input_line_modify->addWidget(lineEdit);
  input_line_modify->addWidget(remove_last);

  form_p->addLayout(top_line_p);
  form_p->addLayout(input_line_modify);

  totalize_box = new QGroupBox();
  barcodes = new QVBoxLayout;
  barcodes_columns = new QHBoxLayout;
  barcodes_left = new QVBoxLayout;
  barcodes_right = new QVBoxLayout;
  totalize_box_label = new QLabel(tr("Data Items"));

  barcodes->addWidget(totalize_box_label);
  barcodes_left->addWidget(totalize_count_button);
  barcodes_left->addWidget(weight_button);		//yet to be implemented
  barcodes_left->addWidget(batch_count_button);
  barcodes_left->addWidget(dump_count_button);
  barcodes_left->addWidget(text_entry_button);
  barcodes_left->addWidget(seed_button);

  barcodes_right->addWidget(barcode_button1);
  barcodes_right->addWidget(barcode_button2);
  barcodes_right->addWidget(barcode_button3);
  barcodes_right->addWidget(barcode_button4);
  barcodes_right->addWidget(lotcode_button);  
  barcodes_right->addWidget(packcode_button);  
  barcodes_right->addWidget(substitution_button);  
  
  barcodes_columns->addLayout(barcodes_left);
  barcodes_columns->addLayout(barcodes_right);
  barcodes->addLayout(barcodes_columns);
  totalize_box->setLayout(barcodes);
  
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
  accessibility_keys_container->addWidget(save_button_p);
  accessibility_button_box->setLayout(accessibility_keys_container);
  
  accessibility_keys_container->setAlignment(Qt::AlignTop);
  accessibility_box_label->setStyleSheet("QLabel { font: bold;}");
  accessibility_button_box->setStyleSheet("QGroupBox {  border: 2px solid gray;"
                     "border-radius: 4px;"
                     "background-color: transparent;}");
  QHBoxLayout *macro_menu_main_horizontal = new QHBoxLayout;
  macro_menu_main_horizontal->addWidget(totalize_box);
  macro_menu_main_horizontal->addWidget(accessibility_button_box);
  form_p->addLayout(macro_menu_main_horizontal);

  connect(totalize_count_button, SIGNAL(clicked()), signalMapper, SLOT(map()));	//totalize mode variables
  connect(text_entry_button, SIGNAL(clicked()), signalMapper, SLOT(map()));
  connect(seed_button, SIGNAL(clicked()), signalMapper, SLOT(map()));
  connect(weight_button, SIGNAL(clicked()), signalMapper, SLOT(map()));
  connect(barcode_button1, SIGNAL(clicked()), signalMapper, SLOT(map()));
  connect(barcode_button2, SIGNAL(clicked()), signalMapper, SLOT(map()));
  connect(barcode_button3, SIGNAL(clicked()), signalMapper, SLOT(map()));
  connect(barcode_button4, SIGNAL(clicked()), signalMapper, SLOT(map()));
  
  connect(batch_count_button, SIGNAL(clicked()), signalMapper, SLOT(map()));		//batch mode variables
  connect(lotcode_button, SIGNAL(clicked()), signalMapper, SLOT(map()));
  connect(packcode_button, SIGNAL(clicked()), signalMapper, SLOT(map()));
  connect(substitution_button, SIGNAL(clicked()), signalMapper, SLOT(map()));  //not yet implemented~~~~
  connect(dump_count_button, SIGNAL(clicked()), signalMapper, SLOT(map()));
  
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
  signalMapper -> setMapping (seed_button, 2);
  signalMapper -> setMapping (weight_button, 18);
  signalMapper -> setMapping (barcode_button1, 3);
  signalMapper -> setMapping (barcode_button2, 4);
  signalMapper -> setMapping (barcode_button3, 5);
  signalMapper -> setMapping (barcode_button4, 6);
  
  signalMapper -> setMapping (batch_count_button, 19);			  //batch mode variables
  signalMapper -> setMapping (lotcode_button, 15);
  signalMapper -> setMapping (packcode_button, 16);	
  signalMapper -> setMapping (dump_count_button, 18);	  
  signalMapper -> setMapping (substitution_button, 21);	  
  
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
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(save_button_p, SIGNAL(clicked()), this, SLOT(save_button_clicked()));

  if(centre_p->new_keyboard_entry)//signals that a string was entered on the keyboard to add to the macro
  {          
    centre_p->new_keyboard_entry = false;
    QString string = centre_p->get_macro();
    string.append(QChar(2));//start of text
    string.append(centre_p->keyboard_return_string);
    string . append(QChar(3));//end of text
    centre_p->set_macro(string);
    lineEdit->setText(centre_p->get_macro_display_string());
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

  setLayout(form_p);
}

void macro_builder::dialogbox_buttons(int n)
{
  QString macro_string = centre_p->get_macro();

  switch(n){	  //Generate macro key display sequence and output sequence
	  case 1:
	  {
      macro_string.append("c");
      break;
	  }
	  case 2:
	  {
      macro_string.append("s");
      break;
	  }
	  case 3:
	  {
      macro_string.append("1");
      break;
	  }
	  case 4:
	  {
      macro_string.append("2");
      break;
	  }
	  case 5:
	  {
      macro_string.append("3");
      break;
	  }
	  case 6:
	  {
      macro_string.append("4");
      break;
	  }
	  case 7:
	  {
      macro_string.append("N");
      break;
	  }
	  case 8:
	  {
      macro_string.append("T");
      break;
	  }
	  case 9:
	  {
      macro_string.append("S");
      break;
	  }
	  case 10:
	  {
      macro_string.append("U");
      break;
	  }
	  case 11:
	  {
      macro_string.append("D");
      break;
	  }
	  case 12:
	  {
      macro_string.append("L");
      break;
	  }
	  case 13:
	  {
      macro_string.append("R");
      break;
	  }
	  case 14:
	  {
      macro_string.clear();
      break;
	  }
	  case 15:
	  {
      macro_string.append("l");
      break;
	  }
  	case 16:
  	{
      macro_string.append("1");
      break;
	  }
  	case 17: //remove_last
  	{
      if(macro_string[macro_string.size()-1] == QChar(3))//last item is a text string.  Remove whole text string
      {
        while(macro_string.size() > 0)
        {
          QChar end_char = macro_string[macro_string.size()-1];
          macro_string.remove(macro_string.size()-1, 1);
          if(end_char == QChar(2)) break;
        }
      }
      else
      {
        macro_string.remove(macro_string.size()-1, 1);
      }
      break;
	  }
    case 18:
    {
		  macro_string.append("d");
      break;
    }
    case 19:
    {
		  macro_string.append("c");
      break;
    }
    case 20://exit this screen for keyboard entry
    {
      
      centre_p->new_keyboard_entry = true;//This will tell this screen's constructor that it is returning from keyboard entry
      centre_p->keyboard_message_string = "Enter characters to add to the macro.";
      centre_p->add_waiting_screen(38);//come back here to macro_builder
      centre_p->add_waiting_screen(100);//keyboard
      centre_p->screen_done = true;
      break;
    }  
    case 21:
    {
		  macro_string.append("u");
      break;
    }
	  default:
	  {
      cout<<"macro_builder::dialogbox_buttons default"<<endl;
	  }
  }
  centre_p->set_macro(macro_string);
  lineEdit->setText(centre_p->get_macro_display_string());
}

void macro_builder::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void macro_builder::save_button_clicked()
{
  centre_p->save_macros();
  centre_p->add_waiting_screen(28);//macro_screen.
  centre_p->screen_done = true;
}
