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

#ifndef slave_mode_screen_hpp
#define slave_mode_screen_hpp

#include "centre.hpp"
#include "batch_mode_driver.hpp"
#include <QString>
#include <QList>
#include <QQueue>
class button;
class QGridLayout;
class QTimer;
class QLabel;
class QTextEdit;
class QRadioButton;
class help_screen;
class envelope_sensor;


struct slave_mode_command
{
  QChar type_flag;
  QString crop_name;
  int number_of_sets;
  QList<int> seeds_per_pack;
  QList<int> number_of_packs;
  int speed;
  QString command;
};

class slave_mode_screen : public screen
{
  Q_OBJECT
  
  public:
  slave_mode_screen(centre*set_centre_p, batch_mode_driver* set_batch_mode_driver_p);
  ~slave_mode_screen();
  
  public slots:
  void pack_ready();
//  void pack_collected(int);
  void dump_complete(int);
  void slave_mode_program_finished();
  
  private slots:
  void back_button_clicked();
  void function_as_server_1_clicked();
  void function_as_server_2_clicked();
  void function_as_client_clicked();
  void communicate_by_serial_port_clicked();
  void opcua_clicked();
  void help_button_clicked();
  void exit_button_clicked();
  void connection_detected();
  void command_char(QChar character);
  void opcua_command_char(QChar character);
  void run();
  
  signals:
  void new_slave_mode_command();

  private:
  batch_mode_driver* batch_mode_driver_p;
  
  button* back_button_p;
  button* function_as_server_1_button_p;
  button* function_as_server_2_button_p;
  button* function_as_client_button_p;
  button* communicate_by_serial_port_button_p;
  button* opcua_button_p;
  button* help_button_p;
  button* exit_button_p;
  QLabel* connection_message_p;
  QLabel* command_message_p;
  QTextEdit* command_screen_p;
  QRadioButton* display_data_requests_p;
  help_screen* help_screen_p;
  QGridLayout* main_layout_p;
  QTimer* timer_p;
  envelope_sensor* envelope_sensor_p;
  int remembered_speed;
  
  char command_type_flag;
  QString command_line_string;//to hold entire command line between character 2 (marks start) and character 3 (marks end).  Does not include start and end markers, but does include delimiters (character 31)
  int number_of_sets_expected;
  int number_of_sets_received;
  
  slave_mode_command* new_command_p;
  QQueue<slave_mode_command*> command_p_list;
  slave_mode_command* executing_command_p;
  slave_mode_command* previous_command_p;
  
  int opcua_mode;
  QString opcua_line;
  QString old_command;//to detect if command changes
  int opcua_count;//to time wait for query response
  void run_opcua();
  bool opcua_message_to_write;
  QString opcua_message;
  
  bool batch_mode;
  int pack_count;
  bool pack_ready_bool;
  int speed;
  bool command_finished_bool;
  bool just_starting;//wish to ignore any command on opc device when starting.  enter command only if changed.
  mode_enum mode_to_start;
  
  bool end_gate_full;
  bool end_gate_opened_full;
  bool end_gate_closed_empty;
  bool end_gate_filling;
  
  void end_command();
  void display_command(slave_mode_command* command_p);
};
#endif
