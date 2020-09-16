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
#include "batch.hpp"
#include "button.hpp"
//#include "batch_mode_driver.hpp"
#include "message_box.hpp"


using namespace std;

void barcode_line::keyPressEvent(QKeyEvent* event)
{
  QLineEdit::keyPressEvent(event); //TEST~~~
  if(event->key() == Qt::Key_Return)
  {
    emit barcode_entered(displayText());  //TEST~~~
    cout<<"emitted barcode_entered("<<displayText().toStdString()<<")\n";
    clear();
  }
}


batch::batch(centre* set_centre_p, batch_mode_driver* set_batch_mode_driver_p)
 :screen(set_centre_p)
{
  batch_mode_driver_p = set_batch_mode_driver_p;

  if (centre_p->crops[0].calibrated == false) 
  {
    centre_p->add_waiting_screen(15);//back here to batch
    centre_p->add_waiting_screen(10);//calibrate
    centre_p->screen_done = true;
    return;
  }
  count_message_p = new QLabel;
  options_button_p = new button("Options");
  back_button_p = new button("Back");
  barcode_line_p = new barcode_line;
  barcode_line_p->setMaximumSize(120,40);  //ORIGINAL~~~
  repeat_pack_button_p = new button("Repeat a pack");
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
  
  table_p = new batch_table(centre_p, batch_mode_driver_p);
  status_box_p = new message_box;
  status_box_p -> setMinimumSize(250,100);
  save_program_button_p = new button("Save Program");
  save_table_button_p = new button("Save Table");
  clear_table_button_p = new button("Clear Table");
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
  top_layout_p -> addWidget(back_button_p, 0, 2);
  bottom_layout_p -> addWidget(barcode_line_p, 1, 4); 
  barcode_line_p -> setText("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");  
  barcode_line_p -> clear();  
  control_layout_p -> addWidget(repeat_pack_button_p, 0, 0);
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
  bottom_layout_p -> addWidget(save_table_button_p, 0, 3);
  bottom_layout_p -> addWidget(clear_table_button_p, 0, 4);
  bottom_layout_p -> addWidget(quit_button_p, 0, 5);
  bottom_layout_p -> addWidget(barcode_status_p, 1, 2, 1, 3);
  main_layout_p -> addWidget(top_box_p, 0, 0, 1, 2);
  main_layout_p -> addWidget(control_box_p, 1, 0);
  main_layout_p -> addWidget(table_p, 1, 1);
  main_layout_p -> addWidget(bottom_box_p, 2, 0, 1, 2);
  
  top_box_p -> setLayout(top_layout_p);
  control_box_p -> setLayout(control_layout_p);
  speed_box_p -> setLayout(speed_layout_p);
  bottom_box_p -> setLayout(bottom_layout_p);
  setLayout(main_layout_p);
  
  connect(options_button_p, SIGNAL(clicked()), this, SLOT(options_clicked()));
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_clicked()));
  connect(repeat_pack_button_p, SIGNAL(clicked()), this, SLOT(repeat_pack_clicked()));
  connect(restart_button_p, SIGNAL(clicked()), this, SLOT(restart_clicked()));
  connect(high_speed_set_p, SIGNAL(valueChanged(int)), batch_mode_driver_p, SLOT(set_high_feed_speed(int)));
  connect(low_speed_set_p, SIGNAL(valueChanged(int)), batch_mode_driver_p, SLOT(set_low_feed_speed(int)));
  connect(dump_speed_set_p, SIGNAL(valueChanged(int)), batch_mode_driver_p, SLOT(set_dump_feed_speed(int)));
  connect(save_program_button_p, SIGNAL(clicked()), this, SLOT(save_program_clicked()));
  connect(save_table_button_p, SIGNAL(clicked()), this, SLOT(save_table_clicked()));
  connect(clear_table_button_p, SIGNAL(clicked()), this, SLOT(clear_table_clicked()));
  connect(quit_button_p, SIGNAL(clicked()), this, SLOT(quit_clicked()));
  connect(barcode_line_p, SIGNAL(barcode_entered(QString)), batch_mode_driver_p, SLOT(barcode_entered(QString)));
  connect(batch_mode_driver_p, SIGNAL(seed_lot_barcode_entered(QString)), table_p, SLOT(enter_seed_lot_barcode(QString)));
  connect(batch_mode_driver_p, SIGNAL(pack_barcode_entered(QString)), table_p, SLOT(enter_pack_barcode(QString)));
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
  end_valve_mode = old_end_valve_mode = closed_empty;
  
  
  pack_ready_message = "Pack Ready";
  pack_removed_too_soon_message = "Pack Not Ready\nPut Back";
  dump_container_needed_message = "Place Dump\nContainer";
  dump_container_ready_message = "Finished Dumping";
  dump_container_removed_too_soon_message = "Dumping Not\nFinished";
  bad_seed_lot_message = "Count went over limit.\nDiscard seed lot.\nReduce feed speed.";
  old_barcode_mode = pack;
  
  end_valve_empty_counter = 0;
  connect(batch_mode_driver_p, SIGNAL(pack_collected(int)), this, SLOT(pack_collected(int)));
  connect(batch_mode_driver_p, SIGNAL(dump_complete(int)), this, SLOT(dump_complete(int)));
  connect(batch_mode_driver_p, SIGNAL(dumping()), this, SLOT(dumping()));
  connect(table_p, SIGNAL(focus_on_barcode()), this, SLOT(focus_on_barcode()));
  connect(batch_mode_driver_p, SIGNAL(bad_lot_signal()), this, SLOT(bad_lot_slot()));
    
  table_p->load_file("settings/batch_backup");
  
  if(batch_mode_driver_p->bm_save_program_filename != "")//returning from batch_save_program screen with name of file to save
  {
    batch_mode_driver_p->save_program(batch_mode_driver_p->bm_save_program_filename);   
    batch_mode_driver_p->bm_save_program_filename = "";
  }
  if(batch_mode_driver_p->bm_save_table_filename != "")//returning from batch_save_table_file screen with name of file to save
  {
    
    cout<<"batch::batch.  save file name = "<<batch_mode_driver_p->bm_save_table_filename.toStdString()<<endl;
    
    //save in user folder under crop name
    table_p->save_file(batch_mode_driver_p->bm_save_table_filename);
    
    //save to any USB memory sticks detected
    QDir* dir_p = new QDir("/media/odroid/");
    QStringList directory_list = dir_p->entryList(QDir::Dirs  | QDir::NoDotAndDotDot);
    int num_drives = directory_list.size();//number of USB memory sticks detected
    for(int i=0; i<num_drives; ++i)
    {
      table_p -> save_file(QString("/media/odroid/").append(directory_list[i]).append("/").append(batch_mode_driver_p->bm_last_table_filename).append(".csv"));
    }

    bool drives_removed = false;
    if(num_drives>0)
    {
      QMessageBox box;
      box.setText(QString("saved to %1 memory sticks").arg(num_drives));
      QPushButton* eject_button = box.addButton("Safely remove memory sticks", QMessageBox::ActionRole);
      QPushButton* keep_button = box.addButton("Do not remove", QMessageBox::ActionRole);
      box.exec();
      
      if(box.clickedButton() == eject_button)
      {
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
        if(check_list.size() == 0) drives_removed = true;
      }
      else if(box.clickedButton() == keep_button)
      {
        //do nothing
      }
    }
    else//no memory sticks detected
    {
      QMessageBox box;
      box.setText("No memory sticks detected.  Saved internally only.");
      box.setInformativeText("If you plug in a memory stick and save again, "
                             "the file will be written to it.  It will be in .csv format, "
                             "and can be opened by a spreadsheet or database.");
      box.exec();
    }
    if(drives_removed == true)
    {
      QMessageBox box;
      box.setText("You can now safely remove the memory stick(s)");
      box.exec();
    }
      
    delete dir_p;  

    batch_mode_driver_p->bm_save_table_filename = "";
  }
  barcode_line_p->setFocus();

  top_layout_p->setContentsMargins(0,0,0,0);        //set layout margins to shrink to designated container dimensions//
  control_layout_p->setContentsMargins(0,0,0,0);
  speed_layout_p->setContentsMargins(0,0,0,0);
  bottom_layout_p->setContentsMargins(0,0,0,0);
  main_layout_p->setContentsMargins(0,0,10,10);
  
  main_layout_p->setRowMinimumHeight(5, 0);
  main_layout_p->setRowStretch(5, 0);
  
  batch_mode_driver_p -> use_spreadsheet = false;
  batch_mode_driver_p -> seed_lot_barcode = "";
  batch_mode_driver_p -> pack_barcode = "";

  count_message_p->setStyleSheet("QLabel {" 
        "background-color: white;"          
        "border: 3px solid black;"          
        "font-size: 20pt;}"); 
        
  repeat_pack_window_p = 0;
  repeat_pack_window_exists = false;
}

