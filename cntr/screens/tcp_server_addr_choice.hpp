#ifndef tcp_server_addr_choice_hpp
#define tcp_server_addr_choice_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QRadioButton;
class QSpinBox;

class tcp_server_addr_choice : public screen
{
  Q_OBJECT
  
  public:
  tcp_server_addr_choice(centre*set_centre_p);
  
  private slots:
  void back_button_clicked();
  void net_100_clicked();
  void net_200_clicked();
  void ok_button_clicked();
  void help_button_clicked();
  void connection_detected();

  private:
  button* back_button_p;
  button* net_100_button_p;
  button* net_200_button_p;
  QLabel* message_p;
  button* ok_button_p
  button* help_button_p;

  QGridLayout* main_layout_p;

//  centre* centre_p;
};
#endif
