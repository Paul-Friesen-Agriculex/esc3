#ifndef serial_port_setup_hpp
#define serial_port_setup_hpp

#include "centre.hpp"
#include <QString>
class button;
class QRadioButton;
class QGroupBox;
class QVBoxLayout;
class QGridLayout;
class QLabel;
class help_screen;

class serial_port_setup : public screen
{
  Q_OBJECT
  
  public:
  serial_port_setup(centre*set_centre_p);
  ~serial_port_setup();
  
  private slots:
  void back_button_clicked();
  void done_button_clicked();

  private:
  button* back_button_p;
  QRadioButton* b9600_p;
  QRadioButton* b19200_p;
  QRadioButton* b38400_p;
  QRadioButton* b57600_p;
  QRadioButton* b115200_p;
  button* done_button_p;

  QLabel* message_p;
  QGroupBox* baud_box_p;
  QVBoxLayout* baud_layout_p;
  QGridLayout* main_layout_p;
  
};
#endif
