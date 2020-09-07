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
//    QPushButton *batch_count_button = new QPushButton(tr("Count"), this);
//    QPushButton *seed_button = new QPushButton(tr("Seed name"), this);
  QPushButton *barcode_button1;
  QPushButton *barcode_button2;
  QPushButton *barcode_button3;
  QPushButton *barcode_button4;
//    QPushButton *weight_button = new QPushButton(tr("Weight"), this);		      //yet to be implemented
//    QPushButton *lotcode_button = new QPushButton(tr("Lot Code"), this);
//    QPushButton *packcode_button = new QPushButton(tr("Pack Code"), this);
//    QPushButton *substitution_button = new QPushButton(tr("Substitution"), this);
//    QPushButton *dump_count_button = new QPushButton(tr("Dump Count"), this);
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
  
  static const int macro_button_width = 135;
  static const int macro_button_height = 35;  
  
  public:
  macro_builder(centre* centre_p);
  
  private slots:
  void dialogbox_buttons(int n);
  void back_button_clicked();
  void save_button_clicked();
};
  


#endif
