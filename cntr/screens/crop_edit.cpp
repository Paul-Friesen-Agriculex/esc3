//-----------------------------------------------------
//Class Name: crop_edit
//Created: August 08,2017
//Version 1.0
//--------------------------------------------------------
#include <iostream>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include "centre.hpp"
#include "crop_edit.hpp"
#include "button.hpp"

using namespace std;

crop_edit::crop_edit(centre*set_centre_p)
:screen(set_centre_p)
{
	  centre_p=set_centre_p;
	  
	  cropAddButn_p = new button("Add a New Crop");
	  cropDelButn_p = new button("Delete a Crop");
	  cropEditButn_p = new button("Edit Existing Crop Name");
	  cropRecalibrateButn_p = new button("Recalibrate/Change Settings\nExisting Crop");
	  
	  backButn_p = new button("Back");
	  message_p=new QLabel("Edit Crop...");

	  top_box_p =new QGroupBox;
	  buttonlist_box_p =new QGroupBox;
	  
	  top_layout_p=new QGridLayout;
	  buttonlist_layout_p=new QGridLayout;
	 
	  main_layout_p=new QVBoxLayout;
	  
	  top_layout_p->addWidget(message_p,0,0);
	  top_layout_p->addWidget(backButn_p, 0, 1);
	  top_box_p->setLayout(top_layout_p);
	  
	  buttonlist_layout_p->addWidget(cropAddButn_p,0,0);
	  buttonlist_layout_p->addWidget(cropDelButn_p,1,0);
	  buttonlist_layout_p->addWidget(cropEditButn_p,2,0);
	  buttonlist_layout_p->addWidget(cropRecalibrateButn_p,3,0);
	  buttonlist_box_p->setLayout(buttonlist_layout_p);
	  
	  main_layout_p->addWidget(top_box_p);
	  main_layout_p->addWidget(buttonlist_box_p);
	  
	  setLayout(main_layout_p);
	  
	  first_crop_displayed = 0;
	  
	  connect(cropAddButn_p, SIGNAL(clicked()), this, SLOT(cropAddButn_clicked()));
	  connect(cropDelButn_p, SIGNAL(clicked()), this, SLOT(cropDelButn_clicked()));
	  connect(cropEditButn_p, SIGNAL(clicked()), this, SLOT(cropEditButn_clicked()));
	  connect(cropRecalibrateButn_p, SIGNAL(clicked()), this, SLOT(cropRecalibrate_clicked()));
	  connect(backButn_p, SIGNAL(clicked()), this, SLOT(backButn_clicked()));
}

void crop_edit::cropAddButn_clicked()
{
	  centre_p->add_waiting_screen(4);
	  centre_p->screen_done=true;
}

void crop_edit::cropDelButn_clicked()
{
	  centre_p->add_waiting_screen(7);//crop_delete
	  centre_p->screen_done=true;
}

void crop_edit::cropEditButn_clicked()
{
	  centre_p->add_waiting_screen(3);//back here to crop_edit
    centre_p->add_waiting_screen(4);//crop name entry
	  centre_p->add_waiting_screen(1);//crop choice
	  centre_p->changing_existing_crop = true;//cause crop_name_entry to overwrite instead of creating new crop
	  centre_p->screen_done=true;
}

void crop_edit::cropRecalibrate_clicked()
{
	  centre_p->add_waiting_screen(10);//calibrate
	  centre_p->add_waiting_screen(1);//crop choice
	  centre_p->recalibrating = true;
	  centre_p->screen_done = true;
}

void crop_edit::backButn_clicked()
{
	// back to main page--Home page
	centre_p->add_waiting_screen(centre_p->get_previous_screen());
	centre_p->screen_done=true;
  
}



