//#include <QtWidgets>
#include <iostream>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include "centre.hpp"
#include "crop_delete.hpp"
#include "button.hpp"

using namespace std;

crop_delete::crop_delete(centre*set_centre_p)
:screen(set_centre_p)
{
  centre_p=set_centre_p;
  
  choice1_p=new button("");
  choice2_p=new button("");
  choice3_p=new button("");
  choice4_p=new button("");
  choice5_p=new button("");
  choice6_p=new button("");
  more_crops_p=new button("More Crops");
  done_p=new button("Done");
  back_p=new button("Back");
  message_p=new QLabel("Select Crop to Delete");

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
  
  bottom_layout_p->addWidget(more_crops_p,0,0);
  bottom_layout_p->addWidget(done_p,0,1);
  bottom_box_p->setLayout(bottom_layout_p);
  
  main_layout_p->addWidget(top_box_p);
  main_layout_p->addWidget(middle_box_p);
  main_layout_p->addWidget(bottom_box_p);
  
  setLayout(main_layout_p);
  
  first_crop_displayed = 0;
  display_crops();
  
  connect(choice1_p, SIGNAL(clicked()), this, SLOT(choice1_clicked()));
  connect(choice2_p, SIGNAL(clicked()), this, SLOT(choice2_clicked()));
  connect(choice3_p, SIGNAL(clicked()), this, SLOT(choice3_clicked()));
  connect(choice4_p, SIGNAL(clicked()), this, SLOT(choice4_clicked()));
  connect(choice5_p, SIGNAL(clicked()), this, SLOT(choice5_clicked()));
  connect(choice6_p, SIGNAL(clicked()), this, SLOT(choice6_clicked()));
  connect(more_crops_p, SIGNAL(clicked()), this, SLOT(more_crops_clicked()));
  connect(done_p, SIGNAL(clicked()), this, SLOT(done_clicked()));
  connect(back_p, SIGNAL(clicked()), this, SLOT(back_clicked()));
}

void crop_delete::choice1_clicked()
{
  centre_p->delete_crop(first_crop_displayed);
  display_crops();
}

void crop_delete::choice2_clicked()
{
  centre_p->delete_crop(first_crop_displayed+1);
  display_crops();
}

void crop_delete::choice3_clicked()
{
  centre_p->delete_crop(first_crop_displayed+2);
  display_crops();
}

void crop_delete::choice4_clicked()
{
  centre_p->delete_crop(first_crop_displayed+3);
  display_crops();
}

void crop_delete::choice5_clicked()
{
  centre_p->delete_crop(first_crop_displayed+4);
  display_crops();
}

void crop_delete::choice6_clicked()
{
  centre_p->delete_crop(first_crop_displayed+5);
  display_crops();
}

void crop_delete::more_crops_clicked()
{
  if(first_crop_displayed<88) first_crop_displayed += 6;
  display_crops();
}

void crop_delete::done_clicked()
{
  centre_p->add_waiting_screen(3);//crop_edit
  centre_p->screen_done=true;
}

void crop_delete::back_clicked()
{
  if(first_crop_displayed>5) 
  {
    first_crop_displayed -= 6;
    display_crops();
  }
  else
  {
    centre_p->add_waiting_screen(centre_p->get_previous_screen());
    centre_p->screen_done=true;
  }
}

void crop_delete::display_crops()
{
  choice1_p->setText(centre_p->crops[first_crop_displayed].name);
  choice2_p->setText(centre_p->crops[first_crop_displayed+1].name);
  choice3_p->setText(centre_p->crops[first_crop_displayed+2].name);
  choice4_p->setText(centre_p->crops[first_crop_displayed+3].name);
  choice5_p->setText(centre_p->crops[first_crop_displayed+4].name);
  choice6_p->setText(centre_p->crops[first_crop_displayed+5].name);
}

