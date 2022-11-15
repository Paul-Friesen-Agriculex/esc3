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

#ifndef ESC3_HPP
#define ESC3_HPP

#include <QThread>
#include <QString>
#include <QChar>
#include <QStringList>
#include <QWidget>
#include <QImage>
#include <QFile>

#include "cutgate.hpp"
#include "endgate.hpp"
#include "envelope_sensor.hpp"
#include "feeder.hpp"
#include "crop.hpp"

class QTimer;
class camera;
class processor;
class screen;
class batch_mode_driver;
class diagnostics_console;
class macro_screen; //TEST~~~
class brother_envelope_feeder;
class QTcpServer;
class QTcpSocket;
class QApplication;
class QByteArray;
struct termios;

const int screen_wait_list_size=10;

class centre : public QObject
{
  Q_OBJECT
	
  public:
  centre();
	~centre();

  public slots:
  void increase_count(int);
  void receive_qimage(QImage qimage_set);
  void receive_calibrated_crop(crop calibrated_crop);
  void run();
  bool save_settings(QString file_name);
  bool load_settings(QString file_name);
  void end_of_playback();
  void get_cycle_time(int value);
  void tcp_connection_detected();
  void read_tcp_socket();
  void read_serial_port();
  
  signals:
  void char_from_tcp(QChar tcp_char);
  void char_from_serial_port(QChar sp_char);
  void set_camera_processing(bool state);
  void playback_line();
  void get_qimage();
  void set_to_record(bool record_now);
  void set_crop(crop current_crop_set);
  void processor_display_blobs(bool value);
  void restart_calibration_signal();
  void save_image_signal(QString filename);	
  void load_image_signal(QString filename);	
  void tcp_connection_detected_signal();

  private: 
  
  QWidget* base_widget_p;
  
  bool init_ran;
  void init();
  
  QThread* processor_thread_p;
  feeder* feeder_p;
  brother_envelope_feeder* brother_envelope_feeder_p;
  QTimer* run_timer_p;
  QTimer* serial_port_timer_p;
  screen* screen_p;
  int previous_screen_list[10];
  int previous_screen_index;
  bool previous_screen_going_back;
  ENDGATE_STATE endgate_state;
  diagnostics_console* diagnostics_console_p;
  
  int user;
  QString user_names[100];
  QString user_passwords[100];
  int user_permission_levels[100];
  int default_permission_level;
  /*
  permission levels:
  1 - Operate only.
  2 - Can change settings but not modify or enter programs or crops.
  3 - Can enter programs or crops but not modify or delete old ones.
  4 - Can do anything except change users and permissions.
  5 - Unrestricted.
  */
  QByteArray* tcp_input_array_p;
  termios* termios_p;
  
  public:
  QApplication* application_p;
  void add_waiting_screen(int screen_to_add);
  int get_previous_screen();
  void select_crop(int crop_index);//moves the selected crop to position 0 in the array.  Crop in position 0 becomes the current crop.
  void delete_crop(int crop_index);
  void set_endgate_state(ENDGATE_STATE set_state);
  ENDGATE_STATE get_endgate_state();
  void set_speed(int speed_set);
  void set_camera_processing_f(bool state);
  void playback_line_f();
  void get_qimage_f();
  void set_to_record_f(bool record_now);
  void set_crop_f(crop current_crop_set);
  void processor_display_blobs_f(bool value);
  void restart_calibration_f();
  void save_image(QString filename);
  void load_image(QString filename);
  
  processor* processor_p;
  cutgate* cutgate_p;
  endgate* endgate_p;
  envelope_sensor* envelope_sensor_p;
  int count;
  int feed_speed;
  int totalize_feed_speed;//feed speed while running
  bool envelope_present;
  bool totalize_force_endgate_open;//true indicates that the user has set the endgate open in totalize mode
  
  int screen_wait_list[screen_wait_list_size];
  bool screen_done;
  int current_screen;
  
