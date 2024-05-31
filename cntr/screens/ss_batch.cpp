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
#include <fstream>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QTimer>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include "centre.hpp"
#include "ss_batch.hpp"
#include "button.hpp"
#include "message_box.hpp"
#include "help_screen.hpp"

using namespace std;

void ss_barcode_line::keyPressEvent(QKeyEvent* event)
{
  QLineEdit::keyPressEvent(event); //TEST~~~
  if(event->key() == Qt::Key_Return)
  {
    emit barcode_entered(text());  //TEST~~~
    clear();
  }
}

ss_batch::ss_batch(centre* set_centre_p, batch_mode_driver* set_batch_mode_driver_p)
 :screen(set_centre_p)
{
  batch_mode_driver_p = set_batch_mode_driver_p;
  batch_mode_driver_p->chamber_count_limit_calculation(); //2021_03_22//
  
  if (centre_p->crops[0].calibrated == false) 
  {
    centre_p->add_waiting_screen(33);//back here to ss_batch
    centre_p->add_waiting_screen(10);//calibrate
    centre_p->screen_done = true;
    return;
  }
  count_message_p = new QLabel;
  options_button_p = new button("Options");
  back_button_p = new button("Back");
  barcode_line_p = new ss_barcode_line;
  barcode_line_p->setMaximumSize(120,40);  //ORIGINAL~~~
  release_pack_button_p = new button("Release pack");
  restart_button_p = new button("Dump out and\nrestart seed lot");
  substitution_button_p = new button("Substitute seed lot");
  high_speed_label_p = new QLabel("High");
  high_speed_set_p = new QSlider;
  high_speed_set_p->setMinimum(0);
  high_speed_set_p->setMaximum(1000);
  high_speed_set_p->setOrientation(Qt::Horizontal);
  high_speed_set_p->setFocusPolicy(Qt::NoFocus);
  low_speed_label_p = new QLabel("Low");
  low_speed_set_p = new QSlider;
  low_speed_set_p->setMinimum(0);
  low_speed_set_p->setMaximum(100);
  low_speed_set_p->setOrientation(Qt::Horizontal);
  low_speed_set_p->setFocusPolicy(Qt::NoFocus);
  dump_speed_label_p = new QLabel("Dump");
  dump_speed_set_p = new QSlider;
  dump_speed_set_p->setMinimum(0);
  dump_speed_set_p->setMaximum(1000);
  dump_speed_set_p->setOrientation(Qt::Horizontal);
  dump_speed_set_p->setFocusPolicy(Qt::NoFocus);
  
  //calculate speed slider positions from speed values.  This is needed because the speed values are piecewise linear functions of the slider positions.
  
  int high_speed_value = batch_mode_driver_p->high_feed_speed;
  int high_speed_position;
  if(high_speed_value<=250) high_speed_position = 2*high_speed_value;
  else high_speed_position = (high_speed_value+500) / 1.5;
  
  int low_speed_value = batch_mode_driver_p->low_feed_speed;
  int low_speed_position;
  if(low_speed_value<=250) low_speed_position = 2*low_speed_value;
  else low_speed_position = (low_speed_value+500) / 1.5;
  
  int dump_speed_value = batch_mode_driver_p->dump_speed;
  int dump_speed_position;
  if(dump_speed_value<=250) dump_speed_position = 2*dump_speed_value;
  else dump_speed_position = (dump_speed_value+500) / 1.5;
  
  high_speed_set_p->setValue(high_speed_position);
  low_speed_set_p->setValue(low_speed_position);
  dump_speed_set_p->setValue(dump_speed_position);
  
  ss_table_p = new ss_batch_table(centre_p, batch_mode_driver_p);
  status_box_p = new message_box;
  status_box_p -> setMinimumSize(250,100);
  help_button_p = new button("Help");
  save_program_button_p = new button("Save Spreadsheet\nSetup");
  
//  reprint_button_1_p = new button("");
//  reprint_button_2_p = new button("");
  manual_print_button_p = new button("Manual\noperation");
  
  quit_button_p = new button("Quit");
  barcode_status_p = new message_box;
  
  top_box_p = new QGroupBox;
  control_box_p = new QGroupBox;
  speed_box_p = new QGroupBox;
  speed_box_p -> setTitle("Speeds");
  speed_box_p -> setMinimumWidth(300);
  bottom_box_p = new QGroupBox;
    
  top_layout_p = new QGridLayout;
  control_layout_p = new QGridLayout;
  speed_layout_p = new QGridLayout;
  bottom_layout_p = new QGridLayout;  
  main_layout_p = new QGridLayout;

  top_layout_p -> addWidget(count_message_p, 0, 0);
  top_layout_p -> addWidget(options_button_p, 0, 1);
  top_layout_p -> addWidget(back_button_p, 0, 3);
  bottom_layout_p -> addWidget(barcode_line_p, 1, 4);  
  barcode_line_p->setFocus();
  barcode_line_p -> setText("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");  
  barcode_line_p -> setText("                                                             ");  
  control_layout_p -> addWidget(release_pack_button_p, 0, 0);
  control_layout_p -> addWidget(restart_button_p, 0, 1);
  control_layout_p -> addWidget(substitution_button_p, 1, 0);
  control_layout_p -> addWidget(speed_box_p, 2, 0, 1, 2);     
  speed_layout_p -> addWidget(high_speed_label_p, 0, 0);
  speed_layout_p -> addWidget(high_speed_set_p, 0, 1);
  speed_layout_p -> addWidget(low_speed_label_p, 1, 0);
  speed_layout_p -> addWidget(low_speed_set_p, 1, 1);
  speed_layout_p -> addWidget(dump_speed_label_p, 2, 0);
  speed_layout_p -> addWidget(dump_speed_set_p, 2, 1);
  bottom_layout_p -> addWidget(status_box_p, 0, 0, 2, 1);
  bottom_layout_p -> addWidget(help_button_p, 0, 1);
  bottom_layout_p -> addWidget(save_program_button_p, 0, 2);
  bottom_layout_p -> addWidget(manual_print_button_p, 0, 3);
  bottom_layout_p -> addWidget(quit_button_p, 0, 4);
  bottom_layout_p -> addWidget(barcode_status_p, 1, 1, 1, 4);
  main_layout_p -> addWidget(top_box_p, 0, 0, 1, 2);
  main_layout_p -> addWidget(control_box_p, 1, 0);
  main_layout_p -> addWidget(ss_table_p, 1, 1);
  main_layout_p -> addWidget(bottom_box_p, 2, 0, 1, 2);
  
  top_box_p -> setLayout(top_layout_p);
  control_box_p -> setLayout(control_layout_p);
  speed_box_p -> setLayout(speed_layout_p);
  bottom_box_p -> setLayout(bottom_layout_p);
  setLayout(main_layout_p);
  
  connect(options_button_p, SIGNAL(clicked()), this, SLOT(options_clicked()));
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_clicked()));
  connect(release_pack_button_p, SIGNAL(clicked()), this, SLOT(release_pack_clicked()));
  connect(restart_button_p, SIGNAL(clicked()), this, SLOT(restart_clicked()));
  connect(substitution_button_p, SIGNAL(clicked()), this, SLOT(substitution_button_clicked()));
  connect(high_speed_set_p, SIGNAL(valueChanged(int)), batch_mode_driver_p, SLOT(set_high_feed_speed(int)));
  connect(low_speed_set_p, SIGNAL(valueChanged(int)), batch_mode_driver_p, SLOT(set_low_feed_speed(int)));
  connect(dump_speed_set_p, SIGNAL(valueChanged(int)), batch_mode_driver_p, SLOT(set_dump_feed_speed(int)));
  connect(help_button_p, SIGNAL(clicked()), this, SLOT(help_button_clicked()));
  connect(save_program_button_p, SIGNAL(clicked()), this, SLOT(save_program_clicked()));
  connect(manual_print_button_p, SIGNAL(clicked()), this, SLOT(manual_print_button_clicked()));
  connect(quit_button_p, SIGNAL(clicked()), this, SLOT(quit_clicked()));
  connect(barcode_line_p, SIGNAL(barcode_entered(QString)), batch_mode_driver_p, SLOT(barcode_entered(QString)));
  connect(low_speed_set_p, SIGNAL(sliderReleased()), this, SLOT(focus_on_barcode()));
  connect(batch_mode_driver_p, SIGNAL(send_status_message(QString, QColor, QColor, int)), this, SLOT(get_status_message(QString, QColor, QColor, int)));
  connect(batch_mode_driver_p, SIGNAL(send_barcode_status_message(QString, QColor, QColor, int)), this, SLOT(get_barcode_status_message(QString, QColor, QColor, int)));

  centre_p->set_endgate_state(ENDGATE_CLOSED);
  if(centre_p->totalize_force_endgate_open == true)
  {
  }
  else
  {
  }
  
  feeder_is_running = true;
  
  run_timer_p = new QTimer;
  connect(run_timer_p, SIGNAL(timeout()), this, SLOT(run()));
  run_timer_p->start(100);
  
  centre_p->count = 0;
  batch_mode_driver_p->start();
  
  connect(batch_mode_driver_p, SIGNAL(pack_collected(int)), this, SLOT(pack_collected(int)));
  connect(batch_mode_driver_p, SIGNAL(dump_complete(int)), this, SLOT(dump_complete(int)));
  connect(ss_table_p, SIGNAL(focus_on_barcode()), this, SLOT(focus_on_barcode()));
  
  connect(batch_mode_driver_p, SIGNAL(refresh_screen()), this, SLOT(refresh_screen()));
  connect(batch_mode_driver_p, SIGNAL(enable_substitute_button(bool)), this, SLOT(enable_substitute_button(bool)));
    
  if(batch_mode_driver_p->bm_save_ss_setup_filename != "")//returning from batch_save_ss_setup screen with name of file to save
  {
    batch_mode_driver_p->save_ss_setup(batch_mode_driver_p->bm_save_ss_setup_filename);
    batch_mode_driver_p->bm_save_ss_setup_filename = "";
  }
  
  batch_mode_driver_p ->fill_ss_column_pointers();
  ss_table_p -> refresh();
   
  top_layout_p->setContentsMargins(0,0,0,0);        //set layout margins to shrink to designated container dimensions//
  control_layout_p->setContentsMargins(0,0,0,0);
  speed_layout_p->setContentsMargins(0,0,0,0);
  bottom_layout_p->setContentsMargins(0,0,0,0);
  main_layout_p->setContentsMargins(0,0,10,10);
  
  main_layout_p->setRowMinimumHeight(5, 0);
  main_layout_p->setRowStretch(5, 0);
  
  manual_operation_window_p = 0;
  manual_operation_window_created = false;

  batch_mode_driver_p->require_pack_barcode = true;
  batch_mode_driver_p->require_seed_lot_barcode = true;
  batch_mode_driver_p->pack_match_lot = false;
  batch_mode_driver_p->pack_contain_lot = false;
  batch_mode_driver_p->lot_contain_pack = false;
  batch_mode_driver_p->pack_match_spreadsheet = true;
  batch_mode_driver_p->record_only = false;
  batch_mode_driver_p -> use_spreadsheet = true;
  batch_mode_driver_p -> seed_lot_barcode = "";
  batch_mode_driver_p -> pack_barcode = "";

  if(batch_mode_driver_p->print_envelope)
  {
    batch_mode_driver_p->require_pack_barcode = false;
    batch_mode_driver_p->pack_match_lot = false;
    batch_mode_driver_p->pack_contain_lot = false;
    batch_mode_driver_p->lot_contain_pack = false;
    batch_mode_driver_p->pack_match_spreadsheet = false;
    batch_mode_driver_p->record_only = true;
    batch_mode_driver_p->pack_barcode_ok = true;
  }

  count_message_p->setStyleSheet("QLabel {" 
        "background-color: white;"          
        "border: 3px solid black;"          
        "font-size: 20pt;}");   

  eject_memory_sticks = false;
  
  barcode_line_p->setFocus();
  batch_mode_driver_p->mode = entry;//wait_for_seed_lot_barcode;
  batch_mode_driver_p->seed_lot_barcode_ok = false;
  batch_mode_driver_p -> dump_automatically = true;
}

