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
#include "crop_name_exists.hpp"
#include "diagnostics_console.hpp"
#include "spreadsheet_choice.hpp"
#include "ss_setup_choice.hpp"
#include "ss_setup_entry.hpp"
#include "set_envelope_size.hpp"

#include "ss_setup_delete.hpp"
#include "ss_options.hpp"
#include "ss_column_display_options.hpp"
#include "ss_batch.hpp"
#include "select_envelope_field.hpp"
#include "position_envelope_field.hpp"
#include "select_field_data_source.hpp"
#include "macro_screen.hpp"	
#include "macro_builder.hpp"
#include "communications_menu.hpp"
#include "tcp_server_addr_choice.hpp"
#include "tcp_client_server_addr_entry.hpp"
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
  
  init_ran = false;

  run_timer_p = new QTimer;
  connect(run_timer_p, SIGNAL(timeout()), this, SLOT(run()));
  run_timer_p->start(10);
}

void centre::init()
{
  processor_p = new processor();
  processor_thread_p = new QThread(this);
  processor_p -> moveToThread(processor_thread_p);
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
    f>>crops[i].high_feed_speed;
    f>>crops[i].low_feed_speed;
    f>>crops[i].dump_speed;
    f.get();
    if(f.eof()) break;
  }
  f.close();

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
  tm_autosave_count_limit = 0;//after this many counts are recorded, autosaves the file
  tm_autosave_count = 0;//counts how many counts were recorded;
  tm_save_filename = "";
  block_endgate_opening = false;//true prevents endgate from opening.  Used if barcode test fails in batch.
  communicate_by_keyboard_cable = true;
  communicate_by_tcp = false;
  tcp_link_established = false;
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
  
//  tm_macro_updated = 0; 
  count=0;
  new_keyboard_entry=false;
  for(int i=0; i<10; ++i) control_int[i] = 0;
  
//  build_macro = false;//set true when leaving macro_screen for macro_builder.  signals that macro_builder will run.
//  macro_row = 0;// remember row for return to macro_screen.

//  totalize_macros_index = 0;
//  batch_pack_macros_index = 0;
//  batch_dump_macros_index = 0;
//  batch_substitution_macros_index = 0;
  /*
  totalize_macro_index = -1;//indicates nothing being entered
  batch_pack_macro_index = -1;
  batch_dump_macro_index = -1;
  batch_substitution_macro_index = -1;
  */
  enter_totalize_macro = false;
  enter_batch_pack_macro = false;
  enter_batch_dump_macro = false;
  enter_batch_substitution_macro = false;
  
  load_macros();
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
  save_macros();
}

void centre::increase_count(int to_add)
{
  count += to_add;
}

void centre::receive_qimage(QImage qimage_set)
{
  qimage = qimage_set;
}

void centre::receive_calibrated_crop(crop calibrated_crop)
{
  crops[0] = calibrated_crop;
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
  tcp_socket_p = tcp_server_p->nextPendingConnection();
  emit tcp_connection_detected_signal();
}

