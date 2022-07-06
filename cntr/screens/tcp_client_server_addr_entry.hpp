#ifndef tcp_client_server_addr_entry_hpp
#define tcp_client_server_addr_entry_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QRadioButton;
class QSpinBox;

class tcp_client_server_addr_entry : public screen
{
  Q_OBJECT
  
  public:
  tcp_client_server_addr_entry(centre*set_centre_p);
  
  private slots:
  void back_button_clicked();
  void connect_button_clicked();
  void ok_button_clicked();
  void help_button_clicked();
  void connection_detected();
  void retrieve_connection_ip();
  void diagnostic_button_clicked();  //2022_01_20

  private:
  button* back_button_p;
  button* connect_button_p;
  QSpinBox* addr1_p;
  QSpinBox* addr2_p;
  QSpinBox* addr3_p;
  QSpinBox* addr4_p;
  QLabel* message_p;
  button* ok_button_p;
  button* help_button_p;
  button* diagnostics_button_p; //2022_01_20

  QGridLayout* main_layout_p;
  QString default_ip_addr;
  
//  centre* centre_p;
};
#endif
