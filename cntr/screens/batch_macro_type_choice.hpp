#ifndef batch_macro_type_choice_hpp
#define batch_macro_type_choice_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
//class QVBoxLayout;
class QLabel;
class QRadioButton;
//class QSlider;
//class QTimer;
//class QFileDialog;
class batch_mode_driver;
//class message_box;

class batch_macro_type_choice : public screen
{
  Q_OBJECT
  
  public:
  batch_macro_type_choice(centre*set_centre_p, batch_mode_driver* set_batch_mode_driver_p);
  
  private slots:

  
  private:
  batch_mode_driver* batch_mode_driver_p;
  
  QLabel* instruction_label_p;
  button* back_button_p;
  button* pack_macro_button_p;
  button* dump_macro_button_p;
  button* substitution_macro_button_p;
  button* setup_button_p;
  button* done_button_p;
  
  QLabel* pack_label_p;
  QLabel* dump_label_p;
  QLabel* substitution_label_p;
  
  QGridLayout* main_layout_p;
  
  public slots:
  void back_button_clicked();
  void pack_macro_button_clicked();
  void dump_macro_button_clicked();
  void substitution_macro_button_clicked();
  void setup_button_clicked();
  void done_button_clicked();
};


#endif