ss_batch::~ss_batch()
{
  run_timer_p -> stop();
  while(1)
  {
    int val = batch_mode_driver_p->save_spreadsheet(batch_mode_driver_p->spreadsheet_filename);
    int ret;
    if(val==0)
    {
      QMessageBox box;
      box.setText("Unable to save spreadsheet");
      box.setStandardButtons(QMessageBox::Retry | QMessageBox::Ok);
      box.setDefaultButton(QMessageBox::Retry);
      ret = box.exec();
    }
    if( (val!=0) || (ret==QMessageBox::Ok) ) break;
  }
  
  if(eject_memory_sticks == true)
  {
    QDir dir("/media/odroid/");
    QStringList directory_list = dir.entryList(QDir::Dirs  | QDir::NoDotAndDotDot);
    int num_drives = directory_list.size();//number of USB memory sticks detected
    for (int i=0; i<num_drives; ++i)
    {
      QString command = "umount /media/odroid/";
      command.append(directory_list[i]);
      char* command_a = command.toLatin1().data();
      if(system (command_a) != 0)
      {
        cout<<"unmount command failed\n";
      }
    }
    QDir check_dir("/media/odroid/");
    QStringList check_list = check_dir.entryList(QDir::Dirs  | QDir::NoDotAndDotDot);
    if(check_list.size() == 0) 
    {
      QMessageBox box;
      box.setText(QString("%1 USB stick(s) can be safely removed").arg(num_drives));
      box.exec();
    }
    else
    {
      QMessageBox box;
      box.setText(QString("WARNING - %1 USB stick(s) were NOT unmounted.  Shut down machine to remove them safely.").arg(check_list.size()));
      box.exec();
    }
  }
  
  batch_mode_driver_p -> stop();
  delete count_message_p;
  delete options_button_p;
  delete back_button_p;
  delete restart_button_p;
  delete high_speed_label_p;
  delete high_speed_set_p;
  delete low_speed_label_p;
  delete low_speed_set_p;
  delete dump_speed_label_p;
  delete dump_speed_set_p;
  delete ss_table_p;
  delete status_box_p;
  delete quit_button_p;
  delete top_layout_p;
  delete top_box_p;
  delete speed_box_p;
  delete bottom_box_p;
  delete control_layout_p;
  delete main_layout_p;
  
  if(manual_operation_window_created) delete manual_operation_window_p;
  manual_operation_window_created = false;
  manual_operation_window_p = 0;
}

