#include <iostream>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include "centre.hpp"
#include "tool_choice.hpp"
#include "button.hpp"

using namespace std;

tool_choice::tool_choice(centre*set_centre_p)
:screen(set_centre_p)
{
  centre_p=set_centre_p;
  
  crop_edit_button_p = new button("Edit Crops");
  diagnostics_button_p = new button("Diagnostics");
  back_button_p = new button("Back");

  main_layout_p=new QGridLayout;
  
  main_layout_p->addWidget(back_button_p,0,1);
  main_layout_p->addWidget(crop_edit_button_p, 1, 0);
  main_layout_p->addWidget(diagnostics_button_p, 2, 0);
  
  setLayout(main_layout_p);
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(crop_edit_button_p, SIGNAL(clicked()), this, SLOT(crop_edit_clicked()));
  connect(diagnostics_button_p, SIGNAL(clicked()), this, SLOT(diagnostics_clicked()));
}

void tool_choice::crop_edit_clicked()
{
	  centre_p->add_waiting_screen(3);//crop_edit
	  centre_p->screen_done=true;
}

void tool_choice::diagnostics_clicked()
{
	  centre_p->add_waiting_screen(8);//playback
	  centre_p->screen_done=true;
}

void tool_choice::back_button_clicked()
{
	centre_p->add_waiting_screen(centre_p->get_previous_screen());
	centre_p->screen_done=true;
}



