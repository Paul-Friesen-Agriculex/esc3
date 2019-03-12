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

using namespace std;

spreadsheet_choice::spreadsheet_choice(centre*set_centre_p, batch_mode_driver* batch_mode_driver_p_s)
:screen(set_centre_p)
{
  centre_p=set_centre_p;
  batch_mode_driver_p = batch_mode_driver_p_s;


//  cout<<"spreadsheet_choice::spreadsheet_choice\n";
//  batch_mode_driver_p->list_spreadsheet();

  choice1_p=new button("");
  choice2_p=new button("");
  choice3_p=new button("");
  choice4_p=new button("");
  choice5_p=new button("");
  choice6_p=new button("");
  more_spreadsheets_p=new button("More spreadsheets");
  back_p=new button("Back");
  message_p=new QLabel("Select spreadsheet");

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
  
  bottom_layout_p->addWidget(more_spreadsheets_p,0,0);
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
  
  cout<<"1\n";
  
//  spreadsheet_path = "/media/odroid/KINGSTON/";
//  dir_p = new QDir(spreadsheet_path);
  
  
  spreadsheet_path = "/media/odroid/";
  dir_p = new QDir(spreadsheet_path);
  QStringList directory_list = dir_p->entryList(QDir::Dirs  | QDir::NoDotAndDotDot);
  if(directory_list.size()==0)
  {
    cout<<"no USB drives found\n";
  }
  else
  {
    spreadsheet_path.append(directory_list[0]);
    spreadsheet_path.append("/");
    cout<<"spreadsheet_path = "<<spreadsheet_path.toStdString()<<endl;
    dir_p -> setPath(spreadsheet_path);
  }
  
  
  
  cout<<"2\n";
  
  spreadsheet_list_p = new QStringList;
  QStringList filter;
  filter<<"*.csv";
  *spreadsheet_list_p = dir_p->entryList(filter);
  
  cout<<"3\n";
  
  for(int i=0; i<spreadsheet_list_p->size(); ++i)
  {
    QString temp = spreadsheet_list_p->at(i);
    temp.remove(".csv");
    spreadsheet_list_p->replace(i, temp);
  }
  first_spreadsheet_displayed = 0;
  display_spreadsheets();
  centre_p->batch_mode_driver_p->use_spreadsheet = true;  
  
  cout<<"4\n";
  
}

spreadsheet_choice::~spreadsheet_choice()
{
  delete dir_p;
  dir_p = 0;
  delete spreadsheet_list_p;
  spreadsheet_list_p = 0;
}

void spreadsheet_choice::choice1_clicked()
{
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
