#include <iostream>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDir>
#include "centre.hpp"
#include "spreadsheet_choice.hpp"
#include "button.hpp"
#include "batch_mode_driver.hpp"
#include "help_screen.hpp"

using namespace std;

spreadsheet_choice::spreadsheet_choice(centre*set_centre_p, batch_mode_driver* batch_mode_driver_p_s)
:screen(set_centre_p)
{
  centre_p=set_centre_p;
  batch_mode_driver_p = batch_mode_driver_p_s;

  choice1_p=new button("");
  choice2_p=new button("");
  choice3_p=new button("");
  choice4_p=new button("");
  choice5_p=new button("");
  choice6_p=new button("");
  more_spreadsheets_p=new button("More spreadsheets");
  back_p=new button("Back");
  help_p = new button("Help");
  message_p=new QLabel(" \
Select spreadsheet.\n \
Spreadsheets must be in CSV format and\n \
in the root directory of a USB stick\n \
attached to the machine.");

  top_box_p=new QGroupBox;
  middle_box_p=new QGroupBox;
  bottom_box_p=new QGroupBox;
  
  top_layout_p=new QGridLayout;
  middle_layout_p=new QGridLayout;
  bottom_layout_p=new QGridLayout;
  
  main_layout_p=new QVBoxLayout;
  
  top_layout_p->addWidget(message_p,0,0);
  top_layout_p->addWidget(back_p, 0, 1);
  top_box_p->setLayout(top_layout_p);
  
  middle_layout_p->addWidget(choice1_p,0,0);
  middle_layout_p->addWidget(choice2_p,0,1);
  middle_layout_p->addWidget(choice3_p,1,0);
  middle_layout_p->addWidget(choice4_p,1,1);
  middle_layout_p->addWidget(choice5_p,2,0);
  middle_layout_p->addWidget(choice6_p,2,1);
  middle_box_p->setLayout(middle_layout_p);
  
  bottom_layout_p->addWidget(help_p,0,0);
  bottom_layout_p->addWidget(more_spreadsheets_p,0,1);
  bottom_box_p->setLayout(bottom_layout_p);
  
  main_layout_p->addWidget(top_box_p);
  main_layout_p->addWidget(middle_box_p);
  main_layout_p->addWidget(bottom_box_p);
  
  setLayout(main_layout_p);
  
  connect(choice1_p, SIGNAL(clicked()), this, SLOT(choice1_clicked()));
  connect(choice2_p, SIGNAL(clicked()), this, SLOT(choice2_clicked()));
  connect(choice3_p, SIGNAL(clicked()), this, SLOT(choice3_clicked()));
  connect(choice4_p, SIGNAL(clicked()), this, SLOT(choice4_clicked()));
  connect(choice5_p, SIGNAL(clicked()), this, SLOT(choice5_clicked()));
  connect(choice6_p, SIGNAL(clicked()), this, SLOT(choice6_clicked()));
  connect(more_spreadsheets_p, SIGNAL(clicked()), this, SLOT(more_spreadsheets_clicked()));
  connect(back_p, SIGNAL(clicked()), this, SLOT(back_clicked()));
  connect(help_p, SIGNAL(clicked()), this, SLOT(help_clicked()));
  
  first_spreadsheet_displayed = 0;
  spreadsheet_path = "/media/odroid/";
  dir_p = new QDir(spreadsheet_path);
  QStringList directory_list = dir_p->entryList(QDir::Dirs  | QDir::NoDotAndDotDot);
  if(directory_list.size()==0)
  {
    cout<<"no USB drives found\n";
    message_p->setText(" \
NO SPREADSHEET FOUND.\n\n \
Spreadsheets must be in CSV format and\n \
in the root directory of a USB stick\n \
attached to the machine.");
    spreadsheet_list_p = 0;
  }
  else
  {
    spreadsheet_path.append(directory_list[0]);
    spreadsheet_path.append("/");
    dir_p -> setPath(spreadsheet_path);
    spreadsheet_list_p = new QStringList;
    QStringList filter;
    filter<<"*.csv";
    *spreadsheet_list_p = dir_p->entryList(filter);
    for(int i=0; i<spreadsheet_list_p->size(); ++i)
    {
      QString temp = spreadsheet_list_p->at(i);
      temp.remove(".csv");
      spreadsheet_list_p->replace(i, temp);
    }
    display_spreadsheets();
    centre_p->batch_mode_driver_p->use_spreadsheet = true;  
  }
  
}