batch::~batch()
{
  cout<<"~batch start\n";
  run_timer_p -> stop();
  table_p->save_file(QString("settings/batch_backup"));
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
  delete table_p;
  delete status_box_p;
  delete clear_table_button_p;
  delete quit_button_p;
  delete top_layout_p;
  delete top_box_p;
  delete speed_box_p;
  delete bottom_box_p;
  delete control_layout_p;
  delete main_layout_p;
  cout<<"~batch end\n";
  
  if(repeat_pack_window_exists) delete repeat_pack_window_p;
}

void batch::pack_collected(int count_to_record)
{
  if( (end_valve_mode==closed_bad_lot) || (end_valve_mode==open_emptying_bad_lot) ) return;
  
  table_p->enter_seeds(count_to_record);

  barcode_line_p->setFocus();

//  int row;
//  row = table_p -> model_row;
  
  centre_p->pack_count_str = QString::number(count_to_record);
  
//  QString lotcode_str, packcode_str, batch_count_str, substitution_str, dump_count_str;
  
//  lotcode_str = table_p -> model_p -> item(row-1, 0) -> text();
//  packcode_str = table_p -> model_p -> item(row-1, 1) -> text();
//  batch_count_str = QString::number(count_to_record);
//  substitution_str = table_p -> model_p -> item(row-1, 3) -> text();
  
  centre_p -> communicate_out('p');
}  