void ss_batch::get_status_message(QString message, QColor foreground, QColor background, int text_size)
{
  status_box_p->set_text_size(text_size);
  status_box_p->set_foreground(foreground);
  status_box_p->set_background(background);
  status_box_p->set_text(message);
}

void ss_batch::get_barcode_status_message(QString message, QColor foreground, QColor background, int text_size)
{
  barcode_status_p->set_text_size(text_size);
  barcode_status_p->set_foreground(foreground);
  barcode_status_p->set_background(background);
  barcode_status_p->set_text(message);
}

void ss_batch::pack_collected(int count_to_record)
{
  centre_p->pack_count_str = QString::number(count_to_record);
  
  if(batch_mode_driver_p->substitute_seed_lot==false)
  {
    centre_p -> communicate_out('p');
  }
  else//substituting seed lot
  {
    centre_p -> communicate_out('s');
  }
}  

void ss_batch::dump_complete(int dump_count)
{
  centre_p->dump_count_str = QString::number(dump_count);
  centre_p->communicate_out('d');
}

void ss_batch::focus_on_barcode()
{
  barcode_line_p -> setFocus();
}

void ss_batch::enable_substitute_button(bool on)
{
  substitution_button_p->setEnabled(on);
}

void ss_batch::refresh_screen()
{
  int temp_line_number = batch_mode_driver_p->spreadsheet_line_number;
  if(temp_line_number>0) ss_table_p -> set_current_row(temp_line_number);
  ss_table_p -> refresh();
}  

