#ifndef select_field_data_source_hpp
#define select_field_data_source_hpp

#include "centre.hpp"

class button;
class QGridLayout;
//class QVBoxLayout;
class QLabel;

class select_field_data_source : public screen
{
  Q_OBJECT
  
  public:
  select_field_data_source(centre*set_centre_p, batch_mode_driver* set_batch_mode_driver_p);
  
  private slots:
    void back_clicked();
//    void edit_layout_only_clicked();
	  void ss_data_button_clicked();
	  void ss_heading_button_clicked();
	  void text_entry_button_clicked();
  
  private:
    button* back_button_p;
//    button* edit_layout_only_button_p;
	  QLabel* message_p;
	  button* ss_data_button_p;
	  button* ss_heading_button_p;
	  button* text_entry_button_p;
	  QGridLayout* layout_p;
    batch_mode_driver* batch_mode_driver_p;
};
#endif
