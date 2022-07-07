/***********

Copyright 2021 Agriculex Inc.

This file is part of the Agriculex ESC-3 program.

The Agriculex ESC-3 program is free software: you can redistribute it 
and/or modify it under the terms of the GNU General Public License as 
published by the Free Software Foundation, GPL-3.0-or-later.

The Agriculex ESC-3 program is distributed in the hope that it will be 
useful, but, except as noted below, WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with The Agriculex ESC-3 program.  If not, see 
<https://www.gnu.org/licenses/>.

Agriculex Inc. warrants that executable files of the ESC-3 program 
compiled and installed by Agriculex Inc. will function as advertized in 
ESC-3 machines produced by Agriculex Inc.

**********/

#include <iostream>
#include <QGridLayout>
#include <QGroupBox>
#include <QDir>
#include <QMessageBox>
#include <QApplication>
#include "centre.hpp"
#include "ss_other_options.hpp"
#include "button.hpp"
#include "help_screen.hpp"

using namespace std;

ss_other_options::ss_other_options(centre* set_centre_p)
 :screen(set_centre_p)
{ 
  back_button_p = new button("Back");
  print_button_p = new button("Envelope printing");
  display_columns_button_p = new button("Choose columns to display");
  done_button_p = new button("Done");
  help_button_p = new button("Help");

  main_layout_p = new QGridLayout;

  main_layout_p -> addWidget(back_button_p,0,1);
  main_layout_p -> addWidget(print_button_p,1,0);
  main_layout_p -> addWidget(display_columns_button_p,2,0);
  main_layout_p -> addWidget(done_button_p,3,1);
  main_layout_p -> addWidget(help_button_p,3,0);
  
  setLayout(main_layout_p);

  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(print_button_p, SIGNAL(clicked()), this, SLOT(print_button_clicked()));
  connect(display_columns_button_p, SIGNAL(clicked()), this, SLOT(display_columns_button_clicked()));
  connect(done_button_p, SIGNAL(clicked()), this, SLOT(done_button_clicked()));
  connect(help_button_p, SIGNAL(clicked()), this, SLOT(help_button_clicked()));

  help_screen_p = 0;
}



void ss_other_options::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void ss_other_options::print_button_clicked()
{
  centre_p->add_waiting_screen(25);//set_envelope_size
  centre_p->screen_done=true;
}

void ss_other_options::display_columns_button_clicked()
{
  centre_p->add_waiting_screen(32);//ss_column_display_options
  centre_p->screen_done=true;
}

void ss_other_options::done_button_clicked()
{
  centre_p->add_waiting_screen(33);//ss_batch
  centre_p->screen_done=true;
}

void ss_other_options::help_button_clicked()
{
  help_screen_p = new help_screen;
  help_screen_p -> setGeometry(geometry());
  
  help_screen_p -> set_text("Envelope printing is not yet fully implemented.  "
                            "It is intended to allow a printer to be hooked up to the ESC-3.  "
                            "The machine would then print its own envelopes from the spreadsheet, "
                            "with each envelope being printed only when required, to minimize the chance of envelopes being mixed up.  "
                            "\n\n"
                            
                            "Without printing, the machine checks barcodes on pre-printed envelopes against the pack identifiers in the spreadsheet.  "
                            "If they do not match, seed will be held.  "
                            "\n\n"
                            
                            "\"Choose columns to display\" controls what columns will be displayed on the screen during counting.");
  help_screen_p -> show();
}


