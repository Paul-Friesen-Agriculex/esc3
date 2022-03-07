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
  void pack_collected(int);
  void dump_complete(int);
  
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
  void command_finished();
  void run();

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
  help_screen* help_screen_p;
  QGridLayout* main_layout_p;
  QTimer* timer_p;
  
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
//  QString opcua_command_segment;//command may not arrive all at once.  this holds each segment
//  bool opcua_query_sent;
//  bool opcua_response_received;
  int opcua_count;//to time wait for query response
  void run_opcua();
  
  bool batch_mode;
  int pack_count;
  int speed;
  bool command_finished_bool;
  bool just_starting;//wish to ignore any command on opc device when starting.  enter command only if changed.
  
  bool end_gate_full;
  bool end_gate_opened_full;
  bool end_gate_closed_empty;
  bool end_gate_filling;
  
  void end_command();
  void display_command(slave_mode_command* command_p);
};
#endif
