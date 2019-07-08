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
//#include "batch_mode_driver.hpp"
#include "message_box.hpp"


using namespace std;

void ss_barcode_line::keyPressEvent(QKeyEvent* event)
{
  QLineEdit::keyPressEvent(event); //TEST~~~
  if(event->key() == Qt::Key_Return)
  {
//    cout<<"about to emit barcode_entered("<<displayText().toStdString()<<")\n";
    emit barcode_entered(displayText());  //TEST~~~
//    cout<<"emitted barcode_entered("<<displayText().toStdString()<<")\n";
    clear();
  }
}

ss_batch::ss_batch(centre* set_centre_p, batch_mode_driver* set_batch_mode_driver_p)
 :screen(set_centre_p)
{
  
  cout<<"p1\n";
  
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
  back_button_p = new button("Back");
  barcode_line_p = new ss_barcode_line;
  barcode_line_p->setMaximumSize(120,40);  //ORIGINAL~~~
  rescan_button_p = new button("Rescan seed lot barcode");
  restart_button_p = new button("Dump out and\nrestart seed lot");
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
  
  high_speed_set_p->setValue(batch_mode_driver_p->high_feed_speed);
  low_speed_set_p->setValue(batch_mode_driver_p->low_feed_speed);
  dump_speed_set_p->setValue(batch_mode_driver_p->dump_speed);
  
  cout<<"p2\n";
  
  
  ss_table_p = new ss_batch_table(centre_p, batch_mode_driver_p);
  
  cout<<"p2b\n";
  
  status_box_p = new message_box;
  status_box_p -> setMinimumSize(250,100);
  save_program_button_p = new button("Save Spreadsheet\nSetup");
  reprint_button_p = new button("Reprint\nenvelope");
  quit_button_p = new button("Quit");
  barcode_status_p = new message_box;
  
  top_box_p = new QGroupBox;
  control_box_p = new QGroupBox;
  speed_box_p = new QGroupBox;
  speed_box_p -> setTitle("Speeds");
  bottom_box_p = new QGroupBox;
    
  
  cout<<"p3\n";
  
  top_layout_p = new QGridLayout;
  control_layout_p = new QGridLayout;
  speed_layout_p = new QGridLayout;
  bottom_layout_p = new QGridLayout;  
  main_layout_p = new QGridLayout;

  top_layout_p -> addWidget(count_message_p, 0, 0);
  top_layout_p -> addWidget(options_button_p, 0, 1);
  top_layout_p -> addWidget(back_button_p, 0, 2);
  bottom_layout_p -> addWidget(barcode_line_p, 1, 4);   
  barcode_line_p -> setText("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");  
  barcode_line_p -> clear();  
  control_layout_p -> addWidget(rescan_button_p, 0, 0);
  control_layout_p -> addWidget(restart_button_p, 0, 1);
  control_layout_p -> addWidget(speed_box_p, 1, 0, 1, 2);     
  speed_layout_p -> addWidget(high_speed_label_p, 0, 0);
  speed_layout_p -> addWidget(high_speed_set_p, 0, 1);
  speed_layout_p -> addWidget(low_speed_label_p, 1, 0);
  speed_layout_p -> addWidget(low_speed_set_p, 1, 1);
  speed_layout_p -> addWidget(dump_speed_label_p, 2, 0);
  speed_layout_p -> addWidget(dump_speed_set_p, 2, 1);
  bottom_layout_p -> addWidget(status_box_p, 0, 0, 2, 1);
  bottom_layout_p -> addWidget(save_program_button_p, 0, 2);
  bottom_layout_p -> addWidget(reprint_button_p, 0, 3);
  bottom_layout_p -> addWidget(quit_button_p, 0, 5);
  bottom_layout_p -> addWidget(barcode_status_p, 1, 2, 1, 3);
  main_layout_p -> addWidget(top_box_p, 0, 0, 1, 2);
  main_layout_p -> addWidget(control_box_p, 1, 0);
  main_layout_p -> addWidget(ss_table_p, 1, 1);
  main_layout_p -> addWidget(bottom_box_p, 2, 0, 1, 2);
  
  top_box_p -> setLayout(top_layout_p);
  control_box_p -> setLayout(control_layout_p);
  speed_box_p -> setLayout(speed_layout_p);
  bottom_box_p -> setLayout(bottom_layout_p);
  setLayout(main_layout_p);
  
  cout<<"p4\n";
  
  
  connect(options_button_p, SIGNAL(clicked()), this, SLOT(options_clicked()));
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_clicked()));
  connect(rescan_button_p, SIGNAL(clicked()), this, SLOT(rescan_clicked()));
  connect(restart_button_p, SIGNAL(clicked()), this, SLOT(restart_clicked()));
  connect(high_speed_set_p, SIGNAL(valueChanged(int)), batch_mode_driver_p, SLOT(set_high_feed_speed(int)));
  connect(low_speed_set_p, SIGNAL(valueChanged(int)), batch_mode_driver_p, SLOT(set_low_feed_speed(int)));
  connect(dump_speed_set_p, SIGNAL(valueChanged(int)), batch_mode_driver_p, SLOT(set_dump_feed_speed(int)));
  connect(save_program_button_p, SIGNAL(clicked()), this, SLOT(save_program_clicked()));
  connect(reprint_button_p, SIGNAL(clicked()), this, SLOT(reprint_clicked()));
  connect(quit_button_p, SIGNAL(clicked()), this, SLOT(quit_clicked()));
  connect(barcode_line_p, SIGNAL(barcode_entered(QString)), batch_mode_driver_p, SLOT(barcode_entered(QString)));
