#ifndef windows_tcp_printer_hpp
#define windows_tcp_printer_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QLabel;
class QSpinBox;
//class help_screen;

class windows_tcp_printer : public screen
{
  Q_OBJECT
  
  public:
  windows_tcp_printer(centre*set_centre_p);
  ~windows_tcp_printer();
  
  private slots:
  void back_button_clicked();
  void help_button_clicked();
  void create_ipv4_groupbox();
  void create_port_groupbox();
  void initial_ipv4();
  void initial_port();
  void get_ipv4();
  void set_ipv4();
  void get_port();
  void set_port();
  void check_socket_status();
  void connect_windows_tcp();
  void disconnect_windows_tcp();
  void connection_detected();	//2020_12_16
  void socket_status_ping();	//2020_12_16 WIP~~~//
  void change_network();	//2020_12_17 test to programmatically switching/settings IPv4 addresses
  
  private:
  button* back_button_p;
  button* help_button_p;
  button* connect_button_p;
  button* disconnect_button_p;

  QSpinBox* addr1_p;
  QSpinBox* addr2_p;
  QSpinBox* addr3_p;
  QSpinBox* addr4_p;
  QSpinBox* addr5_p;
  QSpinBox* port1_p;
  QSpinBox* port2_p;
  QSpinBox* port3_p;
  QSpinBox* port4_p;
  QSpinBox* port5_p;
  
  QGroupBox* main_groupbox_p;
  QGroupBox* ipv4_groupbox_p;
  QGroupBox* port_groupbox_p;
  
  QGridLayout* ipv4_layout_p;
  QGridLayout* port_layout_p;  
  QGridLayout* main_layout_p;
  
  int default_windows_printer_port;
  int windows_printer_port;
  QString default_windows_printer_ipv4;
  QString windows_printer_ipv4;
  
  QLabel* connection_header_label_p;
  QLabel* connection_status_label_p;
};
#endif