void centre::run()
{
  if(init_ran == false)
  {
    init();
    init_ran = true;
  }
    
  emit set_crop(crops[0]);
  endgate_state = endgate_p->get_state();
  envelope_present = envelope_sensor_p->read();
  brother_envelope_feeder_p -> run();

  if(current_screen==5)//totalize
  {
    if(endgate_state == ENDGATE_CLOSED)
    {
      if(totalize_force_endgate_open==true || envelope_present==true)
      {
        set_endgate_state(ENDGATE_OPEN);
      }
    }
    else //endgate is open
    {
      if(totalize_force_endgate_open==false && envelope_present==false)
      {
        set_endgate_state(ENDGATE_CLOSED);
      }
    }
  }
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
  if(screen_done)
  {
    screen_done=false;
    if(previous_screen_going_back == false)//add the current screen to previous_screen_list
    {
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
    }
    current_screen = screen_wait_list[0];
    previous_screen_going_back = false;//will be set true if get_previous_screen is called again
    for(int i=0; i<screen_wait_list_size-1; ++i)
    {
      screen_wait_list[i]=screen_wait_list[i+1];
    }
    screen_wait_list[screen_wait_list_size-1]=0;
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
//      case 27: screen_p=new macro_name_entry(this);  break;    //TEST~~~ 11_13_2018//
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
      case 38: screen_p=new macro_builder(this); break;
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
    delete old_screen_p;
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
  for(int i=screen_wait_list_size-1; i>0; --i)
  {
    screen_wait_list[i]=screen_wait_list[i-1];
  }
  screen_wait_list[0]=screen_to_add;
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

void centre::communicate_out(char type)//'t'->totalize.  'p'->batch pack.  'd'->batch dump.  's'->batch substitution
{
  QString out_string;
  QStringList* macro_list_p = &totalize_macros;
  if(type == 'p')
  {
    macro_list_p = &batch_pack_macros;
  }
  if(type == 'd')
  {
    macro_list_p = &batch_dump_macros;
  }
  if(type == 's')
  {
    macro_list_p = &batch_substitution_macros;
  }
  
  int index = -1;//to hold index of active macro.  -1 indicates invalid
  for(int i=0; i<macro_list_p->size(); ++i)
  {
    if( (macro_list_p->at(i)).size() > 0)//normally expected, but check
    {
      if( (macro_list_p->at(i))[0] == '1') //this is the active macro
      {
        index = i;
        break;
      }
    }
  }
  if(index == -1)
  {
    cout<<"no active macro\n";
    return;
  }

  QString m_string = macro_list_p->at(index);
  //remove active indication character and name
  while(m_string.size()>0)
  {
    if(m_string[0] == QChar(23))
    {
      m_string.remove(0,1);
      break;
    }
    m_string.remove(0,1);
  }
  
  //remaining m_string is the macro itself
  bool in_text = false;//true indicates that characters in macro should be sent as characters.  Otherwise, interpret as codes
  for(int i=0; i<m_string.size(); ++i)
  {
    if(m_string[i] == 2)//start of text
    {
      in_text = true;
    }
    else if(in_text)
    {
      if(m_string[i] == 3)//end of text
      {
        in_text = false;
      }
      else out_string += m_string[i];
    }
    else if(m_string[i] == 'c')
    {
      out_string += pack_count_str;
    }
    else if(m_string[i] == 's')
    {
      out_string += seed_type_str;
    }
    else if(m_string[i] == 'd')
    {
      out_string += dump_count_str;
    }
    else if(m_string[i] == 'w')
    {
      out_string += weight_str;
    }
    else if(m_string[i] == 'a')
    {
      out_string += av_seed_weight_str;
    }
    else if(m_string[i] == '1')
    {
      out_string += bar_str_1;
    }
    else if(m_string[i] == '2')
    {
      out_string += bar_str_2;
    }
    else if(m_string[i] == '3')
    {
      out_string += bar_str_3;
    }
    else if(m_string[i] == '4')
    {
      out_string += bar_str_4;
    }
    else if(m_string[i] == 'l')
    {
      out_string += lotcode_str;
    }
    else if(m_string[i] == 'u')
    {
      out_string += substitution_str;
    }
    else if(m_string[i] == 'N')
    {
      out_string += "\n";
    }
    else if(m_string[i] == 'T')
    {
      out_string += "\t";
    }
    else if(m_string[i] == 'S')
    {
      out_string += " ";
    }
    else if(m_string[i] == 'U')
    {
      out_string += "\\U";
    }
    else if(m_string[i] == 'D')
    {
      out_string += "\\D";
    }
    else if(m_string[i] == 'R')
    {
      out_string += "\\R";
    }
    else if(m_string[i] == 'L')
    {
      out_string += "\\L";
    }
  }  

  if(communicate_by_keyboard_cable==true)
  {
    int filedesc = open("/dev/usb2serial", O_WRONLY);
//    int ret_val = write(filedesc,(out_string.toUtf8().constData()), size_string_macros);
    int ret_val = write(filedesc,(out_string.toUtf8().constData()), out_string.size());
    if(ret_val<0) cout<<"write error writing to keyboard cable\n";
  }
  if(communicate_by_tcp==true)
  {
    tcp_write(out_string);
  }
}


/*
void centre::communicate_out(char type)//'t'->totalize.  'p'->batch pack.  'd'->batch dump.
{
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
        
  ifstream macros;
  if(type=='t') macros.open("macro_table");
  if(type=='p') macros.open("macro_table_batch_pack");
  if(type=='d') macros.open("macro_table_batch_dump");
  
  if(macros.good())
  {
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
          if(sending_string)
          {
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
              totalize_count_str = "";
	          }
  	        else if(macro_function_char[j+1] == '1')		//Barcodes
	          {
              macro_string = macro_string + bar_str_1;
              bar_str_1 = "";
  	        }
	          else if(macro_function_char[j+1] == '2')
	          {
              macro_string = macro_string + bar_str_2;
              bar_str_2 = "";
	          }
	          else if(macro_function_char[j+1] == '3')
	          {
              macro_string = macro_string + bar_str_3;
              bar_str_3 = "";
	          }
	          else if(macro_function_char[j+1] == '4')
	          {
              macro_string = macro_string + bar_str_4;
              bar_str_4 = "";
	          }
	          else if(macro_function_char[j+1] == 'n')
	          {
              macro_string = macro_string + QString("\r\n");
	          }
	          else if(macro_function_char[j+1] == 't')
	          {
              macro_string = macro_string + QString("\t");
	          }
	          else if(macro_function_char[j+1] == '"')
	          {
              sending_string = true;
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
//  else//macro file did not open.  probably does not exist
//--------------------------------------------------------------OUTPUT TO SERIAL--------------------------------------------------------------//
  int size_string_macros = combined_macro_functions.size();
  if(communicate_by_keyboard_cable==true)
  {
    int filedesc = open("/dev/usb2serial", O_WRONLY);
    int ret_val = write(filedesc,(combined_macro_functions.toUtf8().constData()), size_string_macros);
    if(ret_val<0) cout<<"write error writing to keyboard cable\n";
  }
  if(communicate_by_tcp==true)
  {
    tcp_write(combined_macro_functions);
  }
}
*/

screen::screen(centre* set_centre_p)
 :QWidget()
{
  centre_p=set_centre_p;
  setMaximumSize(800, 480);		//Original~~~
  setGeometry(0,0,800,480);		//Original~~~
  
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
        "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
        "stop: 0 #f6f7fa, stop: 1 #dadbde);"
        "border: 3px solid #404040;"  //TEST~~~
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
        "border: 1px solid black}"
        "margin: 2px"
  );
}
/*
//==============================================================================================================//
void centre::load_macros()	//TEST~~~ connecting macros screen
{
  QString macro_string;
  combined_macro_functions.clear();
  QString count_string;
  
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
//  tm_macro_updated = true;
}
*/
QString centre::choose_tcp_network(int choice)//choice 1 -> 192.168.100.1.  choice 2 -> 192.168.200.1.  Empty return -> success.  Error string returned for failure
{
  if(tcp_socket_p) tcp_socket_p->disconnectFromHost();
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
  tcp_socket_p->write(string.toLatin1());
}

int centre::number_of_macros()
{
  QStringList* list_p = 0;
  if(macro_type_for_entry==0) list_p = &totalize_macros;
  if(macro_type_for_entry==1) list_p = &batch_pack_macros;
  if(macro_type_for_entry==2) list_p = &batch_dump_macros;
  if(macro_type_for_entry==3) list_p = &batch_substitution_macros;
  return(list_p->size());
}

void centre::set_macro_selection(bool select)
{
  QStringList* list_p = 0;
  if(macro_type_for_entry==0) list_p = &totalize_macros;
  if(macro_type_for_entry==1) list_p = &batch_pack_macros;
  if(macro_type_for_entry==2) list_p = &batch_dump_macros;
  if(macro_type_for_entry==3) list_p = &batch_substitution_macros;
  
  QString string = list_p->at(macro_row_for_entry);
  if(select) string.replace(0, 1, "1");
  else string.replace(0, 1, "0");
  list_p->replace(macro_row_for_entry, string);
}

bool centre::get_macro_selection()
{
  QStringList* list_p = 0;
  if(macro_type_for_entry==0) list_p = &totalize_macros;
  if(macro_type_for_entry==1) list_p = &batch_pack_macros;
  if(macro_type_for_entry==2) list_p = &batch_dump_macros;
  if(macro_type_for_entry==3) list_p = &batch_substitution_macros;
  
  QString string = list_p->at(macro_row_for_entry);
  if(string[0] == '1') return true;
  return false;
}

void centre::set_macro_name(QString name)
{
  
  cout<<"centre::set_macro_name 1\n";
  cout<<"macro_type_for_entry = "<<macro_type_for_entry<<endl;
  
  QStringList* list_p = 0;
  if(macro_type_for_entry==0) list_p = &totalize_macros;
  if(macro_type_for_entry==1) list_p = &batch_pack_macros;
  if(macro_type_for_entry==2) list_p = &batch_dump_macros;
  if(macro_type_for_entry==3) list_p = &batch_substitution_macros;
  
  cout<<"centre::set_macro_name 2\n";
  
  for(int i=0; i<list_p->size(); ++i)//mark all macros of this type de-selected
  {
//    if(list_p->at(i).size()>0) (list_p->at(i))[0] = QChar('0');
    if(list_p->at(i).size()>0) 
    {
      QString string = list_p->at(i);
      string.replace(0, 1, "0");
      list_p->replace(i, string);
    }
  }
  
  cout<<"centre::set_macro_name 3\n";
  
  QString string = list_p->at(macro_row_for_entry);
  int name_end = string.indexOf(QChar(23));
  string = string.right(string.size()-name_end);//remove old name plus selection character at start
  string.prepend(name);//add new name
  string.prepend("1");//select this macro.  add new selection character
  list_p->replace(macro_row_for_entry, string);
  
  cout<<"centre::set_macro_name 4\n";
  
}

QString centre::get_macro_name()
{
  QStringList* list_p = 0;
  if(macro_type_for_entry==0) list_p = &totalize_macros;
  if(macro_type_for_entry==1) list_p = &batch_pack_macros;
  if(macro_type_for_entry==2) list_p = &batch_dump_macros;
  if(macro_type_for_entry==3) list_p = &batch_substitution_macros;
  
  QString string = list_p->at(macro_row_for_entry);
  int name_end = string.indexOf(QChar(23));
  string = string.left(name_end);
  string = string.remove(0,1);//remove selection character at start
  return string;
}
  
void centre::set_macro(QString macro)
{
  QStringList* list_p = 0;
  if(macro_type_for_entry==0) list_p = &totalize_macros;
  if(macro_type_for_entry==1) list_p = &batch_pack_macros;
  if(macro_type_for_entry==2) list_p = &batch_dump_macros;
  if(macro_type_for_entry==3) list_p = &batch_substitution_macros;

  QString string = list_p->at(macro_row_for_entry);
  int name_end = string.indexOf(QChar(23));
  string = string.left(name_end+1);//remove old macro
  string.append(macro);
  list_p->replace(macro_row_for_entry, string);
}
  
QString centre::get_macro()
{
  QStringList* list_p = 0;
  if(macro_type_for_entry==0) list_p = &totalize_macros;
  if(macro_type_for_entry==1) list_p = &batch_pack_macros;
  if(macro_type_for_entry==2) list_p = &batch_dump_macros;
  if(macro_type_for_entry==3) list_p = &batch_substitution_macros;
  
  QString string = list_p->at(macro_row_for_entry);
  int name_end = string.indexOf(QChar(23));
  cout<<"name_end = "<<name_end<<endl;
  cout<<"string.size = "<<string.size()<<endl;
  string = string.right(string.size()-name_end-1);
  cout<<"string-"<<string.toStdString()<<"-\n";
  return string;
}
  
QString centre::get_macro_display_string()
{
  QString macro_display_string;
  QString macro_string=get_macro();
  bool in_text = false;
  if(macro_string.size() == 0)//not expected
  {
    cout<<"centre::get_macro_display_string.  macro_string empty\n";
    return "";
  }
  for(int j=0; j<macro_string.size(); ++j)
  {
    if(macro_string[j] == QChar(2))//start of text
    {
      in_text = true;
      macro_display_string.append(" \"");
      continue;
    }
    if(macro_string[j] == QChar(3))//end of text. characters outside of text are codes
    {
      in_text = false;
      macro_display_string.append("\" ");
      continue;
    }
    if(in_text)
    {
      macro_display_string.append(macro_string[j]);
    }
    else
    {
      if(macro_string[j] == 'c')
      {
        macro_display_string.append("<pack count>");
      }
      if(macro_string[j] == 's')
      {
        macro_display_string.append("<seed type>");
      }
      if(macro_string[j] == 'd')
      {
        macro_display_string.append("<dump count>");
      }
      if(macro_string[j] == 'w')
      {
        macro_display_string.append("<weight>");
      }
      if(macro_string[j] == 'a')
      {
        macro_display_string.append("<average seed weight>");
      }
      if(macro_string[j] == '1')
      {
        macro_display_string.append("<barcode 1>");
      }
      if(macro_string[j] == '2')
      {
        macro_display_string.append("<barcode 2>");
      }
      if(macro_string[j] == '3')
      {
        macro_display_string.append("<barcode 3>");
      }
      if(macro_string[j] == '4')
      {
        macro_display_string.append("<barcode 4>");
      }
      if(macro_string[j] == 'l')
      {
        macro_display_string.append("<seed lot barcode>");
      }
      if(macro_string[j] == 'u')
      {
        macro_display_string.append("<substitution bar code>");
      }
      if(macro_string[j] == 'N')
      {
        macro_display_string.append("<new line>");
      }
      if(macro_string[j] == 'T')
      {
        macro_display_string.append("<tab>");
      }
      if(macro_string[j] == 'S')
      {
        macro_display_string.append("<space>");
      }
      if(macro_string[j] == 'U')
      {
        macro_display_string.append("<up arrow>");
      }
      if(macro_string[j] == 'D')
      {
        macro_display_string.append("<down arrow>");
      }
      if(macro_string[j] == 'R')
      {
        macro_display_string.append("<right arrow>");
      }
      if(macro_string[j] == 'L')
      {
        macro_display_string.append("<left arrow>");
      }
    }
  }
  return macro_display_string;
}

void centre::load_macros()
{
  QFile file("communication_macros");
  char ch;
  QString string;
  bool getting_totalize_macros = true;
  bool getting_batch_pack_macros = false;
  bool getting_batch_dump_macros = false;
  bool getting_batch_substitution_macros = false;
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text))//file does not exist.  initialize macro lists with blank entries
  {
    string = "0";//first character indicates not selected
    string.append(QChar(23));//ASCII code 23 used to mark end of macro name, blank in this case
    for(int i=0; i<10; ++i)
    {
      totalize_macros.append(string);
      batch_pack_macros.append(string);
      batch_dump_macros.append(string);
      batch_substitution_macros.append(string);
    }
    return;
  }
  cout<<"communication_macros file\n";
  while(!file.atEnd())
  {
    file.getChar(&ch);
    if(ch<32) cout<<"character "<<int(ch)<<endl;
    else cout<<ch<<endl;
    if(ch == 0)//marks end of macro
    {
      if(getting_totalize_macros)
      {
        totalize_macros.append(string);
        string.clear();
      }
      if(getting_batch_pack_macros)
      {
        batch_pack_macros.append(string);
        string.clear();
      }
      if(getting_batch_dump_macros)
      {
        batch_dump_macros.append(string);
        string.clear();
      }
      if(getting_batch_substitution_macros)
      {
        batch_substitution_macros.append(string);
        string.clear();
      }
    }
    else if(ch == 1)//marks end of macro type
    {
      if(getting_totalize_macros)
      {
        getting_totalize_macros = false;
        getting_batch_pack_macros = true;
      }
      else if(getting_batch_pack_macros)
      {
        getting_batch_pack_macros = false;
        getting_batch_dump_macros = true;
      }
      else if(getting_batch_dump_macros)
      {
        getting_batch_dump_macros = false;
        getting_batch_substitution_macros = true;
      }
    }
    else
    {
      string.append(ch);
    }
  }
  cout<<"end communication_macros file\n";
}

void centre::save_macros()
{
  QFile file("communication_macros");
  QString string;
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
  for(int i=0; i<totalize_macros.size(); ++i)
  {
    string = totalize_macros[i];
    for(int j=0; j<string.size(); ++j)
    {
      file.putChar(string[j].toLatin1());
    }
    file.putChar(0);
  }
  file.putChar(1);//mark end of macro type
  for(int i=0; i<batch_pack_macros.size(); ++i)
  {
    string = batch_pack_macros[i];
    for(int j=0; j<string.size(); ++j)
    {
      file.putChar(string[j].toLatin1());
    }
    file.putChar(0);
  }
  file.putChar(1);//mark end of macro type
  for(int i=0; i<batch_dump_macros.size(); ++i)
  {
    string = batch_dump_macros[i];
    for(int j=0; j<string.size(); ++j)
    {
      file.putChar(string[j].toLatin1());
    }
    file.putChar(0);
  }
  file.putChar(1);//mark end of macro type
  for(int i=0; i<batch_substitution_macros.size(); ++i)
  {
    string = batch_substitution_macros[i];
    for(int j=0; j<string.size(); ++j)
    {
      file.putChar(string[j].toLatin1());
    }
    file.putChar(0);
  }
}
  
