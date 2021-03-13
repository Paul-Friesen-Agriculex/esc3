#include <iostream>
#include <fstream>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QTimer>
#include <QKeyEvent>
#include <QFileDialog>
#include "centre.hpp"
#include "ss_batch.hpp"
#include "button.hpp"
#include "message_box.hpp"


using namespace std;

void ss_barcode_line::keyPressEvent(QKeyEvent* event)
{
  QLineEdit::keyPressEvent(event); //TEST~~~
  if(event->key() == Qt::Key_Return)
  {
//    emit barcode_entered(displayText());  //TEST~~~
    emit barcode_entered(text());  //TEST~~~
    clear();
  }
}

ss_batch::ss_batch(centre* set_centre_p, batch_mode_driver* set_batch_mode_driver_p)
 :screen(set_centre_p)
{
  batch_mode_driver_p = set_batch_mode_driver_p;

  if (centre_p->crops[0].calibrated == false) 
  {
    centre_p->add_waiting_screen(33);//back here to ss_batch
    centre_p->add_waiting_screen(10);//calibrate
    centre_p->screen_done = true;
    return;
  }
  count_message_p = new QLabel;
  options_button_p = new button("Options");
  remove_drive_button_p = new button("Safely remove\nUSB stick(s)");
  back_button_p = new button("Back");
  barcode_line_p = new ss_barcode_line;
  barcode_line_p->setMaximumSize(120,40);  //ORIGINAL~~~
  release_pack_button_p = new button("Release pack");
  restart_button_p = new button("Dump out and\nrestart seed lot");
  substitution_button_p = new button("Substitute seed lot");
  cancel_substitution_button_p = new button("Cancel substitute\nseed lot");
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
  
  
  
  
  
  
//  high_speed_set_p->setValue(batch_mode_driver_p->high_feed_speed);
//  low_speed_set_p->setValue(batch_mode_driver_p->low_feed_speed);
//  dump_speed_set_p->setValue(batch_mode_driver_p->dump_speed);
  
  ss_table_p = new ss_batch_table(centre_p, batch_mode_driver_p);
  status_box_p = new message_box;
  status_box_p -> setMinimumSize(250,100);
  save_program_button_p = new button("Save Spreadsheet\nSetup");
  
  reprint_button_1_p = new button("");
  reprint_button_2_p = new button("");
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
  top_layout_p -> addWidget(remove_drive_button_p, 0, 2);
  top_layout_p -> addWidget(back_button_p, 0, 3);
  bottom_layout_p -> addWidget(barcode_line_p, 1, 4);  
  barcode_line_p->setFocus();
  barcode_line_p -> setText("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");  
//  barcode_line_p -> setText("10XXxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");  
  barcode_line_p -> setText("                                                             ");  
//  barcode_line_p -> clear();  
  control_layout_p -> addWidget(release_pack_button_p, 0, 0);
  control_layout_p -> addWidget(restart_button_p, 0, 1);
  control_layout_p -> addWidget(substitution_button_p, 1, 0);
  control_layout_p -> addWidget(cancel_substitution_button_p, 1, 1);
  control_layout_p -> addWidget(speed_box_p, 2, 0, 1, 2);     
  speed_layout_p -> addWidget(high_speed_label_p, 0, 0);
  speed_layout_p -> addWidget(high_speed_set_p, 0, 1);
  speed_layout_p -> addWidget(low_speed_label_p, 1, 0);
  speed_layout_p -> addWidget(low_speed_set_p, 1, 1);
  speed_layout_p -> addWidget(dump_speed_label_p, 2, 0);
  speed_layout_p -> addWidget(dump_speed_set_p, 2, 1);
  bottom_layout_p -> addWidget(status_box_p, 0, 0, 2, 1);
  bottom_layout_p -> addWidget(save_program_button_p, 0, 2);
  bottom_layout_p -> addWidget(reprint_button_1_p, 0, 3);
  bottom_layout_p -> addWidget(reprint_button_2_p, 0, 4);
  bottom_layout_p -> addWidget(manual_print_button_p, 0, 5);
  bottom_layout_p -> addWidget(quit_button_p, 0, 6);
  bottom_layout_p -> addWidget(barcode_status_p, 1, 2, 1, 4);
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
  connect(remove_drive_button_p, SIGNAL(clicked()), this, SLOT(remove_drive_clicked()));
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_clicked()));
  connect(release_pack_button_p, SIGNAL(clicked()), this, SLOT(release_pack_clicked()));
  connect(restart_button_p, SIGNAL(clicked()), this, SLOT(restart_clicked()));
  connect(substitution_button_p, SIGNAL(clicked()), this, SLOT(substitution_button_clicked()));
  connect(cancel_substitution_button_p, SIGNAL(clicked()), this, SLOT(cancel_substitution_button_clicked()));
  connect(high_speed_set_p, SIGNAL(valueChanged(int)), batch_mode_driver_p, SLOT(set_high_feed_speed(int)));
  connect(low_speed_set_p, SIGNAL(valueChanged(int)), batch_mode_driver_p, SLOT(set_low_feed_speed(int)));
  connect(dump_speed_set_p, SIGNAL(valueChanged(int)), batch_mode_driver_p, SLOT(set_dump_feed_speed(int)));
  connect(save_program_button_p, SIGNAL(clicked()), this, SLOT(save_program_clicked()));
  connect(reprint_button_1_p, SIGNAL(clicked()), this, SLOT(reprint_button_1_clicked()));
  connect(reprint_button_2_p, SIGNAL(clicked()), this, SLOT(reprint_button_2_clicked()));
  connect(manual_print_button_p, SIGNAL(clicked()), this, SLOT(manual_print_button_clicked()));
  connect(quit_button_p, SIGNAL(clicked()), this, SLOT(quit_clicked()));
  connect(barcode_line_p, SIGNAL(barcode_entered(QString)), batch_mode_driver_p, SLOT(barcode_entered(QString)));
  connect(low_speed_set_p, SIGNAL(sliderReleased()), this, SLOT(focus_on_barcode()));

  centre_p->set_endgate_state(ENDGATE_CLOSED);
  if(centre_p->totalize_force_endgate_open == true)
  {
    screen_endgate=ENDGATE_OPEN;
  }
  else
  {
    screen_endgate=ENDGATE_CLOSED;
  }
  
  feeder_is_running = true;
  
  run_timer_p = new QTimer;
  connect(run_timer_p, SIGNAL(timeout()), this, SLOT(run()));
  run_timer_p->start(100);
  
  centre_p->count = 0;
  batch_mode_driver_p->start();
  
  mode = old_mode = wait_for_seed_lot_barcode;
  end_valve_mode = old_end_valve_mode = ss_closed_empty;
  
  pack_ready_message = "Pack Ready";
  pack_removed_too_soon_message = "Pack Not Ready\nPut Back";
  dump_container_needed_message = "Place Dump\nContainer";
  dump_container_ready_message = "Finished Dumping";
  dump_container_removed_too_soon_message = "Dumping Not\nFinished";
  bad_seed_lot_message = "Count went over limit.\nDiscard seed lot.\nReduce feed speed.";
  substitution_cleanout_message = "Remove seed in end valve.";
  substitution_barcode_message = "Scan barcode for\nsubstitute seed lot.";
  cancel_substitution_message = "Clean out and\nre-load seed";
  old_barcode_mode = pack;
  
  end_valve_empty_counter = 0;
  connect(batch_mode_driver_p, SIGNAL(pack_collected(int)), this, SLOT(pack_collected(int)));
  connect(batch_mode_driver_p, SIGNAL(dump_complete(int)), this, SLOT(dump_complete(int)));
  connect(batch_mode_driver_p, SIGNAL(dumping()), this, SLOT(dumping()));
  connect(ss_table_p, SIGNAL(focus_on_barcode()), this, SLOT(focus_on_barcode()));
  
  connect(batch_mode_driver_p, SIGNAL(bad_lot_signal()), this, SLOT(bad_lot_slot()));
  connect(batch_mode_driver_p, SIGNAL(refresh_screen()), this, SLOT(refresh_screen()));
    
  if(batch_mode_driver_p->bm_save_ss_setup_filename != "")//returning from batch_save_ss_setup screen with name of file to save
  {
    batch_mode_driver_p->save_ss_setup(batch_mode_driver_p->bm_save_ss_setup_filename);
    batch_mode_driver_p->bm_save_ss_setup_filename = "";
  }
  
  batch_mode_driver_p ->fill_ss_column_pointers();
  ss_table_p -> refresh();
  reprint_line_1 = reprint_line_2 = 0;
  update_reprint_buttons();
   
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

  count_message_p->setStyleSheet("QLabel {" 
        "background-color: white;"          
        "border: 3px solid black;"          
        "font-size: 20pt;}");   

  eject_memory_sticks = false;
  
  barcode_line_p->setFocus();
  batch_mode_driver_p->mode = wait_for_seed_lot_barcode;
  batch_mode_driver_p->seed_lot_barcode_ok = false;

