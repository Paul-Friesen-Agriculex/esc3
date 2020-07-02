#include <fstream>
#include <iostream>
#include <QWidget>
#include <QString>
#include <QTimer>
#include <QMetaType>
#include <QThread>
#include <QTextStream>
#include <QList>  //add for modifying specific position in file// 11_14_2018
#include <QTcpServer>
#include <QTcpSocket>
//#include <QLabel>
#include "message_box.hpp"
#include <time.h>
#include <fcntl.h>	//library used to use system call command "open()" used to check available serial
#include <unistd.h>	//library to enable write() function

#include "centre.hpp"
#include "processor.hpp"

#include "start_screen.hpp"
#include "crop_choice.hpp"
#include "crop_edit.hpp"
#include "crop_delete.hpp"
#include "keyboard.hpp"
#include "totalize.hpp"
#include "totalize_options.hpp"
#include "playback.hpp"
#include "tool_choice.hpp"
#include "calibrate.hpp"
#include "gateset.hpp"
#include "program_choice.hpp"
#include "enter_program.hpp"
#include "batch.hpp"
#include "batch_mode_driver.hpp"
#include "batch_options.hpp"
#include "program_delete.hpp"
//#include "batch_save_file.hpp"
//#include "delete_program.hpp"
#include "crop_name_exists.hpp"
#include "diagnostics_console.hpp"
#include "spreadsheet_choice.hpp"
#include "ss_setup_choice.hpp"
#include "ss_setup_entry.hpp"
//#include "envelope_layout_choice.hpp"
#include "set_envelope_size.hpp"

#include "ss_setup_delete.hpp"
#include "ss_options.hpp"
#include "ss_column_display_options.hpp"
#include "ss_batch.hpp"
#include "select_envelope_field.hpp"
#include "position_envelope_field.hpp"
#include "select_field_data_source.hpp"
#include "macro_screen.hpp"	//TEST~~~
#include "communications_menu.hpp"
#include "tcp_server_addr_choice.hpp"
#include "tcp_client_server_addr_entry.hpp"
//#include "table.hpp"        //TEST~~~ //access barcodes and count from table
#include "brother_envelope_feeder.hpp"
#include "signal_port.hpp"


Q_DECLARE_METATYPE(crop)


using namespace std;

//extern const int image_lines = 32;
extern const int image_lines = 64;
//extern const int image_lines = 128;
//extern const int image_lines = 256;
//extern const int image_lines = 512;
//extern const int image_lines = 1024;
//extern const int image_lines = 2048;

extern const int line_length = 2048;

//extern const int images_to_record = 128;
extern const int images_to_record = 512;
//extern const int images_to_record = 1500;

centre::centre():
  QObject()
{
  qRegisterMetaType<crop>();
  
//  startup_progress_label_p = new QLabel("Starting ESC-3 Program");
//  startup_progress_label_p = new message_box;
//  startup_progress_label_p -> set_text("Starting ESC-3 Program");
//  startup_progress_label_p -> show();
//  startup_progress_label_p -> update();
  
  init_ran = false;

  run_timer_p = new QTimer;
  connect(run_timer_p, SIGNAL(timeout()), this, SLOT(run()));
  run_timer_p->start(1000);
}

