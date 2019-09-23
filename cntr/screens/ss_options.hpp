#ifndef ss_options_hpp
#define ss_options_hpp

#include "centre.hpp"

class button;
//class QGridLayout;
class QGroupBox;
//class QVBoxLayout;
class QGridLayout;
class QLabel;
class QRadioButton;
//class QSlider;
//class QTimer;
//class QFileDialog;
class batch_mode_driver;
//class message_box;

class ss_options : public screen
{
  Q_OBJECT
  
  public:
  ss_options(centre*set_centre_p, batch_mode_driver* set_batch_mode_driver_p);
//  ~batch();
  
  private slots:
  void back_button_clicked();
  void envelope_layout_button_clicked();
  void column_display_button_clicked();
  void macro_menu_button_clicked();
  
  void start_on_pack_collect_clicked(bool val);
  void start_on_previous_pack_collect_clicked(bool val);
  void preprint_batch_clicked(bool val);
  

  
  private:
  batch_mode_driver* batch_mode_driver_p;
  QLabel* title_p;
//  QRadioButton* require_seed_lot_barcode_p;
  QRadioButton* require_pack_barcode_p;
  QRadioButton* pack_match_lot_p;
  QRadioButton* pack_contain_lot_p;
  QRadioButton* lot_contain_pack_p;
  QRadioButton* pack_match_spreadsheet_p;
  QRadioButton* record_only_p;
  
  QRadioButton* start_on_pack_collect_p;
  QRadioButton* start_on_previous_pack_collect_p;
  QRadioButton* preprint_batch_p;
  
  button* back_button_p;
  button* envelope_layout_p;
  button* column_display_p;
  button* macro_menu_button_p;
  button* done_button_p;
  
  QGroupBox* barcode_matching_group_p;
  QGroupBox* print_control_group_p;
  QGridLayout* barcode_matching_group_layout_p;
  QGridLayout* print_control_group_layout_p;
  QGridLayout* main_layout_p;
  
  
  public slots:
  void done_button_clicked();
};


#endif