//  batch_mode_driver_p -> restart();

//  cout<<"end of ss_batch::ss_batch.  spreadsheet_line_number = "<<batch_mode_driver_p->spreadsheet_line_number<<endl;
}

ss_batch::~ss_batch()
{
  run_timer_p -> stop();
  while(1)
  {
    cout<<"spreadsheet_filename = "<<batch_mode_driver_p->spreadsheet_filename.toStdString()<<endl;
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

void ss_batch::pack_collected(int)
{
  if( (end_valve_mode==ss_closed_bad_lot) || (end_valve_mode==ss_open_emptying_bad_lot) ) return;
  refresh_screen();
  
  barcode_line_p->setFocus();
  /*
  //QString Alternative  to Integers//  //11_02_2018~~~//
  int row;
  row = table_p -> model_row;
  QString lotcode_str, packcode_str, batch_count_str, substitution_str, dump_count_str;
  
  lotcode_str = table_p -> model_p -> item(row-1, 0) -> text();
  packcode_str = table_p -> model_p -> item(row-1, 1) -> text();
  batch_count_str = QString::number(count_to_record);
  substitution_str = table_p -> model_p -> item(row-1, 3) -> text();
  
  macro_screen_p -> usb_serial_out(lotcode_str, packcode_str, batch_count_str, substitution_str, dump_count_str);
  */
}  

void ss_batch::dump_complete(int)
{
  end_valve_mode = ss_dump_open_empty;
  barcode_line_p->setFocus();
  /*
  //QString variant to integers//	11_02_2018//
  QString lotcode_str, packcode_str, batch_count_str, substitution_str, dump_count_str;
  dump_count_str = QString::number(dump_count);
  macro_screen_p -> usb_serial_out(lotcode_str, packcode_str, batch_count_str, substitution_str, dump_count_str);
  */
}

void ss_batch::dumping()
{
  end_valve_mode = ss_dump_closed_empty;
}

void ss_batch::focus_on_barcode()
{
  barcode_line_p -> setFocus();
}

void ss_batch::bad_lot_slot()
{
  end_valve_mode = ss_closed_bad_lot;
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

void ss_batch::remove_drive_clicked()
{
  eject_memory_sticks = true;//signal to destructor
  
  batch_mode_driver_p -> stop();
  centre_p->set_speed(0);
  centre_p->add_waiting_screen(0);
  centre_p->screen_done = true;
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
  
  focus_on_barcode();
}

void ss_batch::restart_clicked()
{
  batch_mode_driver_p -> restart();
}

void ss_batch::substitution_button_clicked()
{
  if(centre_p->get_endgate_state() == ENDGATE_CLOSED)
  {
    batch_mode_driver_p -> mode = substitution_wait_for_cleanout_open;
    end_valve_mode = ss_substitution_wait_for_cleanout;
  }
  else
  {
    batch_mode_driver_p -> mode = substitution_wait_for_cleanout_close;
    end_valve_mode = ss_substitution_wait_for_cleanout;
  }
  batch_mode_driver_p->pack_barcode_ok = false;
  batch_mode_driver_p->pack_barcode_old = true;
  batch_mode_driver_p->substitute_seed_lot = true;
  batch_mode_driver_p->barcode_mode = substitution;
  
  focus_on_barcode();
}

void ss_batch::cancel_substitution_button_clicked()
{
  batch_mode_driver_p->current_pack --;
  if(centre_p->get_endgate_state() == ENDGATE_CLOSED)
  {
    batch_mode_driver_p -> mode = cancel_substitution_wait_for_cleanout_open;
  }
  else
  {
    batch_mode_driver_p -> mode = cancel_substitution_wait_for_cleanout_close;
  }
  end_valve_mode = ss_cancel_substitution;
  batch_mode_driver_p->pack_barcode_ok = false;
  batch_mode_driver_p->pack_barcode_old = true;
  batch_mode_driver_p->substitute_seed_lot = false;
  batch_mode_driver_p->barcode_mode = seed_lot;
  
  focus_on_barcode();
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
  if(batch_mode_driver_p->ss_batch_exit_flag == true) quit_clicked();//exit flag can be set by QMessageBox in batch_mode_driver::run
  
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

  old_mode = mode;
  mode = batch_mode_driver_p->mode;
  old_end_valve_mode = end_valve_mode;
  ++end_valve_empty_counter;
  
  
  switch(end_valve_mode)
  {
    case ss_closed_empty:
      if(centre_p->cutgate_p->get_state() == CUTGATE_OPEN)
      {
        end_valve_mode = ss_closed_filling;
      }
      if(centre_p->get_endgate_state() == ENDGATE_OPEN)
      {
        end_valve_mode = ss_opened_while_empty;
      }
      break;
    case ss_opened_while_empty:
      if(centre_p->cutgate_p->get_state() == CUTGATE_OPEN)
      {
        end_valve_mode = ss_open_pass_through;
      }
      if(centre_p->get_endgate_state() == ENDGATE_CLOSED)
      {
        end_valve_mode = ss_pack_removed_too_soon;
      }
      break;
    case ss_closed_filling:
      if(centre_p->cutgate_p->get_state() == CUTGATE_CLOSED)
      {
        end_valve_mode = ss_closed_full;
      }
      if(centre_p->get_endgate_state() == ENDGATE_OPEN)
      {
        end_valve_mode = ss_open_pass_through;
      }
      break;
    case ss_closed_full:
      if(centre_p->cutgate_p->get_state() == CUTGATE_OPEN)//not expected
      {
        cout<<"cutgate opened in end_valve_mode ss_closed_full\n";
      }
      if(centre_p->get_endgate_state() == ENDGATE_OPEN)
      {
        end_valve_mode = ss_open_emptying;
        end_valve_empty_counter = 0;
      }
      break;
    case ss_open_emptying:
      if(end_valve_empty_counter >= 5)//0.5 sec.
      {
        end_valve_mode = ss_open_empty;
      }
      if(centre_p->get_endgate_state() == ENDGATE_CLOSED)
      {
        end_valve_mode = ss_pack_removed_too_soon;
      }
      break;
    case ss_open_pass_through:
      if(centre_p->cutgate_p->get_state() == CUTGATE_CLOSED)
      {
        end_valve_mode = ss_open_emptying;
        end_valve_empty_counter = 0;
      }
      if(centre_p->get_endgate_state() == ENDGATE_CLOSED)
      {
        end_valve_mode = ss_pack_removed_too_soon;
      }
      break;
    case ss_open_empty:
      if(centre_p->cutgate_p->get_state() == CUTGATE_OPEN)//not expected
      {
        cout<<"cutgate opened in end_valve_mode ss_open_empty\n";
      }
      if(centre_p->get_endgate_state() == ENDGATE_CLOSED)
      {
        end_valve_mode = ss_closed_empty;
        batch_mode_driver_p->pack_complete = true;
//        batch_mode_driver_p->substitute_seed_lot = false;
      }
      break;
    case ss_pack_removed_too_soon:
      if(centre_p->get_endgate_state() == ENDGATE_OPEN)
      {
        if(centre_p->cutgate_p->get_state() == CUTGATE_OPEN)
        {
          end_valve_mode = ss_open_pass_through;
        }
        else
        {
          end_valve_mode = ss_open_emptying;
          end_valve_empty_counter = 0;
        }
      }
      break;
    case ss_closed_bad_lot:
      if(centre_p->cutgate_p->get_state() == CUTGATE_OPEN)//not expected
      {
        cout<<"cutgate opened in end_valve_mode ss_closed_bad_lot\n";
      }
      if(centre_p->get_endgate_state() == ENDGATE_OPEN)
      {
        end_valve_mode = ss_open_emptying_bad_lot;
      }
      break;
    case ss_open_emptying_bad_lot:
      if(centre_p->cutgate_p->get_state() == CUTGATE_OPEN)//not expected
      {
        cout<<"cutgate opened in end_valve_mode ss_open_emptying_bad_lot\n";
      }
      if(centre_p->get_endgate_state() == ENDGATE_CLOSED)
      {
        end_valve_mode = ss_closed_empty;
      }
      break;
    case ss_dump_closed_empty:
      if(centre_p->cutgate_p->get_state() == CUTGATE_OPEN)
      {
        end_valve_mode = ss_dump_closed_filling;
      }
      if(centre_p->get_endgate_state() == ENDGATE_OPEN)
      {
        end_valve_mode = ss_dump_opened_while_empty;
      }
      break;
    case ss_dump_opened_while_empty:
      if(centre_p->cutgate_p->get_state() == CUTGATE_OPEN)
      {
        end_valve_mode = ss_dump_pass_through;
      }
      if(centre_p->get_endgate_state() == ENDGATE_CLOSED)
      {
        end_valve_mode = ss_dump_container_removed_too_soon;
        cout<<"entering ss_dump_container_removed_too_soon from ss_dump_opened_while_empty\n";
      }
      break;
    case ss_dump_closed_filling:
      if(centre_p->cutgate_p->get_state() == CUTGATE_CLOSED)//not expected
      {
        cout<<"cutgate closed in end_valve_mode ss_dump_closed_filling\n";
      }
      if(centre_p->get_endgate_state() == ENDGATE_OPEN)
      {
        end_valve_mode = ss_dump_pass_through;
      }
      break;
    case ss_dump_pass_through:
      if(centre_p->cutgate_p->get_state() == CUTGATE_CLOSED)//not expected
      {
        cout<<"cutgate closed in end_valve_mode ss_dump_pass_through\n";
      }
      if(centre_p->get_endgate_state() == ENDGATE_CLOSED)
      {
        end_valve_mode = ss_dump_container_removed_too_soon;
        cout<<"entering ss_dump_container_removed_too_soon from ss_dump_pass_through\n";
      }
      break;
    case ss_dump_open_empty:
      if(centre_p->cutgate_p->get_state() == CUTGATE_CLOSED)//not expected
      {
        cout<<"cutgate closed in end_valve_mode ss_dump_open_empty\n";
      }
      if(centre_p->get_endgate_state() == ENDGATE_CLOSED)
      {
        end_valve_mode = ss_closed_filling;
      }
      break;
    case ss_dump_container_removed_too_soon:
      if( (centre_p->cutgate_p->get_state()==CUTGATE_CLOSED) && (centre_p->get_endgate_state()==ENDGATE_OPEN) )
      {
        end_valve_mode = ss_dump_opened_while_empty;
      }
      if( (centre_p->cutgate_p->get_state()==CUTGATE_OPEN) && (centre_p->get_endgate_state()==ENDGATE_OPEN) )
      {
        end_valve_mode = ss_dump_pass_through;
      }
      break;
    case ss_substitution_wait_for_cleanout:
      if(batch_mode_driver_p->mode==substitution_wait_for_barcode)
      {
        end_valve_mode = ss_substitution_enter_barcode;
      }
      break;
    case ss_substitution_enter_barcode:
      if(batch_mode_driver_p->seed_lot_barcode_ok == true)
      {
//        centre_p -> communicate_out('s');
        end_valve_mode = ss_closed_filling;
      }
      break;
    case ss_cancel_substitution:
      if(batch_mode_driver_p->mode == hi_open)
      {
        end_valve_mode = ss_closed_filling;
      }
      break;
    default:
      cout<<"end_valve_mode not found\n";
  }
      
  if( (end_valve_mode==ss_closed_full) || (end_valve_mode==ss_open_emptying) || (end_valve_mode==ss_open_empty) )
  {
    QString number;
    pack_ready_message = "PACK READY.\n";
    pack_ready_message.append(number.setNum(batch_mode_driver_p->lines_left_to_fill));
    pack_ready_message.append(" packs left for \nthis seed lot.");
    status_box_p->set_text(pack_ready_message);
    status_box_p->set_text_size(20);
    status_box_p->set_background(0, 255, 0);
    status_box_p->update();
  }
  
  else if(end_valve_mode==ss_pack_removed_too_soon)
  {
    status_box_p->set_text(pack_removed_too_soon_message);
    status_box_p->set_text_size(30);
    status_box_p->set_background(255, 0, 0);
    status_box_p->update();
  }
  else if(end_valve_mode==ss_dump_open_empty)
  {
    status_box_p->set_text(dump_container_ready_message);
    status_box_p->set_text_size(30);
    status_box_p->set_background(0, 255, 0);
    status_box_p->update();
  }
  else if(end_valve_mode==ss_dump_container_removed_too_soon)
  {
    status_box_p->set_text(dump_container_removed_too_soon_message);
    status_box_p->set_text_size(30);
    status_box_p->set_background(255, 0, 0);
    status_box_p->update();
  }
  else if( (end_valve_mode==ss_closed_bad_lot) || (end_valve_mode==ss_open_emptying_bad_lot) )
  {
    status_box_p->set_text(bad_seed_lot_message);
    status_box_p->set_text_size(15);
    status_box_p->set_background(255, 0, 0);
    status_box_p->update();
  }    
  else if(end_valve_mode==ss_substitution_wait_for_cleanout)
  {
    status_box_p->set_text(substitution_cleanout_message);
    status_box_p->set_text_size(20);
    status_box_p->set_background(255, 255, 0);
    status_box_p->update();
  }    
  else if(end_valve_mode==ss_substitution_enter_barcode)
  {
    status_box_p->set_text(substitution_barcode_message);
    status_box_p->set_text_size(20);
    status_box_p->set_background(30, 200, 255);
    status_box_p->update();
  }    
  else if(end_valve_mode==ss_cancel_substitution)
  {
    status_box_p->set_text(cancel_substitution_message);
    status_box_p->set_text_size(20);
    status_box_p->set_background(255, 255, 0);
    status_box_p->update();
  }    
  else
  {
    if( (end_valve_mode==ss_dump_closed_filling) || (end_valve_mode==ss_dump_pass_through) || (end_valve_mode==ss_dump_open_empty) )
    {
      status_box_p->set_text(dump_container_needed_message);
      status_box_p->set_text_size(30);
      status_box_p->set_background(255, 255, 0);
      status_box_p->update();
    }  
    else
    {
      message=QString("Crop: %1.  Sensitivity: %2.\nGate should be set at %3.\n%4 packs left\nfor this seed lot.\n%5 seeds in this pack.") 
        .arg(centre_p->crops[0].name) 
        .arg(centre_p->crops[0].sensitivity) 
        .arg(centre_p->crops[0].gate_setting)
        .arg(batch_mode_driver_p->lines_left_to_fill)
        .arg(batch_mode_driver_p->current_count_limit);
      status_box_p->set_text(message);
      status_box_p->set_text_size(16);
      status_box_p->set_background(30, 200, 255);
      status_box_p->update();
    }
  } 
  
  //barcode status
  if(batch_mode_driver_p->barcode_mode == seed_lot)
  {
    if(batch_mode_driver_p->mode==wait_for_seed_lot_barcode)
    {
      if(batch_mode_driver_p->seed_lot_barcode_ok==false)
      {
        barcode_status_p->set_text("Scan Seed Lot Barcode");
        barcode_status_p->set_text_size(30); 
        barcode_status_p->set_background(255, 255, 0);
        barcode_status_p->update();
      } 
    }
    else//mode is not wait_for_seed_lot_barcode
    {
      barcode_status_p->set_text("");
      barcode_status_p->set_text_size(20); 
      barcode_status_p->set_background(30, 200, 255);
      barcode_status_p->update();
    }       
  }
  if(batch_mode_driver_p->barcode_mode == pack)
  {
    if(batch_mode_driver_p->require_pack_barcode == true)
    {
      if(batch_mode_driver_p->pack_barcode_old==true)
      {
        QString msg = "Scan Pack Barcode.";
        if(batch_mode_driver_p->use_spreadsheet == true)
        {
          QString spreadsheet_pack_id = batch_mode_driver_p->ss_envelope_id_p->data_list[batch_mode_driver_p->end_valve_spreadsheet_line_number];
          msg.append("  Pack ");
          msg.append(spreadsheet_pack_id);
          msg.append(".\nSeed Lot:"); 
          msg.append(batch_mode_driver_p->seed_lot_barcode);
          
          
          
          
          
          
          if(batch_mode_driver_p->substitute_seed_lot==true)
          {
            msg.append(".  Substituting with Seed Lot ");
            msg.append(batch_mode_driver_p->substitute_barcode);
          }   
          
          
          
          
          
          
        }
        barcode_status_p->set_text(msg);
        barcode_status_p->set_text_size(15); 
        barcode_status_p->set_background(255, 255, 0);
        barcode_status_p->update();
      }
      if( (batch_mode_driver_p->pack_barcode_old==false) && (batch_mode_driver_p->pack_barcode_ok==true) )
      {
        QString msg = "Barcode OK.  Seed Lot: ";
        msg.append(batch_mode_driver_p->seed_lot_barcode);
          
          
          
          
          
        if(batch_mode_driver_p->substitute_seed_lot==true)
        {
          msg.append(".  Substituting with Seed Lot ");
          msg.append(batch_mode_driver_p->substitute_barcode);
        }   
          
          
          
          
          
        msg.append("\nPack Barcode Scanned: ");
        msg.append(batch_mode_driver_p->ss_envelope_id_p->data_list[batch_mode_driver_p->end_valve_spreadsheet_line_number]);
        barcode_status_p->set_text(msg);
        barcode_status_p->set_text_size(15); 
        barcode_status_p->set_background(0, 255, 0);
        barcode_status_p->update();
      }
      if( (batch_mode_driver_p->pack_barcode_old==false) && (batch_mode_driver_p->pack_barcode_ok==false) )
      {
        QString msg = "Incorrect Barcode";
        if(batch_mode_driver_p->use_spreadsheet == true)
        {
          QString spreadsheet_pack_id = batch_mode_driver_p->ss_envelope_id_p->data_list[batch_mode_driver_p->end_valve_spreadsheet_line_number];
          msg.append("  Pack required:");
          msg.append(spreadsheet_pack_id);
          msg.append(".  Pack scanned:");
          msg.append(batch_mode_driver_p->pack_barcode);
          if(batch_mode_driver_p->release_pack == false)
          {
            msg.append("\nIf correct pack unavailabe, press RELEASE PACK");
          }
          else
          {
            msg.append("\nRELEASE PACK was pressed.  lift envelope sensor.");
          }
        }
        else
        {
          msg.append("\nSeed Lot: ");
          msg.append(batch_mode_driver_p->seed_lot_barcode);
        }
        barcode_status_p->set_text(msg);
        barcode_status_p->set_text_size(15); 
        barcode_status_p->set_background(255, 0, 0);
        barcode_status_p->update();
      }
    }
    else//barcode not required
    {
      barcode_status_p->set_text("Barcode not required");
      barcode_status_p->set_text_size(30); 
      barcode_status_p->set_background(0, 255, 0);
      barcode_status_p->update();
    }
  }
  
  
  
  
  
  if(batch_mode_driver_p->barcode_mode == substitution)
  {
    if(end_valve_mode==ss_substitution_enter_barcode)
    {
      barcode_status_p->set_text("Scan Substitution Barcode");
      barcode_status_p->set_text_size(30); 
      barcode_status_p->set_background(255, 255, 0);
      barcode_status_p->update();
    }
    else
    {
      barcode_status_p->set_text("");
      barcode_status_p->set_text_size(30); 
      barcode_status_p->set_background(0, 255, 0);
      barcode_status_p->update();
    }
  }
  
  
  
  
  
  
  
  
  
  if(batch_mode_driver_p->mode == wait_for_bad_lot_cleanout)
  {
    barcode_status_p->set_text("Remove over-counted seed lot.");
    barcode_status_p->set_text_size(20); 
    barcode_status_p->set_background(255, 0, 0);
    barcode_status_p->update();
  }
    
  old_barcode_mode = batch_mode_driver_p->barcode_mode;
  
  update_reprint_buttons();
      
}

void ss_batch::update_reprint_buttons()
{

  QString reprint_message = "reprint pack\n";
  if(batch_mode_driver_p->spreadsheet_line_number >= 0)//-1 value used to signal no more seeds for seed lot.  Do not update in this case.
  {
    reprint_line_1 = batch_mode_driver_p->spreadsheet_line_number;
  }
  if( (reprint_line_1<0)  ||  (reprint_line_1>=batch_mode_driver_p->ss_envelope_id_p->data_list.size())  )//not expected 
  {
    cout<<"ss_batch:update_reprint_buttons.  reprint_line_1 = "<<reprint_line_1<<"\n";
    exit(1);
  }
  reprint_message.append(batch_mode_driver_p->ss_envelope_id_p->data_list[reprint_line_1]);
  reprint_button_1_p -> setText(reprint_message);
  reprint_message = "reprint pack\n";
  reprint_line_2 = reprint_line_1 - 1;
  if(reprint_line_2>=0)
  {
    reprint_message.append(batch_mode_driver_p->ss_envelope_id_p->data_list[reprint_line_2]);
    reprint_button_2_p->setText(reprint_message);
    reprint_button_2_p->show();
  }
  else
  {
    reprint_button_2_p->hide();
  }
}

void ss_batch::reprint_button_1_clicked()
{
  batch_mode_driver_p -> envelope_p -> print(reprint_line_1);
}
  
void ss_batch::reprint_button_2_clicked()
{
  batch_mode_driver_p -> envelope_p -> print(reprint_line_2);
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
  print_line_1_p = new button("");
  print_line_2_p = new button("");
  print_line_3_p = new button("");
  print_line_4_p = new button("");
  print_line_5_p = new button("");
  repeat_line_1_p = new button("");
  repeat_line_2_p = new button("");
  repeat_line_3_p = new button("");
  repeat_line_4_p = new button("");
  repeat_line_5_p = new button("");
  next_p = new button("Next line");
  close_p = new button("Close");
  
  layout_p -> addWidget(previous_p,0,0,1,2);
  layout_p -> addWidget(print_line_1_p,1,0);
  layout_p -> addWidget(print_line_2_p,2,0);
  layout_p -> addWidget(print_line_3_p,3,0);
  layout_p -> addWidget(print_line_4_p,4,0);
  layout_p -> addWidget(print_line_5_p,5,0);
  layout_p -> addWidget(repeat_line_1_p,1,1);
  layout_p -> addWidget(repeat_line_2_p,2,1);
  layout_p -> addWidget(repeat_line_3_p,3,1);
  layout_p -> addWidget(repeat_line_4_p,4,1);
  layout_p -> addWidget(repeat_line_5_p,5,1);
  layout_p -> addWidget(next_p,6,0,1,2);
  layout_p -> addWidget(close_p,7,0,1,2);
  setLayout(layout_p);
  
  connect(previous_p, SIGNAL(clicked()), this, SLOT(previous_clicked()));
  connect(print_line_1_p, SIGNAL(clicked()), this, SLOT(print_line_1_clicked()));
  connect(print_line_2_p, SIGNAL(clicked()), this, SLOT(print_line_2_clicked()));
  connect(print_line_3_p, SIGNAL(clicked()), this, SLOT(print_line_3_clicked()));
  connect(print_line_4_p, SIGNAL(clicked()), this, SLOT(print_line_4_clicked()));
  connect(print_line_5_p, SIGNAL(clicked()), this, SLOT(print_line_5_clicked()));
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
  
}

void manual_operation_window::update_buttons()
{
//  first_line = batch_mode_driver_p->spreadsheet_line_number-5;
  
  int max_first_line = batch_mode_driver_p->ss_envelope_id_p->data_list.size()-5;
  if(first_line>max_first_line) first_line = max_first_line;
//  if(first_line<0) first_line = 0;
  
  cout<<"manual_operation_window::update_buttons.  first_line = "<<first_line<<endl;
  
  if(first_line>=0)
  {
    print_line_1_p -> setText(QString("Reprint ") + batch_mode_driver_p->ss_envelope_id_p->data_list[first_line]); 
    repeat_line_1_p -> setText(QString("Repeat ") + batch_mode_driver_p->ss_envelope_id_p->data_list[first_line]); 
  }
  else
  {
    print_line_1_p -> hide();
    repeat_line_1_p -> hide();
  }
  
  if(first_line+1>=0)
  {
    print_line_2_p -> setText(QString("Reprint ") + batch_mode_driver_p->ss_envelope_id_p->data_list[first_line+1]); 
    repeat_line_2_p -> setText(QString("Repeat ") + batch_mode_driver_p->ss_envelope_id_p->data_list[first_line+1]); 
  }
  else
  {
    print_line_2_p -> hide();
  }
  
  if(first_line+2>=0)
  {
    print_line_3_p -> setText(QString("Reprint ") + batch_mode_driver_p->ss_envelope_id_p->data_list[first_line+2]); 
    repeat_line_3_p -> setText(QString("Repeat ") + batch_mode_driver_p->ss_envelope_id_p->data_list[first_line+2]); 
  }
  else
  {
    print_line_3_p -> hide();
  }
  
  if(first_line+3>=0)
  {
    print_line_4_p -> setText(QString("Reprint ") + batch_mode_driver_p->ss_envelope_id_p->data_list[first_line+3]); 
    repeat_line_4_p -> setText(QString("Repeat ") + batch_mode_driver_p->ss_envelope_id_p->data_list[first_line+3]); 
  }
  else
  {
    print_line_4_p -> hide();
  }
  
  if(first_line+4>=0)
  {
    print_line_5_p -> setText(QString("Reprint ") + batch_mode_driver_p->ss_envelope_id_p->data_list[first_line+4]); 
    repeat_line_5_p -> setText(QString("Repeat ") + batch_mode_driver_p->ss_envelope_id_p->data_list[first_line+4]); 
  }
  else
  {
    print_line_5_p -> hide();
  }
}

void manual_operation_window::previous_clicked()
{
  --first_line;
  update_buttons();//will check first_line and may change it
}

void manual_operation_window::print_line_1_clicked()
{
  if(first_line<0) return;
  batch_mode_driver_p -> envelope_p -> print(first_line);
}
  
void manual_operation_window::print_line_2_clicked()
{
  if(first_line+1<0) return;
  batch_mode_driver_p -> envelope_p -> print(first_line+1);
}
  
void manual_operation_window::print_line_3_clicked()
{
  if(first_line+2<0) return;
  batch_mode_driver_p -> envelope_p -> print(first_line+2);
}
  
void manual_operation_window::print_line_4_clicked()
{
  if(first_line+3<0) return;
  batch_mode_driver_p -> envelope_p -> print(first_line+3);
}
  
void manual_operation_window::print_line_5_clicked()
{
  if(first_line+4<0) return;
  batch_mode_driver_p -> envelope_p -> print(first_line+4);
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

