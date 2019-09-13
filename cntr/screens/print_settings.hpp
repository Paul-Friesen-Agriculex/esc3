#ifndef print_settings_hpp
#define print_settings_hpp

#include "centre.hpp"

class QGridLayout;
class QGroupBox;
class QVBoxLayout;
class QLabel;
class QPushButton;
class QRadioButton;
class QCheckBox;
class rnmark_printer;

class print_settings : public screen
{
  Q_OBJECT
  
  public:
    print_settings(centre*set_centre_p);
  
  private slots:
    void check_printing_selections();
    void toggle_brother_usb_printing();
    void toggle_rnmark_ethernet_printing();
    void toggle_rnmark_serial_printing();
    void back_button_clicked();
  
  private:
    QRadioButton* brother_usb_button_p;
	  QRadioButton* rnmark_ethernet_button_p;
	  QRadioButton* rnmark_serial_button_p;
	  QPushButton* print_settings_back_button_p;
  
    QGroupBox* top_box_p;
    QGroupBox* bottom_navigation_box_p;
    QGridLayout* top_layout_p;
    QGridLayout* bottom_navigation_layout_p;    
    QVBoxLayout* main_layout_p;
};
#endif
