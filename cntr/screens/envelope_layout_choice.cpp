#include <iostream>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDir>
#include "centre.hpp"
#include "envelope_layout_choice.hpp"
#include "button.hpp"
#include "batch_mode_driver.hpp"

using namespace std;

envelope_layout_choice::envelope_layout_choice(centre*set_centre_p, batch_mode_driver* batch_mode_driver_p_s)
:screen(set_centre_p)
{
  centre_p=set_centre_p;
  batch_mode_driver_p = batch_mode_driver_p_s;


//  cout<<"envelope_layout_choice::envelope_layout_choice\n";
//  batch_mode_driver_p->list_envelope_layout();

  choice1_p=new button("");
  choice2_p=new button("");
  choice3_p=new button("");
  choice4_p=new button("");
  choice5_p=new button("");
  choice6_p=new button("");
  more_envelope_layouts_p=new button("More envelope layouts");
  add_envelope_layout_p=new button("Add an envelope layout");
  delete_envelope_layout_p=new button("Delete an envelope layout");
  no_envelope_layout_p=new button("No envelope layout");
  back_p=new button("Back");
  message_p=new QLabel("Select envelope layout");

  top_box_p=new QGroupBox;
  middle_box_p=new QGroupBox;
  bottom_box_p=new QGroupBox;
  
  top_layout_p=new QGridLayout;
  middle_layout_p=new QGridLayout;
  bottom_layout_p=new QGridLayout;
  
  main_layout_p=new QVBoxLayout;
  
  top_layout_p->addWidget(message_p,0,0);
  top_layout_p->addWidget(back_p, 0, 1);
  top_layout_p->addWidget(no_envelope_layout_p, 1, 0);
  top_box_p->setLayout(top_layout_p);
  
  middle_layout_p->addWidget(choice1_p,0,0);
  middle_layout_p->addWidget(choice2_p,0,1);
  middle_layout_p->addWidget(choice3_p,1,0);
  middle_layout_p->addWidget(choice4_p,1,1);
  middle_layout_p->addWidget(choice5_p,2,0);
  middle_layout_p->addWidget(choice6_p,2,1);
  middle_box_p->setLayout(middle_layout_p);
  
  bottom_layout_p->addWidget(more_envelope_layouts_p,0,0);
  bottom_layout_p->addWidget(add_envelope_layout_p,0,1);
  bottom_layout_p->addWidget(delete_envelope_layout_p,0,2);
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
  connect(more_envelope_layouts_p, SIGNAL(clicked()), this, SLOT(more_envelope_layouts_clicked()));
  connect(add_envelope_layout_p, SIGNAL(clicked()), this, SLOT(add_envelope_layout_clicked()));
  connect(delete_envelope_layout_p, SIGNAL(clicked()), this, SLOT(delete_envelope_layout_clicked()));
  connect(no_envelope_layout_p, SIGNAL(clicked()), this, SLOT(no_envelope_layout_clicked()));
  connect(back_p, SIGNAL(clicked()), this, SLOT(back_clicked()));
  
  QString crop_name = centre_p->crops[0].name;
  envelope_layout_path = "envelope_layouts/";
  envelope_layout_path.append(crop_name);
  envelope_layout_path.append("/");
  dir_p = new QDir(envelope_layout_path);
  envelope_layout_list_p = new QStringList;
  QStringList filter;
  filter<<"*.ESC3";
  *envelope_layout_list_p = dir_p->entryList(filter);
  for(int i=0; i<envelope_layout_list_p->size(); ++i)
  {
    QString temp = envelope_layout_list_p->at(i);
    temp.remove(".ESC3");
    envelope_layout_list_p->replace(i, temp);
  }
  first_envelope_layout_displayed = 0;
  display_envelope_layouts();
}

envelope_layout_choice::~envelope_layout_choice()
{
  delete dir_p;
  dir_p = 0;
  delete envelope_layout_list_p;
  envelope_layout_list_p = 0;
}

void envelope_layout_choice::choice1_clicked()
{
  if(first_envelope_layout_displayed<envelope_layout_list_p->size())
  {
    envelope_layout_path.append(envelope_layout_list_p->at(first_envelope_layout_displayed));
    envelope_layout_path.append(".ESC3");
    batch_mode_driver_p->envelope_layout_path = envelope_layout_path;
    batch_mode_driver_p->load_envelope_layout();
    centre_p->add_waiting_screen(33);//ss_batch
    centre_p->screen_done=true;
  }
}

void envelope_layout_choice::choice2_clicked()
{
  if(first_envelope_layout_displayed+1<envelope_layout_list_p->size())
  {
    envelope_layout_path.append(envelope_layout_list_p->at(first_envelope_layout_displayed+1));
    envelope_layout_path.append(".ESC3");
    batch_mode_driver_p->envelope_layout_path = envelope_layout_path;
    batch_mode_driver_p->load_envelope_layout();
    centre_p->add_waiting_screen(33);
    centre_p->screen_done=true;
  }
}