//  connect(batch_mode_driver_p, SIGNAL(seed_lot_barcode_entered(QString)), ss_table_p, SLOT(enter_seed_lot_barcode(QString)));
//  connect(batch_mode_driver_p, SIGNAL(pack_barcode_entered(QString)), ss_table_p, SLOT(enter_pack_barcode(QString)));
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
  
  count = 0;
  batch_mode_driver_p->start();
  
  mode = old_mode = wait_for_seed_lot_barcode;
  end_valve_mode = old_end_valve_mode = ss_closed_empty;
  
  
//  pack_was_placed = false;
//  pack_was_removed = false;
//  seed_for_pack_ready = false;
//  pack_can_be_removed = false;
//  ss_pack_removed_too_soon = false;
  pack_ready_message = "Pack Ready";
  pack_removed_too_soon_message = "Pack Not Ready\nPut Back";
//  dump_container_was_placed = false;
//  dump_container_can_be_removed = false;
//  ss_dump_container_removed_too_soon = false;
  dump_container_needed_message = "Place Dump\nContainer";
  dump_container_ready_message = "Finished Dumping";
  dump_container_removed_too_soon_message = "Dumping Not\nFinished";
  bad_seed_lot_message = "Count went over limit.\nDiscard seed lot.\nReduce feed speed.";
//  dump_flag = false;
  old_barcode_mode = pack;
  
  cout<<"p5\n";
  
  
  end_valve_empty_counter = 0;
  connect(batch_mode_driver_p, SIGNAL(pack_collected(int)), this, SLOT(pack_collected(int)));
  connect(batch_mode_driver_p, SIGNAL(dump_complete(int)), this, SLOT(dump_complete(int)));
  connect(batch_mode_driver_p, SIGNAL(dumping()), this, SLOT(dumping()));
  connect(ss_table_p, SIGNAL(focus_on_barcode()), this, SLOT(focus_on_barcode()));
//  connect(batch_mode_driver_p, SIGNAL(pack_ready()), this, SLOT(pack_ready()));
  
  connect(batch_mode_driver_p, SIGNAL(bad_lot_signal()), this, SLOT(bad_lot_slot()));
  connect(batch_mode_driver_p, SIGNAL(refresh_screen()), this, SLOT(refresh_screen()));
    
//  table_p->load_file("settings/batch_backup");
  
//  if(batch_mode_driver_p->bm_save_program_filename != "")//returning from batch_save_program screen with name of file to save
//  {
//    batch_mode_driver_p->save_program(batch_mode_driver_p->bm_save_program_filename);
//    batch_mode_driver_p->bm_save_program_filename = "";
//  }

  if(batch_mode_driver_p->bm_save_ss_setup_filename != "")//returning from batch_save_ss_setup screen with name of file to save
  {
    batch_mode_driver_p->save_ss_setup(batch_mode_driver_p->bm_save_ss_setup_filename);
    batch_mode_driver_p->bm_save_ss_setup_filename = "";
  }
  
//  if(batch_mode_driver_p->bm_save_table_filename != "")//returning from batch_save_table_file screen with name of file to save
//  {
    
//    cout<<"batch::batch.  save file name = "<<batch_mode_driver_p->bm_save_table_filename.toStdString()<<endl;
    
