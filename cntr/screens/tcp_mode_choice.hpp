#ifndef tcp_mode_choice_hpp
#define tcp_mode_choice_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QRadioButton;
class QSpinBox;

class tcp_mode_choice : public screen
{
  Q_OBJECT
  
  public:
  tcp_mode_choice(centre*set_centre_p);
  
  private slots:
  void back_button_clicked();
  void function_as_server();
  void function_as_client();
  void help_button_clicked();

  private:
  button* back_button_p;
  button* function_as_server_button_p;
  button* function_as_client_button_p;
  button* help_button_p;

  QGridLayout* main_layout_p;

//  centre* centre_p;
};
#endif
