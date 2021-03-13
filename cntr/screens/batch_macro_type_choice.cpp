#include <iostream>
#include "button.hpp"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include "batch_mode_driver.hpp"

#include "batch_macro_type_choice.hpp"

using namespace std;

batch_macro_type_choice::batch_macro_type_choice(centre*set_centre_p, batch_mode_driver* set_batch_mode_driver_p)
:screen(set_centre_p)
{
  batch_mode_driver_p = set_batch_mode_driver_p;
  
  back_button_p = new button("Back");
  instruction_label_p = new QLabel("Select type of macro to enter");
  pack_macro_button_p = new button("Enter pack macro");
  dump_macro_button_p = new button("Enter dump macro");
  substitution_macro_button_p = new button("Enter substitution macro");
  setup_button_p = new button("Set up communications method");
  done_button_p = new button("Done");
  
  pack_label_p = new QLabel("To be issued after each seed pack is collected, if there is no substitution");
  dump_label_p = new QLabel("To be issued after each lot of seed is dumped");
  substitution_label_p = new QLabel("To be issued after each pack if a seed substitution is made");
  
  main_layout_p = new QGridLayout;
  main_layout_p->addWidget(instruction_label_p, 0, 0);
  main_layout_p->addWidget(back_button_p, 0, 1);
  main_layout_p->addWidget(pack_macro_button_p, 1, 0);
  main_layout_p->addWidget(pack_label_p, 1, 1);
  main_layout_p->addWidget(dump_macro_button_p, 2, 0);
  main_layout_p->addWidget(dump_label_p, 2, 1);
  main_layout_p->addWidget(substitution_macro_button_p, 3, 0);
  main_layout_p->addWidget(substitution_label_p, 3, 1);
  main_layout_p->addWidget(setup_button_p, 4, 1);
  main_layout_p->addWidget(done_button_p, 5, 2);
  setLayout(main_layout_p);
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(pack_macro_button_p, SIGNAL(clicked()), this, SLOT(pack_macro_button_clicked()));
  connect(dump_macro_button_p, SIGNAL(clicked()), this, SLOT(dump_macro_button_clicked()));
  connect(substitution_macro_button_p, SIGNAL(clicked()), this, SLOT(substitution_macro_button_clicked()));
  connect(setup_button_p, SIGNAL(clicked()), this, SLOT(setup_button_clicked()));
  connect(done_button_p, SIGNAL(clicked()), this, SLOT(done_button_clicked()));
}

void batch_macro_type_choice::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}


void batch_macro_type_choice::pack_macro_button_clicked()
{
  centre_p->macro_type_for_entry = 1;//batch pack macro
  centre_p->add_waiting_screen(50);//come back here
  centre_p->add_waiting_screen(28);//macro_screen
  centre_p->screen_done=true;
}

void batch_macro_type_choice::dump_macro_button_clicked()
{
  centre_p->macro_type_for_entry = 2;//dump macro
  centre_p->add_waiting_screen(50);//come back here
  centre_p->add_waiting_screen(28);//macro_screen
  centre_p->screen_done=true;
}

void batch_macro_type_choice::substitution_macro_button_clicked()
{
  centre_p->macro_type_for_entry = 3;//substitution macro
  centre_p->add_waiting_screen(50);//come back here
  centre_p->add_waiting_screen(28);//macro_screen
  centre_p->screen_done=true;
}

void batch_macro_type_choice::setup_button_clicked()
{
  centre_p->add_waiting_screen(50);//come back here when done
  centre_p->add_waiting_screen(40);//communications_menu
  centre_p->screen_done=true;
}

void batch_macro_type_choice::done_button_clicked()
{
  centre_p->add_waiting_screen(16);//batch_options
  centre_p->screen_done=true;
}
