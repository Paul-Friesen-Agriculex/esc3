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
class help_screen;
/*
enum command_input_mode_enum
{
  wait_start,
  wait_type_flag,
  wait_crop,
  wait_number_of_sets,
  wait_seeds_per_pack,
  wait_number_of_packs,
  wait_r1,
  wait_r2,
  wait_r3,
  wait_speed,
  wait_command,
  wait_single_speed
};
*/
struct slave_mode_command
{
  QChar type_flag;
  QString crop_name;
  int number_of_sets;
  QList<int> seeds_per_pack;
  QList<int> number_of_packs;
  int speed;
  QString command;
//  bool complete;//true indicates command ready to be executed
//  bool finished;//true indicates command has finished executing
};

class slave_mode_screen : public screen
{
  Q_OBJECT
  
  public:
  slave_mode_screen(centre*set_centre_p, batch_mode_driver* set_batch_mode_driver_p);
  ~slave_mode_screen();
  
  public slots:
  void pack_ready();
  void pack_collected(int);
  void dump_complete(int);
  
  private slots:
  void back_button_clicked();
  void function_as_server_1_clicked();
  void function_as_server_2_clicked();
  void function_as_client_clicked();
  void communicate_by_serial_port_clicked();
  void help_button_clicked();
  void exit_button_clicked();
  void connection_detected();
  void command_char(QChar character);
  void command_finished();
  void run();

  private:
  batch_mode_driver* batch_mode_driver_p;
  
  button* back_button_p;
  button* function_as_server_1_button_p;
  button* function_as_server_2_button_p;
  button* function_as_client_button_p;
  button* communicate_by_serial_port_button_p;
  button* help_button_p;
  button* exit_button_p;
  QLabel* connection_message_p;
  QTextEdit* command_screen_p;
  help_screen* help_screen_p;
  QGridLayout* main_layout_p;
  QTimer* timer_p;
  
//  command_input_mode_enum command_input_mode;
  char command_type_flag;
//  bool waiting_for_delimiter;
  QString command_line_string;//to hold entire command line between character 2 (marks start) and character 3 (marks end).  Does not include start and end markers, but does include delimiters (character 31)
//  QString command_string;//to hold only command itself eg "Start", "Dump".
  int number_of_sets_expected;
  int number_of_sets_received;
//  int int_to_build;//for use in calculating hex integers sent 1 byte at a time
  
  slave_mode_command* new_command_p;
  QQueue<slave_mode_command*> command_p_list;
  slave_mode_command* executing_command_p;
  slave_mode_command* previous_command_p;
//  QList<int> count_limits;
//  QList<int> pack_limits;
//  bool execute;
  
  bool batch_mode;
  int pack_count;
  int speed;
  bool command_finished_bool;
  
  bool end_gate_full;
  bool end_gate_opened_full;
  bool end_gate_closed_empty;
  bool end_gate_filling;
  
  void end_command();
  void display_command(slave_mode_command* command_p);
};
#endif