spreadsheet_choice::~spreadsheet_choice()
{
  delete dir_p;
  dir_p = 0;
  if (spreadsheet_list_p!=0) delete spreadsheet_list_p;
  spreadsheet_list_p = 0;
}

void spreadsheet_choice::choice1_clicked()
{
  if(spreadsheet_list_p==0) return;//no USB drives found
  if(first_spreadsheet_displayed<spreadsheet_list_p->size())
  {
    spreadsheet_path.append(spreadsheet_list_p->at(first_spreadsheet_displayed));
    spreadsheet_path.append(".csv");
    batch_mode_driver_p->spreadsheet_filename = spreadsheet_path;
    batch_mode_driver_p->load_spreadsheet(spreadsheet_path);
    centre_p->add_waiting_screen(22);
    centre_p->screen_done=true;
  }
}

void spreadsheet_choice::choice2_clicked()
{
  if(spreadsheet_list_p==0) return;//no USB drives found
  if(first_spreadsheet_displayed+1<spreadsheet_list_p->size())
  {
    spreadsheet_path.append(spreadsheet_list_p->at(first_spreadsheet_displayed+1));
    spreadsheet_path.append(".csv");
    batch_mode_driver_p->spreadsheet_filename = spreadsheet_path;
    batch_mode_driver_p->load_spreadsheet(spreadsheet_path);
    centre_p->add_waiting_screen(22);
    centre_p->screen_done=true;
  }
}

void spreadsheet_choice::choice3_clicked()
{
  if(spreadsheet_list_p==0) return;//no USB drives found
  if(first_spreadsheet_displayed+2<spreadsheet_list_p->size())
  {
    spreadsheet_path.append(spreadsheet_list_p->at(first_spreadsheet_displayed+2));
    spreadsheet_path.append(".csv");
    batch_mode_driver_p->spreadsheet_filename = spreadsheet_path;
    batch_mode_driver_p->load_spreadsheet(spreadsheet_path);
    centre_p->add_waiting_screen(22);
    centre_p->screen_done=true;
  }
}

void spreadsheet_choice::choice4_clicked()
{
  if(spreadsheet_list_p==0) return;//no USB drives found
  if(first_spreadsheet_displayed+3<spreadsheet_list_p->size())
  {
    spreadsheet_path.append(spreadsheet_list_p->at(first_spreadsheet_displayed+3));
    spreadsheet_path.append(".csv");
    batch_mode_driver_p->spreadsheet_filename = spreadsheet_path;
    batch_mode_driver_p->load_spreadsheet(spreadsheet_path);
    centre_p->add_waiting_screen(22);
    centre_p->screen_done=true;
  }
}

void spreadsheet_choice::choice5_clicked()
{
  if(spreadsheet_list_p==0) return;//no USB drives found
  if(first_spreadsheet_displayed+4<spreadsheet_list_p->size())
  {
    spreadsheet_path.append(spreadsheet_list_p->at(first_spreadsheet_displayed+4));
    spreadsheet_path.append(".csv");
    batch_mode_driver_p->spreadsheet_filename = spreadsheet_path;
    batch_mode_driver_p->load_spreadsheet(spreadsheet_path);
    centre_p->add_waiting_screen(22);
    centre_p->screen_done=true;
  }
}

void spreadsheet_choice::choice6_clicked()
{
  if(spreadsheet_list_p==0) return;//no USB drives found
  if(first_spreadsheet_displayed+5<spreadsheet_list_p->size())
  {
    spreadsheet_path.append(spreadsheet_list_p->at(first_spreadsheet_displayed+5));
    spreadsheet_path.append(".csv");
    batch_mode_driver_p->spreadsheet_filename = spreadsheet_path;
    batch_mode_driver_p->load_spreadsheet(spreadsheet_path);
    centre_p->add_waiting_screen(22);
    centre_p->screen_done=true;
  }
}

