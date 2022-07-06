#include <iostream>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDir>
#include "centre.hpp"
#include "ss_setup_entry.hpp"
#include "button.hpp"
#include "batch_mode_driver.hpp"

using namespace std;

ss_setup_entry::ss_setup_entry(centre*set_centre_p, batch_mode_driver* batch_mode_driver_p_s)
:screen(set_centre_p)
{
  centre_p=set_centre_p;
  batch_mode_driver_p = batch_mode_driver_p_s;


//  cout<<"ss_setup_entry::ss_setup_entry\n";
//  batch_mode_driver_p->list_ss_setup();

  choice1_p=new button("");
  choice2_p=new button("");
  choice3_p=new button("");
  choice4_p=new button("");
  choice5_p=new button("");
  choice6_p=new button("");
  next_headings_p=new button("More headings");
  previous_headings_p=new button("Previous headings");
  none_p=new button("None");
  back_p=new button("Back");
  message_p=new QLabel("");

  top_box_p=new QGroupBox;
  middle_box_p=new QGroupBox;
  bottom_box_p=new QGroupBox;
  
  top_layout_p=new QGridLayout;
  middle_layout_p=new QGridLayout;
  bottom_layout_p=new QGridLayout;
  
  main_layout_p=new QVBoxLayout;
  
  top_layout_p->addWidget(message_p,0,0,2,1);
  top_layout_p->addWidget(back_p, 0, 1);
  top_layout_p->addWidget(none_p, 1, 1);
  top_box_p->setLayout(top_layout_p);
  
  middle_layout_p->addWidget(choice1_p,0,0);
  middle_layout_p->addWidget(choice2_p,0,1);
  middle_layout_p->addWidget(choice3_p,1,0);
  middle_layout_p->addWidget(choice4_p,1,1);
  middle_layout_p->addWidget(choice5_p,2,0);
  middle_layout_p->addWidget(choice6_p,2,1);
  middle_box_p->setLayout(middle_layout_p);
  
  bottom_layout_p->addWidget(next_headings_p,0,1);
  bottom_layout_p->addWidget(previous_headings_p,0,0);
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
  connect(next_headings_p, SIGNAL(clicked()), this, SLOT(next_headings_clicked()));
  connect(previous_headings_p, SIGNAL(clicked()), this, SLOT(previous_headings_clicked()));
  connect(none_p, SIGNAL(clicked()), this, SLOT(none_clicked()));
  connect(back_p, SIGNAL(clicked()), this, SLOT(back_clicked()));
  
  QString crop_name = centre_p->crops[0].name;
  ss_setup_path = "ss_setups/";
  ss_setup_path.append(crop_name);
  ss_setup_path.append("/");
  dir_p = new QDir(ss_setup_path);
  first_heading_displayed = 0;
  mode = 'm';
  display_headings();
}

ss_setup_entry::~ss_setup_entry()
{
  delete dir_p;
  dir_p = 0;
}

void ss_setup_entry::choice1_clicked()
{
  enter_choice(first_heading_displayed);
}

void ss_setup_entry::choice2_clicked()
{
  enter_choice(first_heading_displayed+1);
}

void ss_setup_entry::choice3_clicked()
{
  enter_choice(first_heading_displayed+2);
}

void ss_setup_entry::choice4_clicked()
{
  enter_choice(first_heading_displayed+3);
}

void ss_setup_entry::choice5_clicked()
{
  enter_choice(first_heading_displayed+4);
}

void ss_setup_entry::choice6_clicked()
{
  enter_choice(first_heading_displayed+5);
}

void ss_setup_entry::enter_choice(int column)
{
  if(mode=='m')
  {
    batch_mode_driver_p -> ss_setup_p -> material_id_column = column;
    batch_mode_driver_p -> display_column_numbers.append(column);
    mode = 'r';
  }
  else if(mode=='r')
  {
    batch_mode_driver_p -> ss_setup_p -> required_count_column = column;
    batch_mode_driver_p -> display_column_numbers.append(column);
    mode = 'e';
  }
  else if(mode=='e')
  {
    batch_mode_driver_p -> ss_setup_p -> envelope_id_column = column;
    batch_mode_driver_p -> display_column_numbers.append(column);
    
    /*
    mode = 'a';
  }
  else if(mode=='a')
  {
    batch_mode_driver_p -> ss_setup_p -> actual_count_column = column;
    */
    
    
    mode = 'p';
  }
  else if(mode=='p')
  {
    batch_mode_driver_p -> ss_setup_p -> print_time_column = column;
    mode = 'f';
  }
  else if(mode=='f')
  {
    batch_mode_driver_p -> ss_setup_p -> fill_time_column = column;
    mode = 'd';
  }
  else if(mode=='d')
  {
    batch_mode_driver_p -> ss_setup_p -> dump_count_column = column;
    mode = 's';
  }
  else if(mode=='s')
  {
    batch_mode_driver_p -> ss_setup_p -> substitution_column = column;
    centre_p -> add_waiting_screen(31);//ss_options
    centre_p -> screen_done = true;
  }
  display_headings();
}

