#ifndef totalize_options_hpp
#define totalize_options_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QRadioButton;
class QSpinBox;

class totalize_options : public screen
{
  Q_OBJECT
  
  public:
  totalize_options(centre*set_centre_p);
  
  private slots:
  void back_button_clicked();
  void zero_when_seed_discharged_changed(bool value);
  void number_of_barcode_columns_changed(int value);
  void autosave_limit_changed(int value);
  void ok_button_clicked();
  void macro_menu_button_clicked();	//TEST~~~

  private:
  button* back_button_p;
  button* macro_menu_button_p;	//TEST~~~
  QRadioButton* zero_when_seed_discharged_p;
  
  QSpinBox* number_of_barcode_columns_p;
  QLabel* number_of_barcode_columns_label_p;
  QGroupBox* number_of_barcode_columns_group_p;
  QHBoxLayout* number_of_barcode_columns_layout_p;
  
  QSpinBox* autosave_limit_p;
  QLabel* autosave_limit_label_p;
  QGroupBox* autosave_limit_group_p;
  QHBoxLayout* autosave_limit_layout_p;
  
  button* ok_button_p;

  QGridLayout* main_layout_p;

//  centre* centre_p;
};
#endif