//    table_p->save_file(batch_mode_driver_p->bm_save_table_filename);
//    batch_mode_driver_p->bm_save_table_filename = "";
//  }
  
  batch_mode_driver_p ->fill_ss_column_pointers();
  ss_table_p -> refresh();
  
  
  
   
  cout<<"p0\n";
  cout<<"heading "<<batch_mode_driver_p->ss_material_id_p->heading.toStdString()<<endl;

  barcode_line_p->setFocus();

  top_layout_p->setContentsMargins(0,0,0,0);        //set layout margins to shrink to designated container dimensions//
  control_layout_p->setContentsMargins(0,0,0,0);
  speed_layout_p->setContentsMargins(0,0,0,0);
  bottom_layout_p->setContentsMargins(0,0,0,0);
  main_layout_p->setContentsMargins(0,0,10,10);
  
  main_layout_p->setRowMinimumHeight(5, 0);
  main_layout_p->setRowStretch(5, 0);

  count_message_p->setStyleSheet("QLabel {" 
        "background-color: white;"          
        "border: 3px solid black;"          
        "font-size: 20pt;}");               
  
  cout<<"p6\n";
  
}

ss_batch::~ss_batch()
{
  run_timer_p -> stop();
//  table_p->save_file(QString("settings/batch_backup"));
//  bool spreadsheet_saved = false;
//  bool exit_loop = false;
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
//  delete clear_table_button_p;
  delete quit_button_p;
  delete top_layout_p;
  delete top_box_p;
  delete speed_box_p;
  delete bottom_box_p;
  delete control_layout_p;
  delete main_layout_p;
  
}
/*
void batch::pack_ready()
{
  seed_for_pack_ready = true;//will stay true until pack is removed
}  
*/
void ss_batch::pack_collected(int count_to_record)
{
//  pack_can_be_removed = false;
//  ss_pack_removed_too_soon = false;
//  pack_was_placed = false;
  
  if( (end_valve_mode==ss_closed_bad_lot) || (end_valve_mode==ss_open_emptying_bad_lot) ) return;
  
//  table_p->enter_seeds(count_to_record);

  if(batch_mode_driver_p->ss_actual_count_p!=0)
  {
    batch_mode_driver_p->ss_actual_count_p->data_list[batch_mode_driver_p->spreadsheet_line_number] = QString::number(count_to_record);
  }
  
  refresh_screen();
  
  barcode_line_p->setFocus();
//  pack_was_removed = true;
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

void ss_batch::dump_complete(int dump_count)
{
//  dump_container_can_be_removed = true;
//  dump_container_ready_count = dump_count;
  
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

void ss_batch::back_clicked()
{
  batch_mode_driver_p -> stop();
  centre_p->set_speed(0);
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void ss_batch::rescan_clicked()
{
  batch_mode_driver_p->mode = wait_for_seed_lot_barcode;
  batch_mode_driver_p->seed_lot_barcode_ok = false;
}

void ss_batch::restart_clicked()
{
  batch_mode_driver_p -> restart();
//  pack_can_be_removed = false;
//  ss_pack_removed_too_soon = false;
//  dump_container_can_be_removed = false;
//  dump_flag = true;
//  ss_dump_container_removed_too_soon = false;
}

void ss_batch::save_program_clicked()
{
  batch_mode_driver_p -> stop();
  centre_p->set_speed(0);
  centre_p->add_waiting_screen(29);//batch_save_ss_setup
  centre_p->screen_done = true;
}

void ss_batch::reprint_clicked()
{
  batch_mode_driver_p -> envelope_p -> print();
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
        
        
        cout<<"        ss_batch::run.  batch_mode_driver_p->pack_complete = "<<batch_mode_driver_p->pack_complete<<".  setting true.\n";
        batch_mode_driver_p->pack_complete = true;
        
        
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
    default:
      cout<<"end_valve_mode not found\n";
  }
      













//  if( (pack_can_be_removed==true) && (ss_pack_removed_too_soon==false) && (dump_flag==false) )
  
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
//  else if(ss_pack_removed_too_soon)
  
  else if(end_valve_mode==ss_pack_removed_too_soon)
  {
    status_box_p->set_text(pack_removed_too_soon_message);
    status_box_p->set_text_size(30);
    status_box_p->set_background(255, 0, 0);
    status_box_p->update();
  }
//  else if(dump_container_can_be_removed)
  else if(end_valve_mode==ss_dump_open_empty)
  {
    status_box_p->set_text(dump_container_ready_message);
    status_box_p->set_text_size(30);
    status_box_p->set_background(0, 255, 0);
    status_box_p->update();
  }
//  else if(ss_dump_container_removed_too_soon)
  else if(end_valve_mode==ss_dump_container_removed_too_soon)
  {
    status_box_p->set_text(dump_container_removed_too_soon_message);
    status_box_p->set_text_size(30);
    status_box_p->set_background(255, 0, 0);
    status_box_p->update();
  }
//  else if(batch_mode_driver_p->mode == wait_for_bad_lot_cleanout)
  else if( (end_valve_mode==ss_closed_bad_lot) || (end_valve_mode==ss_open_emptying_bad_lot) )
  {
    status_box_p->set_text(bad_seed_lot_message);
    status_box_p->set_text_size(15);
    status_box_p->set_background(255, 0, 0);
    status_box_p->update();
  }    
  else
  {
//    if(dump_flag == true)
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
        barcode_status_p->set_text("Scan Pack Barcode");
        barcode_status_p->set_text_size(30); 
        barcode_status_p->set_background(255, 255, 0);
        barcode_status_p->update();
      }
      if( (batch_mode_driver_p->pack_barcode_old==false) && (batch_mode_driver_p->pack_barcode_ok==true) )
      {
        QString msg = "Barcore OK\nSeed Lot: ";
        msg.append(batch_mode_driver_p->seed_lot_barcode);
        barcode_status_p->set_text(msg);
        barcode_status_p->set_text_size(15); 
        barcode_status_p->set_background(0, 255, 0);
        barcode_status_p->update();
      }
      if( (batch_mode_driver_p->pack_barcode_old==false) && (batch_mode_driver_p->pack_barcode_ok==false) )
      {
        QString msg = "Incorrect Barcore\nSeed Lot: ";
        msg.append(batch_mode_driver_p->seed_lot_barcode);
        barcode_status_p->set_text(msg);
        barcode_status_p->set_text_size(15); 
//        barcode_status_p->set_text("Incorrect Barcode");
//        barcode_status_p->set_text_size(30); 
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
  if(batch_mode_driver_p->mode == wait_for_bad_lot_cleanout)
  {
    barcode_status_p->set_text("Remove over-counted seed lot.");
    barcode_status_p->set_text_size(20); 
    barcode_status_p->set_background(255, 0, 0);
    barcode_status_p->update();
  }
    
  old_barcode_mode = batch_mode_driver_p->barcode_mode;
  

      
      
}


/*  
  if( (centre_p->envelope_present==true) && (pack_was_placed==false) && (centre_p->count>0) )
  {
    cout<<"setting pack_was_placed true\n";
    pack_was_placed = true;
  }
  if( (centre_p->envelope_present==false) && (pack_was_placed==true) && (centre_p->count>0) )
  {
    cout<<"setting pack_was_removed true\n";
    pack_was_removed = true;
    pack_was_placed = false;
    if( (dump_flag==false) && (count>0) && (centre_p->cutgate_p->get_state()==CUTGATE_OPEN) && (ss_pack_removed_too_soon==false) )
    {
      
      cout<<"setting ss_pack_removed_too_soon true\n";
      
      ss_pack_removed_too_soon = true;
    }
    if( (dump_flag==true) && (count>0) && (centre_p->cutgate_p->get_state()==CUTGATE_OPEN) && (ss_dump_container_removed_too_soon==false) )
    {
      ss_dump_container_removed_too_soon = true;
    }
  }
  if(ss_pack_removed_too_soon==true)
  {
    if(centre_p->envelope_present==true)
    {
      ss_pack_removed_too_soon = false;
      pack_was_removed = false;
    }
  }
  if( (seed_for_pack_ready==true) && (batch_mode_driver_p->pack_barcode_ok==true) )
  {
    pack_can_be_removed = true;
    seed_for_pack_ready = false;
  }
  
  if(centre_p->envelope_present==true)
  {
    ss_pack_removed_too_soon = false;
    pack_was_removed = false;
    ss_dump_container_removed_too_soon = false;
  }
  if( (dump_container_can_be_removed==true) && (centre_p->envelope_present==false) )
  {
    //new seed lot.  reset dump variables.
    dump_container_can_be_removed = false;
    ss_dump_container_removed_too_soon = false;
    dump_container_was_placed = false;
    cout<<"setting dump_flag false 1 \n";
    dump_flag = false;
    pack_was_removed = false;
    pack_can_be_removed = false;
    ss_pack_removed_too_soon = false;
    pack_was_placed = false;
  }



  if( (pack_can_be_removed==true) && (ss_pack_removed_too_soon==false) && (dump_flag==false) )
  {
    QString number;
    if(batch_mode_driver_p->use_spreadsheet==false)
    {
      pack_ready_message = "PACK READY.\nPackage ";
      pack_ready_message.append(number.setNum(batch_mode_driver_p->pack_ready_pack+1));
      pack_ready_message.append(" of ");
      pack_ready_message.append(number.setNum(batch_mode_driver_p->pack_ready_pack_limit));
      pack_ready_message.append("\n");
      pack_ready_message.append(number.setNum(batch_mode_driver_p->pack_ready_count_limit));
      pack_ready_message.append(" seeds");
    }
    else
    {
      pack_ready_message = "PACK READY.\n";
      pack_ready_message.append(number.setNum(batch_mode_driver_p->lines_left_to_fill));
      pack_ready_message.append(" packs left for \nthis seed lot.");
    }
    status_box_p->set_text(pack_ready_message);
    status_box_p->set_text_size(20);
    status_box_p->set_background(0, 255, 0);
    status_box_p->update();
  }
  else if(ss_pack_removed_too_soon)
  {
    status_box_p->set_text(pack_removed_too_soon_message);
    status_box_p->set_text_size(30);
    status_box_p->set_background(255, 0, 0);
    status_box_p->update();
  }
  else if(dump_container_can_be_removed)
  {
    status_box_p->set_text(dump_container_ready_message);
    status_box_p->set_text_size(30);
    status_box_p->set_background(0, 255, 0);
    status_box_p->update();
  }
  else if(ss_dump_container_removed_too_soon)
  {
    status_box_p->set_text(dump_container_removed_too_soon_message);
    status_box_p->set_text_size(30);
    status_box_p->set_background(255, 0, 0);
    status_box_p->update();
  }
  else if(batch_mode_driver_p->mode == wait_for_bad_lot_cleanout)
  {
    status_box_p->set_text(bad_seed_lot_message);
    status_box_p->set_text_size(15);
    status_box_p->set_background(255, 0, 0);
    status_box_p->update();
  }    
  else
  {
    if(dump_flag == true)
    {
      status_box_p->set_text(dump_container_needed_message);
      status_box_p->set_text_size(30);
      status_box_p->set_background(255, 255, 0);
      status_box_p->update();
    }  
    else
    {
      if(batch_mode_driver_p->use_spreadsheet==false)
      {
        message=QString("Crop: %1.  Sensitivity: %2.\nGate should be set at %3.\nPack %4 of %5.\n%6 seeds/pack.\nfile %7") 
          .arg(centre_p->crops[0].name) 
          .arg(centre_p->crops[0].sensitivity) 
          .arg(centre_p->crops[0].gate_setting)
          .arg(batch_mode_driver_p->current_pack+1)
          .arg(batch_mode_driver_p->current_pack_limit)
          .arg(batch_mode_driver_p->current_count_limit)
          .arg(batch_mode_driver_p->program_path);
      }  
      else
      {
        message=QString("Crop: %1.  Sensitivity: %2.\nGate should be set at %3.\n%4 packs left\nfor this seed lot.\n%5 seeds in this pack.") 
          .arg(centre_p->crops[0].name) 
          .arg(centre_p->crops[0].sensitivity) 
          .arg(centre_p->crops[0].gate_setting)
          .arg(batch_mode_driver_p->lines_left_to_fill)
          .arg(batch_mode_driver_p->current_count_limit);
      }  
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
        barcode_status_p->set_text("Scan Pack Barcode");
        barcode_status_p->set_text_size(30); 
        barcode_status_p->set_background(255, 255, 0);
        barcode_status_p->update();
      }
      if( (batch_mode_driver_p->pack_barcode_old==false) && (batch_mode_driver_p->pack_barcode_ok==true) )
      {
        barcode_status_p->set_text("Barcode OK");
        barcode_status_p->set_text_size(30); 
        barcode_status_p->set_background(0, 255, 0);
        barcode_status_p->update();
      }
      if( (batch_mode_driver_p->pack_barcode_old==false) && (batch_mode_driver_p->pack_barcode_ok==false) )
      {
        barcode_status_p->set_text("Incorrect Barcode");
        barcode_status_p->set_text_size(30); 
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
  if(batch_mode_driver_p->mode == wait_for_bad_lot_cleanout)
  {
    barcode_status_p->set_text("Remove over-counted seed lot.");
    barcode_status_p->set_text_size(20); 
    barcode_status_p->set_background(255, 0, 0);
    barcode_status_p->update();
  }
    
  old_barcode_mode = batch_mode_driver_p->barcode_mode;
  

}
*/