void ss_setup_entry::next_headings_clicked()
{
  first_heading_displayed += 6;
  display_headings();
}

void ss_setup_entry::previous_headings_clicked()
{
  first_heading_displayed -= 6;
  if(first_heading_displayed<0) first_heading_displayed = 0;
  display_headings();
}

void ss_setup_entry::none_clicked()
{
  enter_choice(-1);//indicates invalid column
}

void ss_setup_entry::back_clicked()
{
  if(mode=='m')
  {
    centre_p->add_waiting_screen(centre_p->get_previous_screen());
    centre_p->screen_done=true;
  }
  if(mode=='r')
  {
    mode='m';
  }
  if(mode=='e')
  {
    mode='r';
  }
  if(mode=='a')
  {
    mode='e';
  }
  if(mode=='p')
  {
    //mode='a';
    mode = 'e';
  }
  if(mode=='f')
  {
    mode='p';
  }
  if(mode=='d')
  {
    mode='f';
  }
  if(mode=='s')
  {
    mode='d';
  }
  display_headings();
}

void ss_setup_entry::display_headings()
{
  none_p->show();
  if(mode=='m') 
  {
    message_p->setText(" \
Pick material id. column.\n\n \
This column should contain the\n \
bar code of the seed lot to be\n \
used for each packet.");
    
    none_p -> hide();
  }
  if(mode=='r') 
  {
    message_p->setText(" \
Pick required count column.\n\n \
This column should contain the\n \
number of seeds for each packet.");  
  
    none_p -> hide();
  }
  if(mode=='e') 
  {
    message_p->setText(" \
Pick envelope id. column. \n\n \
Should contain a unique identifier for \n \
each packet.  If printed on the envelope in \n \
barcode, the barcode can be checked against \n \
this column.");  

    none_p -> hide();
  }
  if(mode=='a') message_p->setText(" \
Pick actual count column (optional)\n\n \
If selected, the number of seeds counted will \n \
be recorded here.  This could differ from the \n \
required count if seed is short.");
  if(mode=='p') message_p->setText(" \
Pick print time column (optional).\n\n \
If selected, time of printing packet \n \
will be recorded here. ");
  if(mode=='f') message_p->setText(" \
Pick fill time column (optional).\n\n \
If selected, time of filling packet \n \
will be recorded here. ");
  if(mode=='d') message_p->setText(" \
Pick dump count column (optional).\n\n \
If selected, number of seeds dumped \n \
will be recorded here, beside last \n \
packet for seed lot.");
  if(mode=='s') message_p->setText(" \
Pick substitution column (optional).\n\n \
If selected, and a barcode for a \n \
substitute seed lot is scanned, that \n \
barcode will be entered in this column.");
  
  spreadsheet_column* column_p;

  column_p = batch_mode_driver_p->get_spreadsheet_column_pointer(first_heading_displayed);
  if(column_p==0) choice1_p->setText("");
  else choice1_p->setText(column_p->heading);

  column_p = batch_mode_driver_p->get_spreadsheet_column_pointer(first_heading_displayed+1);
  if(column_p==0) choice2_p->setText("");
  else choice2_p->setText(column_p->heading);

  column_p = batch_mode_driver_p->get_spreadsheet_column_pointer(first_heading_displayed+2);
  if(column_p==0) choice3_p->setText("");
  else choice3_p->setText(column_p->heading);

  column_p = batch_mode_driver_p->get_spreadsheet_column_pointer(first_heading_displayed+3);
  if(column_p==0) choice4_p->setText("");
  else choice4_p->setText(column_p->heading);

  column_p = batch_mode_driver_p->get_spreadsheet_column_pointer(first_heading_displayed+4);
  if(column_p==0) choice5_p->setText("");
  else choice5_p->setText(column_p->heading);

  column_p = batch_mode_driver_p->get_spreadsheet_column_pointer(first_heading_displayed+5);
  if(column_p==0) choice6_p->setText("");
  else choice6_p->setText(column_p->heading);

//  cout<<"end ss_setup_entry::display_headings.  mode="<<mode<<endl;
}