void ss_batch::options_clicked()
{
  batch_mode_driver_p -> stop();
  centre_p->set_speed(0);
  centre_p->add_waiting_screen(33);//come back to ss_batch
  centre_p->add_waiting_screen(31);//ss_options
  centre_p->screen_done=true;
}

void ss_batch::back_clicked()
{
  batch_mode_driver_p -> stop();
  centre_p->set_speed(0);
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void ss_batch::release_pack_clicked()
{
  batch_mode_driver_p->ss_first_column_p->data_list[batch_mode_driver_p->end_valve_spreadsheet_line_number] = "R";
  if(batch_mode_driver_p->ss_setup_p->fill_time_column >= 0)// -1 value signals not to record
  {
    batch_mode_driver_p->ss_fill_time_p->data_list[batch_mode_driver_p->end_valve_spreadsheet_line_number] = "";
  }
  if(batch_mode_driver_p->ss_setup_p->actual_count_column >= 0)// -1 value signals not to record
  {
    batch_mode_driver_p->ss_actual_count_p->data_list[batch_mode_driver_p->end_valve_spreadsheet_line_number] = "";
  }
  batch_mode_driver_p->release_pack = true;//true signals to release counted seed, even if barcode matching not satisfied.  For use in case of lost packet.
  get_barcode_status_message("Release pack chosen",  QColor(0,0,0), QColor(255,255,0), 25);
  focus_on_barcode();
}

void ss_batch::restart_clicked()
{
  QMessageBox box;
  box.setText("This will dump out the current seed lot and mark all packs from it unfilled.");
  box.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
  int ret = box.exec();
  if(ret == QMessageBox::Ok)
  {
    batch_mode_driver_p -> restart();
  }
  focus_on_barcode();
}

void ss_batch::substitution_button_clicked()
{
  batch_mode_driver_p -> seed_lot_substitution();
  focus_on_barcode();
}

void ss_batch::help_button_clicked()
{
  help_screen_p = new help_screen;
  help_screen_p -> setGeometry(geometry());
  help_screen_p -> set_text(
    "This is the screen for handling spreadsheets.  "
    "It fills packets from a spreadsheet, which "
    "should be in .csv format on a USB memory stick plugged into the machine.  "
    "Each row of the spreadsheet represents one pack, except the top row, "
    "which should be column headings.  "
    "There must be columns to fill the following roles: a column of material identification codes, "
    "a column of numbers which are the required number of seeds for the packet, "
    "and a column of unique identifiers for each packet.  "
    "You can choose columns to fill these roles, so order is not important.  "
    "You can also choose columns for other optional roles."
    "\n\n"
    "In operation, you scan a material identification code from your seed lot.  "
    "The program picks all rows from your spreadsheet with that code for filling.  "
    "After all packets are filled, remaining seed dumps automatically.  "
    "\n\n"
    "You may need to set up the feed hopper for your seed.  "
    "Instructions are given in the help screen accessible from the startup screen.  "
    "\n\n"
    "The \"Options\" button allows you to set up barcode checking and communications.  "
    "\n\n"
    "\"Release Pack\" allows seed in the end gate to be released even if the barcode on the pack does not scan properly.  "
    "If chosen, that will be recorded in the spreadsheet with an 'R' in the first column.  "
    "The row will be displayed with a yellow background.  "
    "\n\n"
    "The \"Substitute seed lot\" button is for use if you run out of seed of a particular lot, and need to substitute a different lot.  "
    "The substitution will be recorded, and you can also control how this is communicated to another computer.  "
    "\n\n"
    "You can control the speed of counting.  "
    "High speed is the speed the turntable will run at when a pack starts.  "
    "As the pack nears the end, the turntable slows down, with the low speed being the minimum.  "
    "This is to increase the accuracy of the end point where the cut gate closes.  "
    "In general, the lower the speed, the more accurate the count.  "
    "\n\n"
    "Excessive speed may cause problems.  "
    "The cut valve may not have time to open fully, resulting in a \"cut gate timing error\".  "
    "Or the seed may come down so fast that the turntable cannot stop before there is already too much seed in the cut gate chamber.  "
    "The machine will detect both of these problems and advise you to reduce the speed.  "
    "\n\n"
    "Dump speed is the speed the turntable will use while dumping.  "
    "While the machine can count at very high speeds, you may wish to reduce the dump speed if you need an accurate dump count.  "
    "You can check how accurate the dump count is by re-counting the dumped seed in totalize mode, using a low speed setting.  "
    "");
  help_screen_p->show();
}    
    
void ss_batch::save_program_clicked()
{
  batch_mode_driver_p -> stop();
  centre_p->set_speed(0);
  centre_p->add_waiting_screen(29);//batch_save_ss_setup
  centre_p->screen_done = true;
}

void ss_batch::quit_clicked()
{
  batch_mode_driver_p -> stop();
  centre_p->set_speed(0);
  centre_p->add_waiting_screen(0);
  centre_p->screen_done = true;
}

void ss_batch::run()
{
  QString message=QString("Count: %1").arg(centre_p->count);
  count_message_p->setText(message);
  centre_p->processor_display_blobs(false);
  
  int high_speed_slider_position, low_speed_slider_position, dump_speed_slider_position;
  
  if(high_speed_set_p->sliderPosition() <= 500) high_speed_slider_position = (high_speed_set_p->sliderPosition())/2;      //piece-wise linear, 2 sections//
  else high_speed_slider_position = 1.5*(high_speed_set_p->sliderPosition())-500;
  if(low_speed_set_p->sliderPosition() <= 50) low_speed_slider_position = (low_speed_set_p->sliderPosition())/2;          //piece-wise linear, 2 sections//
  else low_speed_slider_position = 1.5*(low_speed_set_p->sliderPosition())-50;
  if(dump_speed_set_p->sliderPosition() <= 500) dump_speed_slider_position = (dump_speed_set_p->sliderPosition())/2;      //piece-wise linear, 2 sections//
  else dump_speed_slider_position = 1.5*(dump_speed_set_p->sliderPosition())-500;

  QString speed_label_string = (QString("Speeds:        H: %1%   L: %2%   D: %3%")
    .arg(1.0*high_speed_slider_position/10)
    .arg(1.0*low_speed_slider_position/10)
    .arg(1.0*dump_speed_slider_position/10));
  speed_box_p -> setTitle(speed_label_string);  
  
  count = centre_p->count;
}

void ss_batch::manual_print_button_clicked()
{
  manual_operation_window_p = new manual_operation_window(0, batch_mode_driver_p, this);
  manual_operation_window_p -> show();
  manual_operation_window_created = true;
  barcode_line_p -> setFocus();
}
  
manual_operation_window::manual_operation_window(QWidget* parent, batch_mode_driver* batch_mode_driver_p_s, ss_batch* ss_batch_p_s)
:QWidget(parent)
{
  batch_mode_driver_p = batch_mode_driver_p_s;
  ss_batch_p = ss_batch_p_s;
  setGeometry(0,0,100,400);	
  
  layout_p = new QGridLayout;
  previous_p = new button("Previous line");
  repeat_line_1_p = new button("");
  repeat_line_2_p = new button("");
  repeat_line_3_p = new button("");
  repeat_line_4_p = new button("");
  repeat_line_5_p = new button("");
  next_p = new button("Next line");
  close_p = new button("Close");
  
  layout_p -> addWidget(previous_p,0,0);
  layout_p -> addWidget(repeat_line_1_p,1,0);
  layout_p -> addWidget(repeat_line_2_p,2,0);
  layout_p -> addWidget(repeat_line_3_p,3,0);
  layout_p -> addWidget(repeat_line_4_p,4,0);
  layout_p -> addWidget(repeat_line_5_p,5,0);
  layout_p -> addWidget(next_p,6,0);
  layout_p -> addWidget(close_p,7,0);
  setLayout(layout_p);
  
  connect(previous_p, SIGNAL(clicked()), this, SLOT(previous_clicked()));
  connect(repeat_line_1_p, SIGNAL(clicked()), this, SLOT(repeat_line_1_clicked()));
  connect(repeat_line_2_p, SIGNAL(clicked()), this, SLOT(repeat_line_2_clicked()));
  connect(repeat_line_3_p, SIGNAL(clicked()), this, SLOT(repeat_line_3_clicked()));
  connect(repeat_line_4_p, SIGNAL(clicked()), this, SLOT(repeat_line_4_clicked()));
  connect(repeat_line_5_p, SIGNAL(clicked()), this, SLOT(repeat_line_5_clicked()));
  connect(next_p, SIGNAL(clicked()), this, SLOT(next_clicked()));
  connect(close_p, SIGNAL(clicked()), this, SLOT(deleteLater()));
  connect(close_p, SIGNAL(clicked()), this, SLOT(close_clicked()));
  
  first_line = batch_mode_driver_p->spreadsheet_line_number-5;
  update_buttons();//will check first_line and may change it
  
  ss_batch_p -> manual_operation_window_created = true;
  
  setWindowFlags(Qt::WindowStaysOnTopHint);
}

void manual_operation_window::update_buttons()
{
  int max_first_line = batch_mode_driver_p->ss_envelope_id_p->data_list.size()-5;
  if(first_line>max_first_line) first_line = max_first_line;
  if(first_line<0) first_line = 0;
  if(first_line>=0)
  {
    repeat_line_1_p -> setText(QString("Repeat ") + batch_mode_driver_p->ss_envelope_id_p->data_list[first_line]); 
  }
  else
  {
    repeat_line_1_p -> hide();
  }
  
  if(first_line+1>=0)
  {
    repeat_line_2_p -> setText(QString("Repeat ") + batch_mode_driver_p->ss_envelope_id_p->data_list[first_line+1]); 
  }
  
  if(first_line+2>=0)
  {
    repeat_line_3_p -> setText(QString("Repeat ") + batch_mode_driver_p->ss_envelope_id_p->data_list[first_line+2]); 
  }
  
  if(first_line+3>=0)
  {
    repeat_line_4_p -> setText(QString("Repeat ") + batch_mode_driver_p->ss_envelope_id_p->data_list[first_line+3]); 
  }
  
  if(first_line+4>=0)
  {
    repeat_line_5_p -> setText(QString("Repeat ") + batch_mode_driver_p->ss_envelope_id_p->data_list[first_line+4]); 
  }
  
}

void manual_operation_window::previous_clicked()
{
  --first_line;
  update_buttons();//will check first_line and may change it
}

void manual_operation_window::repeat_line_1_clicked()
{
  if(first_line<0) return;
  batch_mode_driver_p->ss_first_column_p->data_list[first_line] = "N";
  ss_batch_p->refresh_screen();
}

void manual_operation_window::repeat_line_2_clicked()
{
  if(first_line+1<0) return;
  batch_mode_driver_p->ss_first_column_p->data_list[first_line+1] = "N";
  ss_batch_p->refresh_screen();
}

void manual_operation_window::repeat_line_3_clicked()
{
  if(first_line+2<0) return;
  batch_mode_driver_p->ss_first_column_p->data_list[first_line+2] = "N";
  ss_batch_p->refresh_screen();
}

void manual_operation_window::repeat_line_4_clicked()
{
  if(first_line+3<0) return;
  batch_mode_driver_p->ss_first_column_p->data_list[first_line+3] = "N";
  ss_batch_p->refresh_screen();
}

void manual_operation_window::repeat_line_5_clicked()
{
  if(first_line+4<0) return;
  batch_mode_driver_p->ss_first_column_p->data_list[first_line+4] = "N";
  ss_batch_p->refresh_screen();
}

void manual_operation_window::next_clicked()
{
  ++first_line;
  update_buttons();//will check first_line and may change it
}

void manual_operation_window::close_clicked()//note - close button is connected to both this and delete later slot.  this function is to prevent ss_batch from deleting this window if already deleted
{
  ss_batch_p -> manual_operation_window_created = false;
}