void batch::dump_complete(int dump_count)
{
  table_p->enter_dump_count(dump_count);
  
  end_valve_mode = dump_open_empty;
  barcode_line_p->setFocus();
  
//  QString lotcode_str, packcode_str, batch_count_str, substitution_str, dump_count_str;
  centre_p->dump_count_str = QString::number(dump_count);
  centre_p->communicate_out('d');
}

void batch::dumping()
{
  end_valve_mode = dump_closed_empty;
}

void batch::focus_on_barcode()
{
  cout<<"start batch::focus_on_barcode \n";
  barcode_line_p -> setFocus();
}

void batch::bad_lot_slot()
{
  end_valve_mode = closed_bad_lot;
}

void batch::options_clicked()
{
  batch_mode_driver_p -> stop();
  centre_p->set_speed(0);
  centre_p->add_waiting_screen(15);//come back to batch
  centre_p->add_waiting_screen(16);//batch_options
  centre_p->screen_done=true;
}

void batch::back_clicked()
{
  batch_mode_driver_p -> stop();
  centre_p->set_speed(0);
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void batch::repeat_pack_clicked()
{
  repeat_pack_window_p = new repeat_pack_window(batch_mode_driver_p, this);
  connect(repeat_pack_window_p, SIGNAL(destroyed(QObject*)), this, SLOT(focus_on_barcode()));
  connect(batch_mode_driver_p, SIGNAL(send_extra_pack_message(QString)), repeat_pack_window_p, SLOT(set_message(QString)));
  connect(batch_mode_driver_p, SIGNAL(extra_pack_finished_signal()), repeat_pack_window_p, SLOT(cancel_button_clicked()));
  focus_on_barcode();
}

void batch::restart_clicked()
{
  batch_mode_driver_p -> restart();
}

void batch::save_program_clicked()
{
  batch_mode_driver_p -> stop();
  centre_p->set_speed(0);
  centre_p->add_waiting_screen(17);//batch_save_program
  centre_p->screen_done = true;
}

void batch::save_table_clicked()
{
  batch_mode_driver_p -> stop();
  centre_p->set_speed(0);
  centre_p->add_waiting_screen(20);//batch_save_table
  centre_p->screen_done = true;
}

void batch::clear_table_clicked()
{
  table_p->clear();
  barcode_line_p->setFocus();
}

void batch::quit_clicked()
{
  batch_mode_driver_p -> stop();
  centre_p->set_speed(0);
  centre_p->add_waiting_screen(0);
  centre_p->screen_done = true;
}

void batch::run()
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
    case closed_empty:
      if(centre_p->cutgate_p->get_state() == CUTGATE_OPEN)
      {
        end_valve_mode = closed_filling;
      }
      if(centre_p->get_endgate_state() == ENDGATE_OPEN)
      {
        end_valve_mode = opened_while_empty;
      }
      break;
    case opened_while_empty:
      if(centre_p->cutgate_p->get_state() == CUTGATE_OPEN)
      {
        end_valve_mode = open_pass_through;
      }
      if(centre_p->get_endgate_state() == ENDGATE_CLOSED)
      {
        end_valve_mode = pack_removed_too_soon;
      }
      break;
    case closed_filling:
      if(centre_p->cutgate_p->get_state() == CUTGATE_CLOSED)
      {
        end_valve_mode = closed_full;
      }
      if(centre_p->get_endgate_state() == ENDGATE_OPEN)
      {
        end_valve_mode = open_pass_through;
      }
      break;
    case closed_full:
      if(centre_p->cutgate_p->get_state() == CUTGATE_OPEN)//not expected
      {
        cout<<"cutgate opened in end_valve_mode closed_full\n";
      }
      if(centre_p->get_endgate_state() == ENDGATE_OPEN)
      {
        end_valve_mode = open_emptying;
        end_valve_empty_counter = 0;
      }
      break;
    case open_emptying:
      if(end_valve_empty_counter >= 5)//0.5 sec.
      {
        end_valve_mode = open_empty;
      }
      if(centre_p->get_endgate_state() == ENDGATE_CLOSED)
      {
        end_valve_mode = pack_removed_too_soon;
      }
      break;
    case open_pass_through:
      if(centre_p->cutgate_p->get_state() == CUTGATE_CLOSED)
      {
        end_valve_mode = open_emptying;
        end_valve_empty_counter = 0;
      }
      if(centre_p->get_endgate_state() == ENDGATE_CLOSED)
      {
        end_valve_mode = pack_removed_too_soon;
      }
      break;
    case open_empty:
      if(centre_p->cutgate_p->get_state() == CUTGATE_OPEN)//not expected
      {
        cout<<"cutgate opened in end_valve_mode open_empty\n";
      }
      if(centre_p->get_endgate_state() == ENDGATE_CLOSED)
      {
        end_valve_mode = closed_empty;
        batch_mode_driver_p->pack_complete = true;
      }
      break;
    case pack_removed_too_soon:
      if(centre_p->get_endgate_state() == ENDGATE_OPEN)
      {
        if(centre_p->cutgate_p->get_state() == CUTGATE_OPEN)
        {
          end_valve_mode = open_pass_through;
        }
        else
        {
          end_valve_mode = open_emptying;
          end_valve_empty_counter = 0;
        }
      }
      break;
    case closed_bad_lot:
      if(centre_p->cutgate_p->get_state() == CUTGATE_OPEN)//not expected
      {
        cout<<"cutgate opened in end_valve_mode closed_bad_lot\n";
      }
      if(centre_p->get_endgate_state() == ENDGATE_OPEN)
      {
        end_valve_mode = open_emptying_bad_lot;
      }
      break;
    case open_emptying_bad_lot:
      if(centre_p->cutgate_p->get_state() == CUTGATE_OPEN)//not expected
      {
        cout<<"cutgate opened in end_valve_mode open_emptying_bad_lot\n";
      }
      if(centre_p->get_endgate_state() == ENDGATE_CLOSED)
      {
        end_valve_mode = closed_empty;
      }
      break;
    case dump_closed_empty:
      if(centre_p->cutgate_p->get_state() == CUTGATE_OPEN)
      {
        end_valve_mode = dump_closed_filling;
      }
      if(centre_p->get_endgate_state() == ENDGATE_OPEN)
      {
        end_valve_mode = dump_opened_while_empty;
      }
      break;
    case dump_opened_while_empty:
      if(centre_p->cutgate_p->get_state() == CUTGATE_OPEN)
      {
        end_valve_mode = dump_pass_through;
      }
      if(centre_p->get_endgate_state() == ENDGATE_CLOSED)
      {
        end_valve_mode = dump_container_removed_too_soon;
      }
      break;
    case dump_closed_filling:
      if(centre_p->cutgate_p->get_state() == CUTGATE_CLOSED)//not expected
      {
        cout<<"cutgate closed in end_valve_mode dump_closed_filling\n";
      }
      if(centre_p->get_endgate_state() == ENDGATE_OPEN)
      {
        end_valve_mode = dump_pass_through;
      }
      break;
    case dump_pass_through:
      if(centre_p->cutgate_p->get_state() == CUTGATE_CLOSED)//not expected
      {
        cout<<"cutgate closed in end_valve_mode dump_pass_through\n";
      }
      if(centre_p->get_endgate_state() == ENDGATE_CLOSED)
      {
        end_valve_mode = dump_container_removed_too_soon;
      }
      break;
    case dump_open_empty:
      if(centre_p->cutgate_p->get_state() == CUTGATE_CLOSED)//not expected
      {
        cout<<"cutgate closed in end_valve_mode dump_open_empty\n";
      }
      if(centre_p->get_endgate_state() == ENDGATE_CLOSED)
      {
        end_valve_mode = closed_filling;
      }
      break;
    case dump_container_removed_too_soon:
      if( (centre_p->cutgate_p->get_state()==CUTGATE_CLOSED) && (centre_p->get_endgate_state()==ENDGATE_OPEN) )
      {
        end_valve_mode = dump_opened_while_empty;
      }
      if( (centre_p->cutgate_p->get_state()==CUTGATE_OPEN) && (centre_p->get_endgate_state()==ENDGATE_OPEN) )
      {
        end_valve_mode = dump_pass_through;
      }
      break;
    default:
      cout<<"end_valve_mode not found\n";
  }
      
  if( (end_valve_mode==closed_full) || (end_valve_mode==open_emptying) || (end_valve_mode==open_empty) )
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
  else if(end_valve_mode==pack_removed_too_soon)
  {
    status_box_p->set_text(pack_removed_too_soon_message);
    status_box_p->set_text_size(30);
    status_box_p->set_background(255, 0, 0);
    status_box_p->update();
  }
  else if(end_valve_mode==dump_open_empty)
  {
    status_box_p->set_text(dump_container_ready_message);
    status_box_p->set_text_size(30);
    status_box_p->set_background(0, 255, 0);
    status_box_p->update();
  }
  else if(end_valve_mode==dump_container_removed_too_soon)
  {
    status_box_p->set_text(dump_container_removed_too_soon_message);
    status_box_p->set_text_size(30);
    status_box_p->set_background(255, 0, 0);
    status_box_p->update();
  }
  else if( (end_valve_mode==closed_bad_lot) || (end_valve_mode==open_emptying_bad_lot) )
  {
    status_box_p->set_text(bad_seed_lot_message);
    status_box_p->set_text_size(15);
    status_box_p->set_background(255, 0, 0);
    status_box_p->update();
  }    
  else
  {
    if( (end_valve_mode==dump_closed_filling) || (end_valve_mode==dump_pass_through) || (end_valve_mode==dump_open_empty) )
    {
      status_box_p->set_text(dump_container_needed_message);
      status_box_p->set_text_size(30);
      status_box_p->set_background(255, 255, 0);
      status_box_p->update();
    }  
    else
    {
      message=QString("Crop: %1.  Sensitivity: %2.\nGate should be set at %3.\nPack %4 of %5.\n%6 seeds/pack.\nfile %7") 
        .arg(centre_p->crops[0].name) 
        .arg(centre_p->crops[0].sensitivity) 
        .arg(centre_p->crops[0].gate_setting)
        .arg(batch_mode_driver_p->current_pack+1)
        .arg(batch_mode_driver_p->current_pack_limit)
        .arg(batch_mode_driver_p->current_count_limit)
        .arg(batch_mode_driver_p->program_path);
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

repeat_pack_window::repeat_pack_window(batch_mode_driver* batch_mode_driver_p_s, batch* batch_p_s)
:QWidget()
{
  batch_mode_driver_p = batch_mode_driver_p_s;
  batch_p = batch_p_s;
  
  seeds_to_count = batch_mode_driver_p->current_count_limit;
  cancel_button_p = new button("Cancel");
  entry_button_p = new button("");
  QString entry_button_text("Press to count Extra Pack of %1 Seeds");
  entry_button_text = entry_button_text.arg(seeds_to_count);
  entry_button_p -> setText(entry_button_text);
  message_p = new QLabel("Or enter another value");
  keypad_p = new keypad;
  barcode_line_p = new barcode_line;
  
  connect(cancel_button_p, SIGNAL(clicked()), this, SLOT(cancel_button_clicked()));
  connect(entry_button_p, SIGNAL(clicked()), this, SLOT(entry_button_clicked()));
  connect(keypad_p, SIGNAL(number_entered(int)), this, SLOT(number_entered(int)));
  connect(barcode_line_p, SIGNAL(barcode_entered(QString)), batch_mode_driver_p, SLOT(barcode_entered(QString)));

  
  layout_p = new QVBoxLayout;
  layout_p -> addWidget(cancel_button_p);
  layout_p -> addWidget(entry_button_p);
  layout_p -> addWidget(message_p);
  layout_p -> addWidget(keypad_p);
  layout_p -> addWidget(barcode_line_p);
  setLayout(layout_p);
  
  barcode_line_p -> setFocus();
  
  setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
  show();
}
  
void repeat_pack_window::set_message(QString message)
{
  message_p->setText(message);  
}
  
void repeat_pack_window::entry_button_clicked()
{
  number_entered(seeds_to_count);
  barcode_line_p -> setFocus();  
}

void repeat_pack_window::cancel_button_clicked()
{
  batch_p->repeat_pack_window_exists = false;
  deleteLater();
}

void repeat_pack_window::number_entered(int val)
{
  batch_mode_driver_p -> extra_pack_count_limit = val;
  batch_mode_driver_p -> fill_extra_pack = true;
  barcode_line_p -> setFocus();
}
