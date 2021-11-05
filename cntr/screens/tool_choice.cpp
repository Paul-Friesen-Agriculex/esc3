#include <iostream>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QApplication>
#include "centre.hpp"
#include "tool_choice.hpp"
#include "button.hpp"
#include "help_screen.hpp"

using namespace std;

tool_choice::tool_choice(centre*set_centre_p)
:screen(set_centre_p)
{
  centre_p=set_centre_p;
  
  about_button_p = new button("About this program");
  crop_edit_button_p = new button("Edit Crops");
  communications_setup_button_p = new button("Set up communications");
  diagnostics_button_p = new button("Diagnostics");
  stop_program_button_p = new button("Stop ESC-3 program and exit to desktop");
  back_button_p = new button("Back");
  signal_port_pulse_when_endgate_closes_button_p = new QRadioButton("Send Signal to Envelope Feeder\n port when endgate closes");
  signal_port_pulse_when_endgate_closes_button_p->setChecked(centre_p->signal_port_pulse_when_endgate_closes);

  main_layout_p=new QGridLayout;
  
  main_layout_p->addWidget(about_button_p,0,0);
  main_layout_p->addWidget(back_button_p,0,1);
  main_layout_p->addWidget(communications_setup_button_p,1,1);
  main_layout_p->addWidget(crop_edit_button_p, 1, 0);
  main_layout_p->addWidget(diagnostics_button_p, 2, 0);
  main_layout_p->addWidget(stop_program_button_p, 3, 0);
  main_layout_p->addWidget(signal_port_pulse_when_endgate_closes_button_p, 2, 1);
  setLayout(main_layout_p);
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(crop_edit_button_p, SIGNAL(clicked()), this, SLOT(crop_edit_clicked()));
  connect(communications_setup_button_p, SIGNAL(clicked()), this, SLOT(communications_setup_button_clicked()));
  connect(diagnostics_button_p, SIGNAL(clicked()), this, SLOT(diagnostics_clicked()));
  connect(stop_program_button_p, SIGNAL(clicked()), this, SLOT(stop_program_clicked()));
  connect(signal_port_pulse_when_endgate_closes_button_p, SIGNAL(toggled(bool)), this, SLOT(signal_port_pulse_when_endgate_closes(bool)));
  connect(about_button_p, SIGNAL(clicked()), this, SLOT(about_button_clicked()));
}

void tool_choice::crop_edit_clicked()
{
	  centre_p->add_waiting_screen(3);//crop_edit
	  centre_p->screen_done=true;
}

void tool_choice::communications_setup_button_clicked()
{
  centre_p->add_waiting_screen(40);//communications_menu
  centre_p->screen_done = true;
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

void tool_choice::stop_program_clicked()
{
  centre_p->~centre();
  centre_p->application_p->exit(0);
}

void tool_choice::signal_port_pulse_when_endgate_closes(bool state)
{
  centre_p->signal_port_pulse_when_endgate_closes = state;
}

void tool_choice::about_button_clicked()
{
  help_screen_p = new help_screen;
  help_screen_p -> setGeometry(geometry());
  
  help_screen_p -> set_text("Copyright 2021 Agriculex Inc.\n\n"

                            "The Agriculex ESC-3 program is free software: you can redistribute it and/or modify "
                            "it under the terms of the GNU General Public License as published by "
                            "the Free Software Foundation, GPL-3.0-or-later.\n\n"
                            
                            "The Agriculex ESC-3 program is distributed in the hope that it will be useful, "
                            "but, except as noted below, WITHOUT ANY WARRANTY; without even the implied warranty of "
                            "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
                            "GNU General Public License for more details.\n\n"
                            
                            "You should have received a copy of the GNU General Public License "
                            "along with The Agriculex ESC-3 program.  If not, see <https://www.gnu.org/licenses/>.\n\n"
                            
                            "Agriculex Inc. warrants that executable files of the ESC-3 program compiled and "
                            "installed by Agriculex Inc. will function as advertized in ESC-3 machines produced "
                            "by Agriculex Inc.  ");
  
  help_screen_p -> show();
}
