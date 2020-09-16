#ifndef macro_builder_hpp
#define macro_builder_hpp

#include "centre.hpp"

#include <QString>

class QSignalMapper;
class QLabel;
class QLineEdit;
//class QFormLayout;
class QHBoxLayout;
class QVBoxLayout;
class QPushButton;
class QGroupBox;
class button;

class macro_builder:public screen
{
  Q_OBJECT
  
//  QString macro_function_string;
  QSignalMapper* signalMapper;
  QLabel* instruction_label_p;
  QLineEdit* lineEdit;
  QHBoxLayout *top_line_p;
  QHBoxLayout *input_line_modify;
//  QFormLayout* form_p;
  QVBoxLayout* form_p;
//  QHBoxLayout *input_line_modify;
  QHBoxLayout *bottom_line_p;
    
  QPushButton *totalize_count_button;
  QPushButton *text_entry_button;
  QPushButton *batch_count_button;
  QPushButton *seed_button;
  QPushButton *barcode_button1;
  QPushButton *barcode_button2;
  QPushButton *barcode_button3;
  QPushButton *barcode_button4;
  QPushButton *weight_button;
  QPushButton *lotcode_button;
  QPushButton *packcode_button;
  QPushButton *substitution_button;
  QPushButton *dump_count_button;
  QPushButton *newline;
  QPushButton *horizontal_tab;
  QPushButton *spacebar;
  QPushButton *arrow_up;
  QPushButton *arrow_down;
  QPushButton *arrow_left;
  QPushButton *arrow_right;
  QPushButton *clear_entry;
  QPushButton *remove_last;
  
  QGroupBox *totalize_box;
  QVBoxLayout *barcodes;
  QHBoxLayout *barcodes_columns;
  QVBoxLayout *barcodes_left;
  QVBoxLayout *barcodes_right;
  QLabel *totalize_box_label;
  
  QGroupBox *accessibility_button_box;
  QVBoxLayout *accessibility_keys_container;
  QHBoxLayout *accessibility_keys;
  QVBoxLayout *accessibility_keys_left;
  QVBoxLayout *accessibility_keys_right;
  QLabel *accessibility_box_label;
  
  button* save_button_p;
  button* back_button_p;
  
  static const int macro_button_width = 160;
  static const int macro_button_height = 35;  
  
  public:
  macro_builder(centre* centre_p);
  
  private slots:
  void dialogbox_buttons(int n);
  void back_button_clicked();
  void save_button_clicked();
};
  


#endif
