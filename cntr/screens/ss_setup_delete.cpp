#include <iostream>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDir>
#include "centre.hpp"
#include "ss_setup_delete.hpp"
#include "button.hpp"
#include "batch_mode_driver.hpp"

using namespace std;

ss_setup_delete::ss_setup_delete(centre*set_centre_p, batch_mode_driver* batch_mode_driver_p_s)
:screen(set_centre_p)
{
  centre_p=set_centre_p;
  batch_mode_driver_p = batch_mode_driver_p_s;


//  cout<<"ss_setup_delete::ss_setup_delete\n";
//  batch_mode_driver_p->list_ss_setup();

  choice1_p=new button("");
  choice2_p=new button("");
  choice3_p=new button("");
  choice4_p=new button("");
  choice5_p=new button("");
  choice6_p=new button("");
  more_ss_setups_p=new button("More spreadsheet setups");
//  add_ss_setup_p=new button("Add a ss_setup");
//  delete_ss_setup_p=new button("Delete a ss_setup");
  back_p=new button("Back");
  message_p=new QLabel("Select spreadsheet setup to delete");

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
  
  bottom_layout_p->addWidget(more_ss_setups_p,0,0);
//  bottom_layout_p->addWidget(add_ss_setup_p,0,1);
//  bottom_layout_p->addWidget(delete_ss_setup_p,0,2);
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
  connect(more_ss_setups_p, SIGNAL(clicked()), this, SLOT(more_ss_setups_clicked()));
//  connect(add_ss_setup_p, SIGNAL(clicked()), this, SLOT(add_ss_setup_clicked()));
//  connect(delete_ss_setup_p, SIGNAL(clicked()), this, SLOT(delete_ss_setup_clicked()));
  connect(back_p, SIGNAL(clicked()), this, SLOT(back_clicked()));
  
  QString crop_name = centre_p->crops[0].name;
  ss_setup_path = "ss_setups/";
  ss_setup_path.append(crop_name);
  ss_setup_path.append("/");
  dir_p = new QDir(ss_setup_path);

  generate_ss_setup_list();
  first_ss_setup_displayed = 0;
  display_ss_setups();
  
}

ss_setup_delete::~ss_setup_delete()
{
  delete dir_p;
  dir_p = 0;
  delete ss_setup_list_p;
  ss_setup_list_p = 0;
}

void ss_setup_delete::choice1_clicked()
{
  if(first_ss_setup_displayed<ss_setup_list_p->size())
  {
    QString ss_setup_name = ss_setup_list_p->at(first_ss_setup_displayed);
    ss_setup_name.append(".ESC3sss");
    bool result = dir_p -> remove(ss_setup_name);
    if(result == false) cout<<"failed to remove ss_setup\n";
    generate_ss_setup_list();
    display_ss_setups();
  }
}

void ss_setup_delete::choice2_clicked()
{
  if(first_ss_setup_displayed+1<ss_setup_list_p->size())
  {
    QString ss_setup_name = ss_setup_list_p->at(first_ss_setup_displayed+1);
    ss_setup_name.append(".ESC3sss");
    bool result = dir_p -> remove(ss_setup_name);
    if(result == false) cout<<"failed to remove ss_setup\n";
    generate_ss_setup_list();
    display_ss_setups();
  }
}

void ss_setup_delete::choice3_clicked()
{
  if(first_ss_setup_displayed+2<ss_setup_list_p->size())
  {
    QString ss_setup_name = ss_setup_list_p->at(first_ss_setup_displayed+2);
    ss_setup_name.append(".ESC3sss");
    bool result = dir_p -> remove(ss_setup_name);
    if(result == false) cout<<"failed to remove ss_setup\n";
    generate_ss_setup_list();
    display_ss_setups();
  }
}