  crop crops[100];
  bool crop_list_changed;
  bool changing_existing_crop;//crop name entry should overwrite, not create a new crop
  bool recalibrating;//re-calibrating an existing crop
  bool end_of_playback_flag;
  int measured_line_frequency;
  float dust_streak_percentage();
  
  //options
  QString last_crop;
  
  //totalize mode
  int tm_barcode_columns;
  bool tm_zero_when_seed_discharged;
  int tm_autosave_count_limit;//after this many counts are recorded, autosaves the file
  int tm_autosave_count;//counts how many counts were recorded;
  QString tm_save_filename;//if this is not blank, a file save is triggered with this name.  Includes directory info.
  QString tm_last_filename;//holds the name last used to save a file.  No directory info or extension.
  
  bool communicate_by_keyboard_cable;
  bool communicate_by_tcp;
  bool tcp_link_established;
  int network;
  QString tcp_client_server_addr;
  QTcpServer* tcp_server_p;
  QTcpSocket* tcp_socket_p;
  QString choose_tcp_network(int choice);//choice 1 -> 192.168.100.1.  choice 2 -> 192.168.200.1.  Empty return -> success.  Error string returned for failure
  void tcp_write(QString string);
  
  bool communicate_by_serial_port;
  void setup_serial_communications(int baud_rate);
  void serial_port_write(QString string);
  QString serial_port_read();
  int serial_port_fd;
  int baud_rate;
  QString serial_port_name;
  bool serial_port_opened;
  
  bool communicate_by_opcua;
  
  //batch mode
  bool block_endgate_opening;//true prevents endgate from opening.  Used if barcode test fails in batch.
  
  //playback
  QImage qimage;
  
  bool signal_port_pulse_when_endgate_closes;
  
  QString macro_name;
  
  batch_mode_driver* batch_mode_driver_p;
  void communicate_out(char type);//'t'->totalize.  'p'->batch pack.  'd'->batch dump.  's'->batch substitution
  
  //general keyboard entry
  bool new_keyboard_entry;//true indicates that a keyboard screen has just run.  Use to signal the screen needing keyboard data that it asked for it.
  QString keyboard_message_string;
  QString keyboard_return_string;
  int control_int[10];//use to set conditions on return to asking screen
  
  //Macro types
  QStringList totalize_macros;//issued after each pack in totalize
  QStringList batch_pack_macros;//issued after each pack in batch mode
  QStringList batch_dump_macros;//issued after each seed lot dumped in batch mode
  QStringList batch_substitution_macros;//issued if user substitutes seed for a lot that is short in batch
  
  //macro_type: 0->totalize_macros, 1->batch_pack_macros, 2->batch_dump_macros, 4->batch_substitution_macros
  
  //for macro or macro name entry
  int macro_type_for_entry;
  int macro_row_for_entry;
  int number_of_macros();//of above type
  
  //following functions use macro_type_for_entry and macro_row_for_entry to determine which macro to operate on
  void set_macro_selection(bool select);
  bool get_macro_selection();
  void set_macro_name(QString name);
  QString get_macro_name();
  void set_macro(QString macro);
  QString get_macro();
  QString get_macro_display_string();
  
  //macro type to enter
  bool enter_totalize_macro;
  bool enter_batch_pack_macro;
  bool enter_batch_dump_macro;
  bool enter_batch_substitution_macro;
  
  //macro variables
  QString bar_str_1;
  QString bar_str_2;
  QString bar_str_3;
  QString bar_str_4;
  QString seed_type_str;
  QString pack_count_str;
  QString weight_str;
  QString av_seed_weight_str;
  QString lotcode_str;
  QString substitution_str;
  QString dump_count_str;
  
  void load_macros();
  void save_macros();
};

class screen : public QWidget
{
  Q_OBJECT
  
  public:
  screen(centre* set_centre_p);
  
  protected:
  centre* centre_p;
  int permission_level_required;
};

#endif
