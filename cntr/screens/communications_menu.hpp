#ifndef communications_menu_hpp
#define communications_menu_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QRadioButton;
class QSpinBox;
class help_screen;

class communications_menu : public screen
{
  Q_OBJECT
  
  public:
  communications_menu(centre*set_centre_p);
  ~communications_menu();
  
  private slots:
  void back_button_clicked();
  void communicate_by_keyboard_cable_button_clicked();
  void function_as_server_1_clicked();
  void function_as_server_2_clicked();
  void function_as_client_clicked();
  void communicate_by_serial_port_clicked();
  void help_button_clicked();
  void ok_button_clicked();
  void connection_detected();

  private:
  button* back_button_p;
  button* communicate_by_keyboard_cable_button_p;
  button* function_as_server_1_button_p;
  button* function_as_server_2_button_p;
  button* function_as_client_button_p;
  button* communicate_by_serial_port_button_p;
  button* help_button_p;
  button* ok_button_p;
  QLabel* message_p;
  help_screen* help_screen_p;

  QGridLayout* main_layout_p;

//  centre* centre_p;
};
#endif