void ss_setup_delete::choice4_clicked()
{
  if(first_ss_setup_displayed+3<ss_setup_list_p->size())
  {
    QString ss_setup_name = ss_setup_list_p->at(first_ss_setup_displayed+3);
    ss_setup_name.append(".ESC3sss");
    bool result = dir_p -> remove(ss_setup_name);
    if(result == false) cout<<"failed to remove ss_setup\n";
    generate_ss_setup_list();
    display_ss_setups();
  }
}

void ss_setup_delete::choice5_clicked()
{
  if(first_ss_setup_displayed+4<ss_setup_list_p->size())
  {
    QString ss_setup_name = ss_setup_list_p->at(first_ss_setup_displayed+4);
    ss_setup_name.append(".ESC3sss");
    bool result = dir_p -> remove(ss_setup_name);
    if(result == false) cout<<"failed to remove ss_setup\n";
    generate_ss_setup_list();
    display_ss_setups();
  }
}

void ss_setup_delete::choice6_clicked()
{
  if(first_ss_setup_displayed+5<ss_setup_list_p->size())
  {
    QString ss_setup_name = ss_setup_list_p->at(first_ss_setup_displayed+5);
    ss_setup_name.append(".ESC3sss");
    bool result = dir_p -> remove(ss_setup_name);
    if(result == false) cout<<"failed to remove ss_setup\n";
    generate_ss_setup_list();
    display_ss_setups();
  }
}

void ss_setup_delete::more_ss_setups_clicked()
{
  if(first_ss_setup_displayed<(ss_setup_list_p->size()-6)) first_ss_setup_displayed += 6;
  display_ss_setups();
}
/*
void ss_setup_delete::add_ss_setup_clicked()
{
  centre_p->add_waiting_screen(14);//enter_ss_setup
  batch_mode_driver_p->clear_ss_setup();
  centre_p->screen_done=true;
}

void ss_setup_delete::delete_ss_setup_clicked()
{
  centre_p->add_waiting_screen(18);//delete_ss_setup
  centre_p->screen_done=true;
}
*/
void ss_setup_delete::back_clicked()
{
  if(first_ss_setup_displayed>5) 
  {
    first_ss_setup_displayed -= 6;
    display_ss_setups();
  }
  else
  {
    centre_p->add_waiting_screen(centre_p->get_previous_screen());
    centre_p->screen_done=true;
  }
}

void ss_setup_delete::generate_ss_setup_list()
{
  ss_setup_list_p = new QStringList;
  QStringList filter;
  filter<<"*.ESC3sss";
  *ss_setup_list_p = dir_p->entryList(filter);
  for(int i=0; i<ss_setup_list_p->size(); ++i)
  {
    QString temp = ss_setup_list_p->at(i);
    temp.remove(".ESC3sss");
    ss_setup_list_p->replace(i, temp);
  }
}

void ss_setup_delete::display_ss_setups()
{
  if(first_ss_setup_displayed<ss_setup_list_p->size())    choice1_p->setText(ss_setup_list_p->at(first_ss_setup_displayed));
  else choice1_p->setText("");
  if(first_ss_setup_displayed+1<ss_setup_list_p->size())  choice2_p->setText(ss_setup_list_p->at(first_ss_setup_displayed+1));
  else choice2_p->setText("");
  if(first_ss_setup_displayed+2<ss_setup_list_p->size())  choice3_p->setText(ss_setup_list_p->at(first_ss_setup_displayed+2));
  else choice3_p->setText("");
  if(first_ss_setup_displayed+3<ss_setup_list_p->size())  choice4_p->setText(ss_setup_list_p->at(first_ss_setup_displayed+3));
  else choice4_p->setText("");
  if(first_ss_setup_displayed+4<ss_setup_list_p->size())  choice5_p->setText(ss_setup_list_p->at(first_ss_setup_displayed+4));
  else choice5_p->setText("");
  if(first_ss_setup_displayed+5<ss_setup_list_p->size())  choice6_p->setText(ss_setup_list_p->at(first_ss_setup_displayed+5));
  else choice6_p->setText("");
}