void envelope_layout_choice::choice3_clicked()
{
  if(first_envelope_layout_displayed+2<envelope_layout_list_p->size())
  {
    envelope_layout_path.append(envelope_layout_list_p->at(first_envelope_layout_displayed+2));
    envelope_layout_path.append(".ESC3");
    batch_mode_driver_p->envelope_layout_path = envelope_layout_path;
    batch_mode_driver_p->load_envelope_layout();
    centre_p->add_waiting_screen(33);
    centre_p->screen_done=true;
  }
}

void envelope_layout_choice::choice4_clicked()
{
  if(first_envelope_layout_displayed+3<envelope_layout_list_p->size())
  {
    envelope_layout_path.append(envelope_layout_list_p->at(first_envelope_layout_displayed+3));
    envelope_layout_path.append(".ESC3");
    batch_mode_driver_p->envelope_layout_path = envelope_layout_path;
    batch_mode_driver_p->load_envelope_layout();
    centre_p->add_waiting_screen(33);
    centre_p->screen_done=true;
  }
}

void envelope_layout_choice::choice5_clicked()
{
  if(first_envelope_layout_displayed+4<envelope_layout_list_p->size())
  {
    envelope_layout_path.append(envelope_layout_list_p->at(first_envelope_layout_displayed+4));
    envelope_layout_path.append(".ESC3");
    batch_mode_driver_p->envelope_layout_path = envelope_layout_path;
    batch_mode_driver_p->load_envelope_layout();
    centre_p->add_waiting_screen(33);
    centre_p->screen_done=true;
  }
}

void envelope_layout_choice::choice6_clicked()
{
  if(first_envelope_layout_displayed+5<envelope_layout_list_p->size())
  {
    envelope_layout_path.append(envelope_layout_list_p->at(first_envelope_layout_displayed+5));
    envelope_layout_path.append(".ESC3");
    batch_mode_driver_p->envelope_layout_path = envelope_layout_path;
    batch_mode_driver_p->load_envelope_layout();
    centre_p->add_waiting_screen(33);
    centre_p->screen_done=true;
  }
}

void envelope_layout_choice::more_envelope_layouts_clicked()
{
  if(first_envelope_layout_displayed<(envelope_layout_list_p->size()-6)) first_envelope_layout_displayed += 6;
  display_envelope_layouts();
}

void envelope_layout_choice::add_envelope_layout_clicked()
{
  centre_p->add_waiting_screen(25);//enter_envelope_layout
  batch_mode_driver_p->clear_envelope_layout();
  batch_mode_driver_p->envelope_layout_path = "";
  centre_p->screen_done=true;
}

void envelope_layout_choice::delete_envelope_layout_clicked()
{
  centre_p->add_waiting_screen(26);//delete_envelope_layout
  centre_p->screen_done=true;
}

void envelope_layout_choice::no_envelope_layout_clicked()
{
  centre_p->add_waiting_screen(33);//ss_batch
  centre_p->screen_done=true;
}

void envelope_layout_choice::back_clicked()
{
  if(first_envelope_layout_displayed>5) 
  {
    first_envelope_layout_displayed -= 6;
    display_envelope_layouts();
  }
  else
  {
    centre_p->add_waiting_screen(centre_p->get_previous_screen());
    centre_p->screen_done=true;
  }
}

void envelope_layout_choice::display_envelope_layouts()
{
  if(first_envelope_layout_displayed<envelope_layout_list_p->size())    choice1_p->setText(envelope_layout_list_p->at(first_envelope_layout_displayed));
  else choice1_p->setText("");
  if(first_envelope_layout_displayed+1<envelope_layout_list_p->size())  choice2_p->setText(envelope_layout_list_p->at(first_envelope_layout_displayed+1));
  else choice2_p->setText("");
  if(first_envelope_layout_displayed+2<envelope_layout_list_p->size())  choice3_p->setText(envelope_layout_list_p->at(first_envelope_layout_displayed+2));
  else choice3_p->setText("");
  if(first_envelope_layout_displayed+3<envelope_layout_list_p->size())  choice4_p->setText(envelope_layout_list_p->at(first_envelope_layout_displayed+3));
  else choice4_p->setText("");
  if(first_envelope_layout_displayed+4<envelope_layout_list_p->size())  choice5_p->setText(envelope_layout_list_p->at(first_envelope_layout_displayed+4));
  else choice5_p->setText("");
  if(first_envelope_layout_displayed+5<envelope_layout_list_p->size())  choice6_p->setText(envelope_layout_list_p->at(first_envelope_layout_displayed+5));
  else choice6_p->setText("");
}