void centre::init()
{
  cout<<"before processor_thread_p\n";
  
  processor_p = new processor();
  processor_thread_p = new QThread(this);
  processor_p -> moveToThread(processor_thread_p);
  
//  startup_progress_label_p ->setText("Done Camera and Processor Setup");
  
  cout<<"after processor creation\n";
  
  connect(this, SIGNAL(set_camera_processing(bool)), processor_p, SLOT(set_camera_processing(bool)), Qt::BlockingQueuedConnection);
  connect(this, SIGNAL(playback_line()), processor_p, SLOT(playback_line()), Qt::QueuedConnection);
  connect(this, SIGNAL(get_qimage()), processor_p, SLOT(get_qimage()), Qt::QueuedConnection);
  connect(this, SIGNAL(set_to_record(bool)), processor_p, SLOT(set_to_record(bool)), Qt::QueuedConnection);
  connect(this, SIGNAL(set_crop(crop)), processor_p, SLOT(set_crop(crop)), Qt::QueuedConnection);
  connect(this, SIGNAL(processor_display_blobs(bool)), processor_p, SLOT(show_blob_on_terminal(bool)), Qt::QueuedConnection);
  connect(this, SIGNAL(restart_calibration_signal()), processor_p, SLOT(restart_calibration()), Qt::QueuedConnection);
  connect(this, SIGNAL(save_image_signal(QString)), processor_p, SLOT(save_image(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(load_image_signal(QString)), processor_p, SLOT(load_image(QString)), Qt::QueuedConnection);
  
  connect(processor_p, SIGNAL(count_out(int)), this, SLOT(increase_count(int)), Qt::QueuedConnection);
  connect(processor_p, SIGNAL(send_qimage(QImage)), this, SLOT(receive_qimage(QImage)), Qt::QueuedConnection);
  connect(processor_p, SIGNAL(send_calibrated_crop(crop)), this, SLOT(receive_calibrated_crop(crop)), Qt::QueuedConnection);
  connect(processor_p, SIGNAL(end_of_playback()), this, SLOT(end_of_playback()), Qt::QueuedConnection);
  connect(processor_p, SIGNAL(send_cycle_time(int)), this, SLOT(get_cycle_time(int)), Qt::QueuedConnection);
  
  processor_thread_p -> start();
  emit set_camera_processing(true);

  cutgate_p = new cutgate;
  endgate_p = new endgate(this);
  signal_port_pulse_when_endgate_closes = true;
  envelope_sensor_p = new envelope_sensor;
  feeder_p = new feeder;
  brother_envelope_feeder_p = new brother_envelope_feeder;
  for(int i=0; i<screen_wait_list_size-1; ++i)
  {
    screen_wait_list[i]=0;
  }
  screen_p=0;
  
  for(int i=0; i<100; ++i)
  {
    user_permission_levels[i]=0;
  }
  default_permission_level=5;
  screen_done=true;
  
  for(int i=0;i<100;++i)
  {
    crops[i].name = QString("");
    crops[i].sensitivity=0;
    crops[i].gate_setting=0;
    crops[i].area_mean=0;
    crops[i].area_stdev=0;
    crops[i].max_inflection_1=0;
    crops[i].max_inflection_3=0;
    crops[i].max_inflection_9=0;
    crops[i].calibrated=0;
    crops[i].high_feed_speed=0;
    crops[i].low_feed_speed=0;
    crops[i].dump_speed=0;
  }

  ifstream f("crop_file");
  char name_string[30];
  
  for(int i=0;i<100;++i)
  {
    f.getline(name_string,29);
    crops[i].name = QString(name_string);
    f>>crops[i].sensitivity;
    f>>crops[i].gate_setting;
    f>>crops[i].area_mean;
    f>>crops[i].area_stdev;
    f>>crops[i].max_inflection_1;
    f>>crops[i].max_inflection_3;
    f>>crops[i].max_inflection_9;
    f>>crops[i].calibrated;
//    f.get();
    f>>crops[i].high_feed_speed;
    f>>crops[i].low_feed_speed;
    f>>crops[i].dump_speed;
    f.get();
    if(f.eof()) break;
  }
  f.close();

//  cout<<"crops after load in centre::centre\n";
//  for(int i=0;i<100;++i)
//  {
//    cout<<i<<"     "<<crops[i].name.toStdString()<<endl;
//  }
  
  
  crop_list_changed = false;
  
  cutgate_p -> open();
  set_endgate_state(ENDGATE_CLOSED);
  
  for(previous_screen_index=0; previous_screen_index<10; ++previous_screen_index)
  {
    previous_screen_list[previous_screen_index] = 0;
  }
  previous_screen_index=0;
  previous_screen_going_back=false;
  changing_existing_crop = false;
  recalibrating = false;
  
  tm_zero_when_seed_discharged = true;
  tm_autosave_count = 0;
  tm_save_filename = "";
  
  totalize_force_endgate_open = false;
  end_of_playback_flag = false;
  measured_line_frequency = 4700;
  
  batch_mode_driver_p = new batch_mode_driver(this, cutgate_p);
  
  //totalize mode
  tm_barcode_columns = 0;
//  tm_zero_when_seed_discharged = false;
  tm_autosave_count_limit = 0;//after this many counts are recorded, autosaves the file
  tm_autosave_count = 0;//counts how many counts were recorded;
  tm_save_filename = "";
  block_endgate_opening = false;//true prevents endgate from opening.  Used if barcode test fails in batch.
  communicate_by_keyboard_cable = true;
  communicate_by_tcp = false;
//  tcp_server = true;
  network = 0;//0-> not set.  1->use 192.168.100.1.  2->use 192.168.200.1.
  QString tcp_client_server_addr = "";
  tcp_server_p = new QTcpServer;
  tcp_socket_p = new QTcpSocket;

  load_settings("default");
  
  diagnostics_console_p = new diagnostics_console(this);
  diagnostics_console_p -> show();

  connect(processor_p, SIGNAL(send_message(QString)), diagnostics_console_p, SLOT(receive_message1(QString)));
  connect(batch_mode_driver_p, SIGNAL(send_message2(QString)), diagnostics_console_p, SLOT(receive_message2(QString)));
  connect(batch_mode_driver_p, SIGNAL(send_message_time_to_end(QString)), diagnostics_console_p, SLOT(receive_message3(QString)));
  connect(diagnostics_console_p, SIGNAL(reset_time_tests_signal()), processor_p, SLOT(reset_time_tests()));
  connect(cutgate_p, SIGNAL(closed_while_opening()), batch_mode_driver_p, SLOT(cutgate_timing_error()));
  connect(cutgate_p, SIGNAL(opened_while_closing()), batch_mode_driver_p, SLOT(cutgate_timing_error()));
  connect(tcp_server_p, SIGNAL(newConnection()), this, SLOT(tcp_connection_detected())); 
  connect(tcp_socket_p, SIGNAL(connected()), this, SIGNAL(tcp_connection_detected_signal())); 
//  connect(batch_mode_driver_p->count_rate_predictor_p, SIGNAL(send_message(QString)), diagnostics_console_p, SLOT(receive_message3(QString)));
  
  tm_macro_updated = 0; 
  count=0;
  new_keyboard_entry=false;
  for(int i=0; i<10; ++i) control_int[i] = 0;
  
//  delete startup_progress_label_p;
//  startup_progress_label_p = 0;
}

centre::~centre()
{
  delete diagnostics_console_p;

  if(screen_p) 
  {
    delete screen_p;
    screen_p = 0;
  }
  if(brother_envelope_feeder_p)
  {
	delete brother_envelope_feeder_p;
	brother_envelope_feeder_p = 0;
  }
  emit set_camera_processing(false);//cause camera thread's run() function to return
  clock_t sleep_until = clock() + 1000;//to let run function return
  while(clock()<sleep_until)
  {
  }
  processor_thread_p -> quit();
  processor_thread_p -> wait();
  delete processor_p;
  processor_p = NULL;
  delete processor_thread_p;
  processor_thread_p = NULL;
  std::ofstream f("crop_file",std::ofstream::out);
  int i;
  for(i=0;i<100;++i)
  {
    f<<(crops[i].name).toStdString()<<"\n";
    f<<crops[i].sensitivity<<" ";
    f<<crops[i].gate_setting<<" ";
    f<<crops[i].area_mean<<" ";
    f<<crops[i].area_stdev<<" ";
    f<<crops[i].max_inflection_1<<" ";
    f<<crops[i].max_inflection_3<<" ";
    f<<crops[i].max_inflection_9<<" ";
    f<<crops[i].calibrated;
    f<<endl;
    f<<crops[i].high_feed_speed<<" ";
    f<<crops[i].low_feed_speed<<" ";
    f<<crops[i].dump_speed;
    f<<endl;
  }
  f.close();
  save_settings("default");
}

void centre::increase_count(int to_add)
{
  count += to_add;
//  batch_mode_driver_p->count_rate_predictor_p->add_counts(to_add, feed_speed);
}

void centre::receive_qimage(QImage qimage_set)
{
  qimage = qimage_set;
}

void centre::receive_calibrated_crop(crop calibrated_crop)
{
//  cout<<"\ncentre::receive_calibrated_crop.\n";
//  cout<<"  calibrated_crop.name="<<calibrated_crop.name.toStdString()<<endl;
  
  crops[0] = calibrated_crop;
  
//  cout<<"crops[0].calibrated="<<crops[0].calibrated<<endl;
//  cout<<"end centre::receive_calibrated_crop\n\n";
}

void centre::end_of_playback()
{
  end_of_playback_flag = true;
}

void centre::get_cycle_time(int value)//value is msec.
{
  if(value>0) measured_line_frequency = float(image_lines)/float(value)*1000.0;
}

void centre::tcp_connection_detected()
{
  
  cout<<"centre::tcp_connection_detected()\n";
  
  tcp_socket_p = tcp_server_p->nextPendingConnection();
  emit tcp_connection_detected_signal();
}

void centre::run()
{
  
  cout<<"centre::run.  new_keyboard_entry = "<<new_keyboard_entry<<"\n";
  
  if(init_ran == false)
  {
    init();
    init_ran = true;
  }
    
  emit set_crop(crops[0]);
//  cutgate_state = cutgate_p->get_state();
  endgate_state = endgate_p->get_state();
  envelope_present = envelope_sensor_p->read();
  brother_envelope_feeder_p -> run();

  if(current_screen==5)//totalize
  {
    if(endgate_state == ENDGATE_CLOSED)
    {
      if(totalize_force_endgate_open==true || envelope_present==true)
      {
//        endgate_p -> open();
        set_endgate_state(ENDGATE_OPEN);
      }
    }
    else //endgate is open
    {
      if(totalize_force_endgate_open==false && envelope_present==false)
      {
//        endgate_p -> close();
        set_endgate_state(ENDGATE_CLOSED);
      }
    }
  }
  /*
  else if(current_screen==15)//batch
  {
    
//    cout<<"screen 15.  endgate_state "<<endgate_state<<endl;
    
    if(endgate_state == ENDGATE_CLOSED)
    {
      if(batch_mode_driver_p->force_endgate_open==true || envelope_present==true)
      {
//        cout<<"block_endgate_opening "<<block_endgate_opening<<endl;
//        if (block_endgate_opening == false) endgate_p -> open();
        if (block_endgate_opening == false) 
        {
          cout<<"opening endgate\n";
          set_endgate_state(ENDGATE_OPEN);
        }
      }
    }
    else //endgate is open
    {
      if(batch_mode_driver_p->force_endgate_open==false && envelope_present==false)
      {
//        endgate_p -> close();
        set_endgate_state(ENDGATE_CLOSED);
      }
    }
  }
  */
  else if( (current_screen==15) || (current_screen==33) )//batch or ss_batch
  {
    if(endgate_state == ENDGATE_CLOSED)
    {
      if(envelope_present==true &&   (block_endgate_opening==false || batch_mode_driver_p->release_pack==true)    )
      {
        batch_mode_driver_p->release_pack = false;
        set_endgate_state(ENDGATE_OPEN);
      }
    }
    else //endgate is open
    {
      if(envelope_present==false)
      {
        set_endgate_state(ENDGATE_CLOSED);
      }
    }
  }
  else //current screen is not totalize or batch
  {
    if(endgate_state == ENDGATE_CLOSED)
    {
      if(envelope_present==true)
      {
//        endgate_p -> open();
        set_endgate_state(ENDGATE_OPEN);
      }
    }
    else //endgate is open
    {
      if(envelope_present==false)
      {
//        endgate_p -> close();
        set_endgate_state(ENDGATE_CLOSED);
      }
    }
  }
  if(screen_done)
  {
    
//    cout<<"screen_done was true\n";
    
      
    cout<<"start screen_done.  new_keyboard_entry = "<<new_keyboard_entry<<endl;
      
    screen_done=false;
    if(previous_screen_going_back == false)//add the current screen to previous_screen_list
    {
      
//      cout<<"adding to previous_scree_list "<<current_screen<<endl;
      
      previous_screen_index++;
      if(previous_screen_index >=10)
      {
        for(int i=0; i<9; ++i)
        {
          previous_screen_list[i] = previous_screen_list[i+1];
        }
        previous_screen_index = 9;
      }
      previous_screen_list[previous_screen_index] = current_screen;
//      cout<<"done adding to previous_screen_list\n";
    }
    current_screen = screen_wait_list[0];
    previous_screen_going_back = false;//will be set true if get_previous_screen is called again
    for(int i=0; i<screen_wait_list_size-1; ++i)
    {
      screen_wait_list[i]=screen_wait_list[i+1];
    }
    screen_wait_list[screen_wait_list_size-1]=0;
    
//    cout<<"deleting screen_p\n";
    
//    delete screen_p;
    screen* old_screen_p = screen_p;
    
    cout<<"current_screen="<<current_screen<<endl;
    
    switch(current_screen)
    {
      case  0: screen_p=new start_screen(this); break;
      case  1: screen_p=new crop_choice(this); break;
//      case  2: screen_p=new gate_set(this); break;
      case  3: screen_p=new crop_edit(this); break;
      case  4: screen_p=new crop_name_entry(this); break;
      case  5: screen_p=new totalize(this); break;
      case  6: screen_p=new totalize_options(this); break;
      case  7: screen_p=new crop_delete(this); break;
      case  8: screen_p=new playback(this); break;
      case  9: screen_p=new tool_choice(this); break;
      case 10: screen_p=new calibrate(this); break;
      case 11: screen_p=new gateset(this); break;
      case 12: screen_p=new totalize_save_file(this); break;
      case 13: screen_p=new program_choice(this, batch_mode_driver_p); break;
      case 14: screen_p=new enter_program(this, batch_mode_driver_p); break;
      case 15: screen_p=new batch(this, batch_mode_driver_p); break;
      case 16: screen_p=new batch_options(this, batch_mode_driver_p); break;
      case 17: screen_p=new batch_save_program(this, batch_mode_driver_p); break;
      case 18: screen_p=new program_delete(this, batch_mode_driver_p); break;
      case 19: screen_p=new crop_name_exists(this); break;
      case 20: screen_p=new batch_save_table(this, batch_mode_driver_p); break;
      case 21: screen_p=new spreadsheet_choice(this, batch_mode_driver_p); break;
      case 22: screen_p=new ss_setup_choice(this, batch_mode_driver_p); break;
      case 23: screen_p=new ss_setup_entry(this, batch_mode_driver_p); break;
//      case 24: screen_p=new envelope_layout_choice(this, batch_mode_driver_p); break;
      case 25: screen_p=new set_envelope_size(this, batch_mode_driver_p); break;
//      case : screen_p=new (this); break;
//      case : screen_p=new (this); break;
//      case : screen_p=new (this); break;
//      case : screen_p=new (this); break;
//      case : screen_p=new (this); break;
      case 27: screen_p=new macro_name_entry(this);  break;    //TEST~~~ 11_13_2018//
      case 28: screen_p=new macro_screen(this); break;	    //TEST~~~ macro_menu
      case 29: screen_p=new batch_save_ss_setup(this, batch_mode_driver_p); break;
      case 30: screen_p=new ss_setup_delete(this, batch_mode_driver_p); break;
      case 31: screen_p=new ss_options(this, batch_mode_driver_p); break;
      case 32: screen_p=new ss_column_display_options(this, batch_mode_driver_p); break;
      case 33: screen_p=new ss_batch(this, batch_mode_driver_p); break;
      case 34: screen_p=new select_envelope_field(this, batch_mode_driver_p); break;
      case 35: screen_p=new position_envelope_field(this, batch_mode_driver_p); break;
      case 36: screen_p=new select_field_data_source(this, batch_mode_driver_p); break;
      case 37: screen_p=new enter_field_text(this, batch_mode_driver_p); break;
//      case : screen_p=new (this); break;
//      case : screen_p=new (this); break;
      case 40: screen_p=new communications_menu(this); break;
      case 41: screen_p=new tcp_server_addr_choice(this); 
//               cout<<"before connect\n";
//               connect(tcp_server_p, SIGNAL(newConnection()), screen_p, SLOT(connection_detected())); 
//               cout<<"after connect\n";
               break;
      case 42: screen_p=new tcp_client_server_addr_entry(this);
//               connect(tcp_socket_p, SIGNAL(connected()), screen_p, SLOT(connection_detected())); 
               break;
//      case : screen_p=new (this); break;
//      case : screen_p=new (this); break;
//      case : screen_p=new (this); break;
//      case : screen_p=new (this); break;
//      case : screen_p=new (this); break;
      case 100 : screen_p=new keyboard(this); break;
      default: screen_p=new start_screen(this);
               cout<<"default screen_p case\n";
    }
    screen_p->show();
//    cout<<"centre::run 1\n";
    delete old_screen_p;
//    cout<<"centre::run 2\n";
    old_screen_p = 0;
  }
}

bool centre::save_settings(QString file_name)
{
  QString f_path = QString("settings/");
  f_path += file_name;
  std::ofstream fset(f_path.toLatin1().data(),std::ofstream::out);
  if(!fset)
  {
    cout<<"settings file not open for write\n";
    return false;
  }
  fset<<totalize_feed_speed<<endl;
  fset<<crops[0].name.toLatin1().data()<<endl;
  fset<<tm_barcode_columns<<endl;
  fset<<tm_zero_when_seed_discharged<<endl;
  fset<<tm_autosave_count_limit<<endl;
  fset<<tm_last_filename.toStdString()<<endl;
  fset<<batch_mode_driver_p->bm_last_table_filename.toStdString()<<endl;
  fset<<communicate_by_keyboard_cable<<endl;
  fset<<communicate_by_tcp<<endl;

  fset.close();
  return true;
}

bool centre::load_settings(QString file_name)
{
  QString f_path = QString("settings/");
  f_path += file_name;
  std::ifstream fset(f_path.toLatin1().data(),std::ofstream::out);
  if(!fset)
  {
    cout<<"settings file not open for read\n";
    return false;
  }
  char input[256];

  fset.getline(input, 100);
  totalize_feed_speed = atoi(input);
  
  fset.getline(input, 100);
  cout<<"input="<<input<<endl;
  QString crop_name = QString(input);
  for(int i=0; i<100; ++i)
  {
    if( (input==crops[i].name) && (crops[i].name.isEmpty()==false) )
    {
      select_crop(i);
      break;
    }
  }
  
  fset.getline(input, 100);
  tm_barcode_columns = atoi(input);
  
  fset.getline(input, 100);
  tm_zero_when_seed_discharged = atoi(input);

  fset.getline(input, 100);
  tm_autosave_count_limit = atoi(input);

  fset.getline(input, 100);
  tm_last_filename = QString(input);

  fset.getline(input, 100);
  batch_mode_driver_p->bm_last_table_filename = QString(input);

  fset.getline(input, 100);
  communicate_by_keyboard_cable = atoi(input);
  
  fset.getline(input, 100);
  communicate_by_tcp = atoi(input);
  
  fset.close();
  return true;
}

void centre::add_waiting_screen(int screen_to_add)
{
  
  cout<<"add_waiting_screen("<<screen_to_add<<")\n";
//  cout<<"screen_wait_list_size="<<screen_wait_list_size<<endl;
  
  for(int i=screen_wait_list_size-1; i>0; --i)
  {
//    cout<<"centre::add_waiting_screen.  i="<<i<<endl;
    screen_wait_list[i]=screen_wait_list[i-1];
  }
  screen_wait_list[0]=screen_to_add;
//  cout<<"end of add_waiting_screen\n";
}

int centre::get_previous_screen()
{
  previous_screen_going_back = true;
  if(previous_screen_index>0)
  {
    int ret_val = previous_screen_list[previous_screen_index];
    --previous_screen_index;
    return ret_val;
  }
  else return(0);
}

void centre::select_crop(int crop_index)//moves the selected crop to position 0 in the array.  Crop in position 0 becomes the current crop.
{
  if(crop_index < 0)
  {
    cout<<"centre::select_crop.  negative value for crop_index\n";
    return;
  }
  else if(crop_index >= 100)
  {
    cout<<"centre::select_crop.  crop_index out of bounds\n";
    return;
  }
  else
  {
    crop_list_changed = true;
    crop temp_crop;
    int i;
    temp_crop = crops[crop_index];
    for(i=crop_index; i>0; --i)
    {
      crops[i] = crops[i-1];
    }
    crops[0] = temp_crop;
  }
}

void centre::delete_crop(int crop_index)
{
  if(crop_index < 0)
  {
    cout<<"centre::delete_crop.  negative value for crop_index\n";
    return;
  }
  else if(crop_index >= 100)
  {
    cout<<"centre::delete_crop.  crop_index out of bounds\n";
    return;
  }
  else
  {
    for(int i=crop_index; i<99; ++i)
    {
      crops[i] = crops[i+1];
    }
    crops[99].name = QString("");
    crops[99].sensitivity=0;
    crops[99].gate_setting=0;
    crops[99].area_mean=0;
    crops[99].area_stdev=0;
    crops[99].max_inflection_1=0;
    crops[99].max_inflection_3=0;
    crops[99].max_inflection_9=0;
    crops[99].calibrated=0;
  }
}
/*  
void centre::set_cutgate_state(CUTGATE_STATE set_state)
{
  if(set_state==CUTGATE_OPEN)
  {
    cutgate_p->open();
    cutgate_state = CUTGATE_OPEN;
  }
  else
  {
    cutgate_p->close();
    cutgate_state = CUTGATE_CLOSED;
  }
}
*/
void centre::set_endgate_state(ENDGATE_STATE set_state)
{
  if(set_state==ENDGATE_OPEN)
  {
    endgate_state = ENDGATE_OPEN;
    endgate_p->open();
  }
  else
  {
    endgate_state = ENDGATE_CLOSED;
    endgate_p->close();
  }
}
/*
CUTGATE_STATE centre::get_cutgate_state()
{
  return cutgate_state;
}
*/
ENDGATE_STATE centre::get_endgate_state()
{
  return endgate_state;
}

void centre::set_speed(int speed_set)
{
  feeder_p->set_speed(speed_set);
  feed_speed=speed_set;
}


void centre::set_camera_processing_f(bool state)
{
  emit set_camera_processing(state);
}

void centre::playback_line_f()
{
  emit playback_line();
}

void centre::get_qimage_f()
{
  emit get_qimage();
}

void centre::set_to_record_f(bool record_now)
{
  emit set_to_record(record_now);
}

void centre::set_crop_f(crop current_crop_set)
{
  emit set_crop(current_crop_set);
}

void centre::processor_display_blobs_f(bool value)
{
  emit processor_display_blobs(value);
}

void centre::restart_calibration_f()
{
  cout<<"centre::restart_calibration_f\n";
  emit restart_calibration_signal();
}

void centre::save_image(QString filename)
{
  emit save_image_signal(filename);
}

void centre::load_image(QString filename)
{
  emit load_image_signal(filename);
}

float centre::dust_streak_percentage() 
{
  return processor_p->dust_streak_percentage();
}








//modified to handle barcodes as characters instead of integers// 11_02_2018~~~//
void centre::communicate_out_totalize(QString bar_str_1, QString bar_str_2, QString bar_str_3, QString bar_str_4, QString totalize_count_str, QString weight_str)
{
  cout<<endl<<"USB2SERIAL QStringVariant"<<endl;                                        //OMIT~~~
  cout<<"bar_1: "<<bar_str_1.toUtf8().constData();                                      //OMIT~~~
  cout<<"\tbar_2: "<<bar_str_2.toUtf8().constData();                                    //
  cout<<"\tbar_3: "<<bar_str_3.toUtf8().constData();                                    //
  cout<<"\tbar_4: "<<bar_str_4.toUtf8().constData()<<endl;                              //
  
  bool macro_status_bool;			      //temporary variable to transfer ifstream to tablewidget
  int macro_numb_int;				        //
  char macro_name_char[30];			    //
  char macro_mask_char[30];			    //
  char macro_function_char[30];		  //
 
  //QString combined_macro_functions;	//new char to combine all macros 
  QString macro_string;
  QString combined_macro_functions;
  QString count_string;
  QString barcode_string;
        
  {
    ifstream macros("macro_table");
    for(int i=0; i<10; ++i)		//searches all 10 macros available
    {
	    macros>>macro_status_bool;
	    macros>>macro_numb_int;
	    macros>>macro_name_char;
	    macros>>macro_mask_char;
	    macros>>macro_function_char;
	    if(macro_status_bool != 0)
      {
        QString macro_string;
        bool sending_string = false;//true indicates that we are sending out a string of characters
	      for(unsigned int j=0; j<strlen(macro_function_char)-1; ++j)
	      {
            
          cout<<"j = "<<j<<"   sending_string = "<<sending_string<<"    macro_function_char[j] = "<<macro_function_char[j]<<endl;
            
          if(sending_string)
          {
            
            cout<<"sending string.  j = "<<j<<endl;
            
            if(macro_function_char[j+1] == '"') 
            {
              sending_string = false;
            }
            else
            {
              macro_string += macro_function_char[j+1];
            }
          }
		      if(macro_function_char[j] == '\\')
		      {
            if(macro_function_char[j+1] == 'C')
	          {
	            macro_string = macro_string + totalize_count_str;
	          }
  	        else if(macro_function_char[j+1] == '1')		//Barcodes
	          {
              macro_string = macro_string + bar_str_1;
              cout<<"bar_1: "<<bar_str_1.toUtf8().constData()<<endl;  //OMIT~~~
  	        }
	          else if(macro_function_char[j+1] == '2')
	          {
              macro_string = macro_string + bar_str_2;
              cout<<"bar_2: "<<bar_str_2.toUtf8().constData()<<endl;  //OMIT~~~
	          }
	          else if(macro_function_char[j+1] == '3')
	          {
              macro_string = macro_string + bar_str_3;
              cout<<"bar_3: "<<bar_str_3.toUtf8().constData()<<endl;  //OMIT~~~
	          }
	          else if(macro_function_char[j+1] == '4')
	          {
              macro_string = macro_string + bar_str_4;
              cout<<"bar_4: "<<bar_str_4.toUtf8().constData()<<endl;  //OMIT~~~
	          }
	          else if(macro_function_char[j+1] == 'n')
	          {
              macro_string = macro_string + QString("\r\n");
              cout<<"newline"<<endl;  //OMIT~~~
	          }
	          else if(macro_function_char[j+1] == 't')
	          {
              macro_string = macro_string + QString("\t");
              cout<<"tab"<<endl;  //OMIT~~~
	          }
	          else if(macro_function_char[j+1] == '"')
	          {
              sending_string = true;
              cout<<"sending_string set true.  j = "<<j<<"\n";
	          }
	          else
	          {
			        macro_string = macro_string + macro_function_char[j] + macro_function_char[j+1];
	          }
	        }
	      }
        combined_macro_functions = combined_macro_functions + macro_string;
	    }
	    if(macros.eof()) break;
    }
    cout<<endl<<combined_macro_functions.toUtf8().constData()<<endl;  //OMIT~~~
    macros.close();
  }
//--------------------------------------------------------------OUTPUT TO SERIAL--------------------------------------------------------------//
  int size_string_macros = combined_macro_functions.size();
  if(communicate_by_keyboard_cable==true)
  {
    //int filedesc = open("/dev/TTL232RG", O_WRONLY);
    int filedesc = open("/dev/usb2serial", O_WRONLY);
    //int filedesc = open("/dev/ttyUSB0", O_WRONLY);  //if udev rules are not applied//
    //write(filedesc,(combined_macro_functions.toUtf8().constData()), size_string_macros) != size_string_macros;
  
    cout<<endl<<"serial string buffer length: "<<write(filedesc,(combined_macro_functions.toUtf8().constData()), size_string_macros)<<endl;
  }
  if(communicate_by_tcp==true)
  {
    tcp_write(combined_macro_functions);
  }
}

void centre::communicate_out_batch(QString lotcode_str, QString packcode_str, QString batch_count_str, QString substitution_str, QString dump_count_str)
{
    
  bool macro_status_bool;			      //temporary variable to transfer ifstream to tablewidget
  int macro_numb_int;				        //
  char macro_name_char[300];			  //
  char macro_mask_char[300];			  //
  char macro_function_char[300];		//
 
  QString macro_string;
  QString combined_macro_functions;
  QString count_string;
  QString barcode_string;
        
  {
    ifstream macros("macro_table");
    for(int i=0; i<10; ++i)
    {
	    macros>>macro_status_bool;
	    macros>>macro_numb_int;
	    macros>>macro_name_char;
	    macros>>macro_mask_char;
	    macros>>macro_function_char;
    
	    if(macro_status_bool != 0)
      {
        QString macro_string;
	      for(unsigned int j=0; j<strlen(macro_function_char); ++j)
	      {
		      if(macro_function_char[j] == '\\')
		      {
            if(macro_function_char[j+1] == 'C')
	          {
	            macro_string = macro_string + batch_count_str;
	          }
  	        else if(macro_function_char[j+1] == 'T')		  //Lot code
 	          {
	   		      macro_string = macro_string + lotcode_str;
            }
	          else if(macro_function_char[j+1] == 'P')		  //Pack code
	          {
			        macro_string = macro_string + packcode_str;
	          }
            else if(macro_function_char[j+1] == 'Q')		  //Dump count
	          {
			        macro_string = macro_string + dump_count_str;
	          }
	          else
	          {
			        macro_string = macro_string + macro_function_char[j] + macro_function_char[j+1];
	          }
	        }
	      }
        combined_macro_functions = combined_macro_functions + macro_string;
	    }
	    if(macros.eof()) break;
    }
    macros.close();
  }  
//--------------------------------------------------------------OUTPUT TO SERIAL--------------------------------------------------------------//
  //cout<<"Macros loaded"<<endl;  //OMIT~~~
//  int size_string_macros = combined_macro_functions.size();
  //int filedesc = open("/dev/TTL232RG", O_WRONLY);
//  int filedesc = open("/dev/usb2serial", O_WRONLY);
  //int filedesc = open("/dev/ttyUSB0", O_WRONLY);  //if udev rules are not applied//
  //write(filedesc,(combined_macro_functions.toUtf8().constData()), size_string_macros) != size_string_macros;
  //write(filedesc,(combined_macro_functions.toUtf8().constData()), size_string_macros);
//  cout<<endl<<"serial string buffer length: "<<write(filedesc,(combined_macro_functions.toUtf8().constData()), size_string_macros)<<endl; //ORIGINAL~~~
//--------------------------------------------------------------------------------------------------------------------------------------------//
  //serial terminal write function with slight delay between commands//
  /*int write_output = 0; //TEST~~~
  QString current_char_string;
  
  for(int xy = 0; xy< size_string_macros; ++xy)
  {
    if(combined_macro_functions.at(xy) == '\\')
    {
      QTime dieTime= QTime::currentTime().addMSecs(250);
      while (QTime::currentTime() < dieTime)
      {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
      }
      ++xy;
      if(combined_macro_functions.at(xy) == 'Q')
      {
        QTime dieTime= QTime::currentTime().addMSecs(350);
        while (QTime::currentTime() < dieTime)
        {
          QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
      }
      current_char_string = '\\' + combined_macro_functions.at(xy);
      write_output = write(filedesc, current_char_string.toUtf8().constData(), 2);
    }
    else
    {
      current_char_string = combined_macro_functions.at(xy);
      write_output = write(filedesc, current_char_string.toUtf8().constData(), 2);
    }
  }
  //cout<<current_char_string.toUtf8().constData();  //OMIT~~~
  cout<<write_output<<endl; //TEST~~~*/
//--------------------------------------------------------------------------------------------------------------------------------------------//
}







screen::screen(centre* set_centre_p)
 :QWidget()
{
  centre_p=set_centre_p;
  setMaximumSize(800, 480);		//Original~~~
  setGeometry(0,0,800,480);		//Original~~~
  //setMaximumSize(1000,600);		  //TEST~~~
  //setGeometry(0,0,1000,600);		//TEST~~~

  
  setStyleSheet(
    "button {"
        "border: 2px solid #8f8f91;"
        "height: 30px;"
        "font: 18px;"
        "border-radius: 12px;"
        "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
        "stop: 0 #f6f7fa, stop: 1 #dadbde);"
        "min-width: 40px;}"
        
    "QPushButton {"
        "border: 2px solid #8f8f91;"
        "font: 16px;"
        "border-radius: 8px;"
        "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
        "stop: 0 #f6f7fa, stop: 1 #dadbde);}"
                                  
    ".QSlider::groove:horizontal {"
        "border:none;"
        "margin-top: 10px;"
        "margin-bottom: 10px;"
        //"min-width: 140px;" //TEST~~~
        "height: 5px;}"
    
    ".QSlider::sub-page {"
        "background: #009900;}"

    ".QSlider::add-page {"
        "background: #8c8c8c;}"

    ".QSlider::handle {"
        //"background: e1e8a7;"
        "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
        "stop: 0 #f6f7fa, stop: 1 #dadbde);"
        "border: 3px solid #404040;"  //TEST~~~
        //"border: 3px solid black;"
        "width: 60px;"
        "margin: -13px 0;"
        "border-radius: 15px;}"
        
    "QGridLayout {"
        "padding: 0px;"
        "margin: 0px;"
        "border: 0px;"
        "max-height: 480px;}"
    
    "QTabWidget {"                    //macro-screen//
        "font-size: 13pt;}"
        
    "QHeaderView {"
        "font-size: 13pt;}"
        
    "QTableView {"
        "font-size: 13pt;"
        "border: 2px solid #8f8f91;}"
        
    "QScrollBar::vertical {"
        "width: 60px;"
        "border: 1px solid #404040;"
        "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
        "stop: 0 #f6f7fa, stop: 1 #dadbde);"
        "}"

    "QScrollBar::horizontal {"
        "height: 40px;"
        "border: 1px solid #404040;"
        "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
        "stop: 0 #f6f7fa, stop: 1 #dadbde);"
        "}"
        
    "QScrollBar::handle {"
        //"border: 3px solid black;"
        "border: 3px solid #595959;"
        "min-height: 40px;"
        "border-radius: 12px;"
        "background: white;"
        "}"
      
    "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {"
        "width: 0;"
        "height: 0;"
        "border: none;"
        "background: none;"
        "color: none;}"
        
    "QScrollBar::add-line:vertical {"
        "width: 0;"
        "height: 0;"      
        "border: none;"
        "background: none;}"

    "QScrollBar::sub-line:vertical {"
        "width: 0;"
        "height: 0;"
        "border: none;"
        "background: none;}"
        
    "QScrollBar::up-arrow:horizontal, QScrollBar::down-arrow:horizontal {"
        "width: 0;"
        "height: 0;"
        "border: none;"
        "background: none;"
        "color: none;}"
        
    "QScrollBar::add-line:horizontal {"
        "width: 0;"
        "height: 0;"      
        "border: none;"
        "background: none;}"

    "QScrollBar::sub-line:horizontal {"
        "width: 0;"
        "height: 0;"
        "border: none;"
        "background: none;}"
        
    "QRadioButton::indicator {"
        "font-size: 15pt;"
        "width: 45px;"
        "height: 45px;}"
        
    "QSpinBox {"
        "font-size: 20pt;"
        "width: 120px;"
        "height: 80px;}"
    
    "QSpinBox::down-arrow, QSpinBox::up-arrow {"
        "width: 60px;"
        "height: 15px;}"
        
    "QSpinBox::down-button, QSpinBox::up-button {"
        "width: 80px;"
        "height: 40px;}"
    
    "QLabel {"
        "font-size: 13pt;}"
        
    "QGroupBox{"
//        "background: white}"
        "border: 1px solid black}"
        "margin: 2px"
  );
}
//==============================================================================================================//  //TEST~~~ 11_14_2018
void centre::macro_name_cell(int row, int col)  //FROM MACRO_SCREEN.CPP//
{
  nRow = row;
  nCol = col;
}

void centre::macro_name_keyboard(QString macro_name) //FROM KEYBOARD.CPP//
{
  QList<QString> macro_file_list;
  QString macro_strings;
  char temp_name[300];
  
  if(macro_name.isEmpty()) macro_name = "-";
  if(macro_name.contains(" ")) macro_name.replace(" ", "_");
  
  ifstream macros_in("macro_table");  //INPUT FROM FILE//
  
  if(macros_in)
  {
    for(int i=1; i<=10; ++i)
    {
      for(int j=1; j<=5; ++j)
      {        
        macros_in>>temp_name;
        macro_file_list << QString::fromLatin1(temp_name);
      }
      if(macros_in.eof()) break;
    }
    macros_in.close();
  }
  macro_file_list.replace((nRow*5+nCol), macro_name);
      
  std::ofstream macros_out("macro_table",(std::ofstream::out));   //OUTPUT TO FILE//
  
  for(int i=0; i<10; ++i)
  {  
    macros_out<<endl;
    for(int j=0; j<5; ++j)
    {
      macros_out<<macro_file_list.at(i*5+j).toLatin1().data();
      macros_out<<" ";
    }
  }
  macros_out.close();
  macros_out.clear();
}
//==============================================================================================================//
void centre::load_macros()	//TEST~~~ connecting macros screen
{
  QString macro_string;
  combined_macro_functions.clear();
  QString count_string;
  
  //int char_array_it = 0;
  char barcode_1[30];		//TEST~~~ totalize_mode variables
  char barcode_2[30];
  char barcode_3[30];
  char barcode_4[30];
  char seed_count[30];
  //char seed_weight[30];	//implement when scale is added to seed counter
  
  char lotcode[30];			//TEST~~~ batch_mode variables
  char packcode[30];
  //char substitution[30];  //yet to be implemeneted//
  //char seed_count[30];
//----------------------------------------------------------------------------------------------------------------//

  {
    //table_p = new table;
	ifstream barcodes("settings/totalize_backup");
	if(!barcodes.is_open())
	{
	  cout<<"settings/totalize_backup file not open\n";
	  exit(1);
	}
	
    while(!barcodes.eof())	//TEST~~~
    {
      barcodes>>barcode_1;
      barcodes>>barcode_2;
      barcodes>>barcode_3;
      barcodes>>barcode_4;
      barcodes>>seed_count;
    }
    
    cout<<endl<<endl<<"LOADING TOTALIZEMODE"<<endl;		//OMIT~~~
    cout<<"\t"<<"barcode_1: "<<barcode_1<<endl		    //
		<<"\t"<<"barcode_2: "<<barcode_2<<endl		        //
		<<"\t"<<"barcode_3: "<<barcode_3<<endl		        //
		<<"\t"<<"barcode_4: "<<barcode_4<<endl		        //
		<<"\t"<<"seed_count: "<<seed_count<<endl;	        //
    barcodes.close();
    
    for(int q=0; q<30; ++q)
    {
      if(barcode_1[q] == ',')
      {
        barcode_1[q] = 0;
      }
      if(barcode_2[q] == ',')
      {
        barcode_2[q] = 0;
      }
      if(barcode_3[q] == ',')
      {
        barcode_3[q] = 0;
      }
      if(barcode_4[q] == ',')
      {
        barcode_4[q] = 0;
      }
    }
  }
//----------------------------------------------------------------------------------------------------------------//
  {  
	ifstream batchcodes("settings/batch_backup");
	if(!batchcodes.is_open())
	{
	  cout<<"settings/batch_backup file not open\n";
	  exit(1);
	}
	
	
    while(!batchcodes.eof())	//TEST~~~
    {
	    batchcodes>>lotcode;
	    batchcodes>>packcode;
	    batchcodes>>seed_count;
	    //batchcodes>>substitution;
    }
    
    cout<<endl<<endl<<"LOADING BATCHMODE"<<endl;			  //OMIT~~~
    cout<<"\t"<<"lotcode: "<<lotcode<<endl				      //
		    <<"\t"<<"packcode: "<<packcode<<endl			      //
		    //<<"\t"<<"substitution: "<<substitution<<endl	//
        <<"\t"<<"seed_count: "<<seed_count<<endl;		    //
    batchcodes.close();									                //
    
    for(int q=0; q<30; ++q)
	  {
	    if(lotcode[q] == ',')
	    {
	      lotcode[q] = 0;
	    }
	    if(packcode[q] == ',')
	    {
	      packcode[q] = 0;
	    }
	    if(seed_count[q] == ',')
	    {
	      seed_count[q] = 0;
	    }
	  }
  }
//----------------------------------------------------------------------------------------------------------------//
  {
    ifstream macros("macro_table");
	if(!macros.is_open())
	{
	  cout<<"macro_table file not open\n";
	  exit(1);
	}
	
    
    for(int i=0; i<10; ++i)		//searches all 10 macros available
    {
	    macros>>macro_status_bool;
	    macros>>macro_numb_int;
	    macros>>macro_name_char;
	    macros>>macro_mask_char;
	    macros>>macro_function_char;
	
      //if(macros.eof()) break;
	    if(macro_status_bool == 0)
      { 
		
      }
      else
      {
        QString macro_string;
	  
        for(unsigned int j=0; j<strlen(macro_function_char); ++j)
        {
          if(macro_function_char[j] == '\\')
          {
            if(macro_function_char[j+1] == 'C')
            {
              count_string = QString::number(count);
              macro_string = macro_string + count_string;
            }
            else if(macro_function_char[j+1] == '1')		//Barcodes
            {
              if(tm_barcode_columns >= 1)
              {
                macro_string = macro_string + barcode_1;
              }
            }
            else if(macro_function_char[j+1] == '2')
            {
              if(tm_barcode_columns >=2)
              {
                macro_string = macro_string + barcode_2;
              }
            }
            else if(macro_function_char[j+1] == '3')
            {
              if(tm_barcode_columns >= 3)
              {
                macro_string = macro_string + barcode_3;
              }
            }
            else if(macro_function_char[j+1] == '4')
            {
              if(tm_barcode_columns >= 4)
              {
                macro_string = macro_string + barcode_4;
              }
            }
            else if(macro_function_char[j+1] == 'T')		//Lot code
            {
              if(tm_barcode_columns >= 1)
			      {
			        macro_string = macro_string + lotcode;
			      }
            }
            else if(macro_function_char[j+1] == 'P')		//Pack code
            {
              if(tm_barcode_columns >=2)
              {
                macro_string = macro_string + packcode;
              } 
            }
            else
            {
              macro_string = macro_string + macro_function_char[j] + macro_function_char[j+1];
            }
          }
        }
        combined_macro_functions = combined_macro_functions + macro_string;
      }
      if(macros.eof()) break;
    }
    macros.close();
  }
  tm_macro_updated = true;
}

QString centre::choose_tcp_network(int choice)//choice 1 -> 192.168.100.1.  choice 2 -> 192.168.200.1.  Empty return -> success.  Error string returned for failure
{
  if(choice == 1)
  {
    network = 1;
    if(tcp_server_p->listen(QHostAddress("192.168.100.1"), 50000) == false)
    {
      return tcp_server_p->errorString();
    }
    return QString("");
  }
  if(choice == 2)
  {
    network = 2;
    if(tcp_server_p->listen(QHostAddress("192.168.200.1"), 50000) == false)
    {
      return tcp_server_p->errorString();
    }
    return QString("");
  }
  return QString("choice not found");
}

void centre::tcp_write(QString string)
{
  QByteArray array = string.toLatin1();
  cout<<"start tcp_write\n";
  for(int i=0; i<array.size(); ++i)
  {
    cout<<"array ["<<i<<"] = "<<int(array[i])<<endl;
  }
  tcp_socket_p->write(string.toLatin1());
}