void spreadsheet_choice::more_spreadsheets_clicked()
{
  if(spreadsheet_list_p==0) return;//no USB drives found
  if(first_spreadsheet_displayed<(spreadsheet_list_p->size()-6)) first_spreadsheet_displayed += 6;
  display_spreadsheets();
}

void spreadsheet_choice::back_clicked()
{
  if(first_spreadsheet_displayed>5) 
  {
    first_spreadsheet_displayed -= 6;
    display_spreadsheets();
  }
  else
  {
    centre_p->add_waiting_screen(centre_p->get_previous_screen());
    centre_p->screen_done=true;
  }
}

void spreadsheet_choice::help_clicked()
{
  help_screen_p = new help_screen;
  help_screen_p -> setGeometry(geometry());
  help_screen_p -> set_text(
    "Spreadsheets must be in CSV format and in the root directory of a USB stick attached to the machine."
    "\n\n"
    "The top row of the spreadsheet should be column headings.  "
    "These will be read by the ESC-3 and treated as labels for the data under them.  "
    "The remaining rows of the spreadsheet each represent one pack to be filled.  "
    "\n\n"
    
    "There must be columns to fill the following roles: "
    "\n\n"
    
    "1 - A column of material identification codes, identifying the seed lot to be used for that packet.  "
    "This should correspond to barcodes that you will scan to identify the material.  "
    "Normally, multiple rows will have the same material identification code.  "
    "They do not need to be together; the program will look for all rows with the same code and fill them from the same source, "
    "wherever they are located in the spreadsheet.  "
    "\n\n"
    
    "2 - A column of numbers which are the required number of seeds for each packet. "
    "\n\n"
    
    "3 - A column of unique identifiers for each packet.  "
    "These identifiers should correspond to barcodes on each packet, which you will scan to identify the packet.  "
    "\n\n"
    
    "You can choose columns to fill these roles, so order is not important.  "
    "You can also choose columns for other optional roles as follows:"
    "\n\n"
    
    "4 - Print time column.  "
    "If you are printing envelopes or labels with an attached printer, the time of printing will be recorded here.  "
    "\n\n"
    
    "5 - Fill time column.  "
    "If chosen, the time the envelope was filled will be recorded here.  "
    "\n\n"
    
    "6 - Dump count column.  "
    "If chosen, the number of seeds dumped after all packets are filled is recorded here.  "
    "You can use this for inventory purposes.  "
    "(If you want an accurate count, make sure the dump speed is not set too high.)"
    "\n\n"
    
    "In operation, you scan a material identification code from your seed lot.  "
    "The program picks all rows from your spreadsheet with that code for filling.  "
    "After all packets are filled, remaining seed dumps automatically.  "
    "\n\n"
    
    );
  help_screen_p -> show();
}

void spreadsheet_choice::display_spreadsheets()
{
  if(first_spreadsheet_displayed<spreadsheet_list_p->size())    choice1_p->setText(spreadsheet_list_p->at(first_spreadsheet_displayed));
  else choice1_p->setText("");
  if(first_spreadsheet_displayed+1<spreadsheet_list_p->size())  choice2_p->setText(spreadsheet_list_p->at(first_spreadsheet_displayed+1));
  else choice2_p->setText("");
  if(first_spreadsheet_displayed+2<spreadsheet_list_p->size())  choice3_p->setText(spreadsheet_list_p->at(first_spreadsheet_displayed+2));
  else choice3_p->setText("");
  if(first_spreadsheet_displayed+3<spreadsheet_list_p->size())  choice4_p->setText(spreadsheet_list_p->at(first_spreadsheet_displayed+3));
  else choice4_p->setText("");
  if(first_spreadsheet_displayed+4<spreadsheet_list_p->size())  choice5_p->setText(spreadsheet_list_p->at(first_spreadsheet_displayed+4));
  else choice5_p->setText("");
  if(first_spreadsheet_displayed+5<spreadsheet_list_p->size())  choice6_p->setText(spreadsheet_list_p->at(first_spreadsheet_displayed+5));
  else choice6_p->setText("");
}
