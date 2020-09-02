#ifndef ESC3_HPP
#define ESC3_HPP

#include <QThread>
#include <QString>
#include <QWidget>
#include <QImage>
#include <QFile>

#include "cutgate.hpp"
#include "endgate.hpp"
#include "envelope_sensor.hpp"
#include "feeder.hpp"
#include "crop.hpp"

class QTimer;
//class QLabel;
//class message_box;
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
  
  signals:
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
  
//  QLabel* startup_progress_label_p; 
//  message_box* startup_progress_label_p; 
  QThread* processor_thread_p;
  endgate* endgate_p;
  envelope_sensor* envelope_sensor_p;
  feeder* feeder_p;
  brother_envelope_feeder* brother_envelope_feeder_p;
  QTimer* run_timer_p;
  screen* screen_p;
  int previous_screen_list[10];
  int previous_screen_index;
  bool previous_screen_going_back;
//  CUTGATE_STATE cutgate_state;
  ENDGATE_STATE endgate_state;
  diagnostics_console* diagnostics_console_p;
  //table* table_p; //TEST~~~
  
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
  
  public:
  QApplication* application_p;
  void add_waiting_screen(int screen_to_add);
  int get_previous_screen();
  void select_crop(int crop_index);//moves the selected crop to position 0 in the array.  Crop in position 0 becomes the current crop.
  void delete_crop(int crop_index);
//  void set_cutgate_state(CUTGATE_STATE set_state);
  void set_endgate_state(ENDGATE_STATE set_state);
//  CUTGATE_STATE get_cutgate_state();
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
  void load_macros();	//original~~~
  
//=========================================================//  
//  void macro_name_cell(int row, int col);
//  void macro_name_keyboard(QString);
//  int nRow, nCol;
//=========================================================//
  
  processor* processor_p;
  cutgate* cutgate_p;
  int count;
//  CUTGATE_STATE cutgate_state;
//  ENDGATE_STATE endgate_state;
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
  bool tm_macro_updated;	//TEST~~~
  int tm_autosave_count_limit;//after this many counts are recorded, autosaves the file
  int tm_autosave_count;//counts how many counts were recorded;
  QString tm_save_filename;//if this is not blank, a file save is triggered with this name.  Includes directory info.
  QString tm_last_filename;//holds the name last used to save a file.  No directory info or extension.
  
  bool macro_status_bool;			  //temporary variable to transfer ifstream to tablewidget
  int macro_numb_int;				    //
  char macro_name_char[30];			//
  char macro_mask_char[30];			//
  char macro_function_char[30];		//
  QString combined_macro_functions;	//new char to combine all macros
  bool communicate_by_keyboard_cable;
  bool communicate_by_tcp;
  bool tcp_link_established;
//  bool tcp_server;
  int network;
  QString tcp_client_server_addr;
  QTcpServer* tcp_server_p;
  QTcpSocket* tcp_socket_p;
  QString choose_tcp_network(int choice);//choice 1 -> 192.168.100.1.  choice 2 -> 192.168.200.1.  Empty return -> success.  Error string returned for failure
  void tcp_write(QString string);
  
  //batch mode
  bool block_endgate_opening;//true prevents endgate from opening.  Used if barcode test fails in batch.
  
  //playback
  QImage qimage;
  
  bool signal_port_pulse_when_endgate_closes;
  
  QString macro_name; //TEST~~~11_13_2018//
  
  batch_mode_driver* batch_mode_driver_p;
//  macro_screen* macro_screen_p; //TEST~~~ 11_13_2018//
  void communicate_out_totalize(QString bar_str_1, QString bar_str_2, QString bar_str_3, QString bar_str_4, QString totalize_count_str, QString weight_str);
  void communicate_out_batch(QString lotcode_str, QString packcode_str, QString batch_count_str, QString substitution_str, QString dump_count_str);
  
  //general keyboard entry
  bool new_keyboard_entry;//true indicates that a keyboard screen has just run.  Use to signal the screen needing keyboard data that it asked for it.
  QString keyboard_message_string;
  QString keyboard_return_string;
  int control_int[10];//use to set conditions on return to asking screen
  
  //macro_builder
  bool build_macro;//set true when leaving macro_screen for macro_builder.  signals that macro_builder will run.
  int macro_row;// remember row for return to macro_screen.
  QString macro_display_string;
  QString macro_function_string;
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
