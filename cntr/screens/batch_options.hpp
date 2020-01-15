#ifndef batch_options_hpp
#define batch_options_hpp

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

class batch_options : public screen
{
  Q_OBJECT
  
  public:
  batch_options(centre*set_centre_p, batch_mode_driver* set_batch_mode_driver_p);
//  ~batch();
  
  private slots:
  void barcode_switch_toggled(bool checked);
  void macro_menu_button_clicked();	//TEST~~~

  
  private:
  batch_mode_driver* batch_mode_driver_p;
  QLabel* title_p;
  QRadioButton* require_seed_lot_barcode_p;
  QRadioButton* require_pack_barcode_p;
  QRadioButton* pack_match_lot_p;
  QRadioButton* pack_contain_lot_p;
  QRadioButton* lot_contain_pack_p;
  QRadioButton* pack_match_spreadsheet_p;
  QRadioButton* record_only_p;
  button* done_button_p;
  button* macro_menu_button_p;
  
  QGroupBox* barcode_matching_group_p;
  QGridLayout* barcode_matching_group_layout_p;
  QGridLayout* main_layout_p;
  
  public slots:
  void done_button_clicked();
};


#endif
