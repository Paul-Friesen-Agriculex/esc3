#include <iostream>
#include <QGridLayout>
#include <QTimer>
#include <QSpinBox>
#include <QTextEdit>
#include <QLabel>
#include "centre.hpp"
#include "enter_program.hpp"
#include "button.hpp"
#include "keypad.hpp"

using namespace std;

enter_program::enter_program(centre*set_centre_p, batch_mode_driver* batch_mode_driver_p_s)
:screen(set_centre_p)
{
//  centre_p=set_centre_p;
  batch_mode_driver_p = batch_mode_driver_p_s;
  
//  batch_mode_driver_p -> clear_program();
  
  prompt_p = new QLabel;
  keypad_p = new keypad;
  options_button_p = new button("Options");
  back_button_p = new button("Back");
  edit_p = new QTextEdit;
  help_button_p = new button("Help");
  done_button_p = new button("Done");
  layout_p = new QGridLayout;
  
  program_string_p = new QString("");
  
  layout_p->addWidget(prompt_p, 0, 0);
  layout_p->addWidget(options_button_p, 0, 3);
  layout_p->addWidget(back_button_p, 0, 6);
  layout_p->addWidget(keypad_p, 1, 0);
  layout_p->addWidget(edit_p, 1, 2, 4, 4);
  layout_p->addWidget(help_button_p, 6, 0);
  layout_p->addWidget(done_button_p, 6, 6);
  
  setLayout(layout_p);
  
  connect(options_button_p, SIGNAL(clicked()), this, SLOT(options_clicked()));
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_clicked()));
  connect(keypad_p, SIGNAL(number_entered(int)), this, SLOT(number_entered(int)));
  connect(help_button_p, SIGNAL(clicked()), this, SLOT(help_clicked()));
  connect(done_button_p, SIGNAL(clicked()), this, SLOT(done_clicked()));
  
  enter_flag = 'p';
  prompt_p -> setText("Enter number of packages");
  
  
  cout<<"enter_program::enter_program.  before update\n";
  
  update_program_string();

  cout<<"enter_program::enter_program.  after update\n";

}

enter_program::~enter_program()
{
  
  delete prompt_p;
  delete keypad_p;
  delete options_button_p;
  delete back_button_p;
  delete help_button_p;
  delete done_button_p;
  delete layout_p;
  
  delete program_string_p;
  program_string_p = 0;

}

void enter_program::options_clicked()
{
  centre_p -> add_waiting_screen(14);//come back here to enter_program
  centre_p -> add_waiting_screen(16);//batch_options
  centre_p -> screen_done = true;
}

void enter_program::back_clicked()
{  
  
  cout<<"back clicked\n";
  /*
  if(batch_mode_driver_p->program_is_empty())
  {
    centre_p->add_waiting_screen(centre_p->get_previous_screen());
    
    cout<<"back clicked.  program empty.  add waiting screen "<<centre_p->get_previous_screen()<<endl;
    
    centre_p->screen_done=true;
  }
  */
  if(batch_mode_driver_p->get_program_size() > 0)
  {
    batch_mode_driver_p->program_remove_last();
    update_program_string();
    enter_flag = 'p';
  }
  else
  {
    centre_p->add_waiting_screen(centre_p->get_previous_screen());
    centre_p->screen_done=true;
  }
}

void enter_program::number_entered(int value)
{
  if(enter_flag == 'p')
  {
//    bm_set_p = new bm_set;
//    bm_set_p -> packs = value;
//    bm_set_p -> seeds = 0;
    
    current_packs = value; 
    enter_flag = 's';
    prompt_p -> setText("Enter number of \nseeds per package");
    cout<<"enter_program::number_entered packs\n";
    batch_mode_driver_p->list_program();
    update_program_string();
    return;
  }
  if(enter_flag == 's')
  {
//    bm_set_p -> seeds = value;
//    batch_mode_driver_p->program.append(bm_set_p);
    
    current_seeds = value;
    batch_mode_driver_p->add_line(current_packs, current_seeds);
    enter_flag = 'p';
    prompt_p -> setText("Enter number of packages");
    cout<<"enter_program::number_entered seeds\n";
    batch_mode_driver_p->list_program();
    update_program_string();
    return;
  }
  //should not execute
  cout<<"invalid enter_flag\n";
}

void enter_program::help_clicked()
{
}

void enter_program::done_clicked()
{
  cout<<"enter_program::done_clicked\n";
  if(batch_mode_driver_p->program_is_empty())
  {
    return;
  }
  batch_mode_driver_p -> high_feed_speed = 100;
  batch_mode_driver_p -> low_feed_speed = 10;
  batch_mode_driver_p -> dump_speed = 1000;
  batch_mode_driver_p->list_program();
  batch_mode_driver_p->reset_program();
  cout<<"done reset_program\n";  
  centre_p->add_waiting_screen(15);//batch
  centre_p->screen_done = true;
}

void enter_program::update_program_string()  
{
  cout<<"update_program_string 1\n";
  delete program_string_p;
  cout<<"update_program_string 2\n";
  program_string_p = new QString;
  cout<<"update_program_string 3\n";
  batch_mode_driver_p->list_program();
  for(int i=0; i<batch_mode_driver_p->get_program_size(); ++i)
  {
    cout<<"  i "<<i<<endl;
    QString str;
    str.setNum(batch_mode_driver_p->get_packs(i));
    program_string_p -> append(str);
    program_string_p -> append(" packs, ");
    str.setNum(batch_mode_driver_p->get_seeds(i));
    program_string_p -> append(str);
    program_string_p -> append(" seeds each.\n");
  }
  cout<<"update_program_string 4\n";
  edit_p -> setText(*program_string_p);
}
  
