#ifndef ss_batch_hpp
#define ss_batch_hpp

//#include "batch_table.hpp"
#include "ss_batch_table.hpp"
#include "centre.hpp"
#include "batch_mode_driver.hpp"
#include "macro_screen.hpp"
//#include "batch.hpp"

#include <QLabel>
#include <QTextEdit>  //ORIGINAL~~~
#include <QLineEdit>  //TEST~~~


class button;
class QGridLayout;
class QGroupBox;
class QVBoxLayout;
class QSlider;
class QTimer;
class QFileDialog;
class message_box;

class ss_barcode_line : public QLineEdit  
{
  Q_OBJECT
  
  protected:
  void keyPressEvent(QKeyEvent* event);
  
  signals:
  void barcode_entered(QString barcode);
};

enum ss_end_valve_mode_enum
{
  ss_closed_empty,
  ss_opened_while_empty,
  ss_closed_filling,
  ss_closed_full,
  ss_open_emptying,
  ss_open_pass_through,
  ss_open_empty,
  ss_pack_removed_too_soon,
  ss_closed_bad_lot,
  ss_open_emptying_bad_lot,
  ss_dump_closed_empty,
  ss_dump_opened_while_empty,
  ss_dump_closed_filling,
  ss_dump_pass_through,
  ss_dump_open_empty,
  ss_dump_container_removed_too_soon
};

class manual_operation_window;

class ss_batch : public screen
{
  Q_OBJECT
  
  public:
  ss_batch(centre*set_centre_p, batch_mode_driver* set_batch_mode_driver_p);
  ~ss_batch();
  batch_mode_driver* batch_mode_driver_p;
  
  public slots:
//  void pack_ready();
  void pack_collected(int count);
  void dump_complete(int dump_count);
//  void dump_complete();
  void dumping();
  void focus_on_barcode();
  void bad_lot_slot();
  void refresh_screen();
  
  private slots:
  void options_clicked();
  void back_clicked();
  void rescan_clicked();
  void restart_clicked();
  void save_program_clicked();
  void reprint_button_1_clicked();
  void reprint_button_2_clicked();
  void manual_print_button_clicked();
  void quit_clicked();
  void run();
  
  private:
  QLabel* count_message_p;
  button* options_button_p;
  button* back_button_p;
  ss_barcode_line* barcode_line_p;
  button* rescan_button_p;
  button* restart_button_p;
  QLabel* high_speed_label_p;
  QLabel* low_speed_label_p;
  QLabel* dump_speed_label_p;
  QSlider* high_speed_set_p;
  QSlider* low_speed_set_p;
  QSlider* dump_speed_set_p;
//  batch_table* table_p;
  ss_batch_table* ss_table_p;
  message_box* status_box_p;
  button* save_program_button_p;
  button* reprint_button_1_p;
  button* reprint_button_2_p;
  button* manual_print_button_p;
  button* quit_button_p;
  message_box* barcode_status_p;
  
  QGroupBox* top_box_p;
  QGroupBox* control_box_p;
  QGroupBox* speed_box_p;
  QGroupBox* bottom_box_p;
    
  QGridLayout* top_layout_p;
  QGridLayout* control_layout_p;
  QGridLayout* speed_layout_p;
  QGridLayout* bottom_layout_p;
  QGridLayout* main_layout_p;
  
  bool feeder_is_running;
  ENDGATE_STATE screen_endgate;//state of endgate according to screen control.
    //If this is open, endgate will be open regardless of envelope sensor.
    //If this is closed, it will be over-ridden by presence of an envelope
  
  int count;
  
  mode_enum mode;
  mode_enum old_mode;
  ss_end_valve_mode_enum end_valve_mode;
  ss_end_valve_mode_enum old_end_valve_mode;
  int end_valve_empty_counter;//use to time valve emptying
  
  
  
//  bool pack_was_placed;
//  bool pack_was_removed;
//  bool seed_for_pack_ready;
//  bool pack_can_be_removed;
  QString pack_ready_message;
//  bool ss_pack_removed_too_soon;
  QString pack_removed_too_soon_message;
//  bool dump_container_was_placed;
//  bool dump_container_can_be_removed;
  QString dump_container_needed_message;
  QString dump_container_ready_message;
//  int dump_container_ready_count;
//  bool ss_dump_container_removed_too_soon;
  QString dump_container_removed_too_soon_message;
  QString bad_seed_lot_message;
//  bool dump_flag;//true -> dumping
  
  barcode_entry_mode old_barcode_mode;
  
  int reprint_line_1;//line number that will be reprinted on envelope if reprint button 1 is clicked
  int reprint_line_2;//line number that will be reprinted on envelope if reprint button 2 is clicked
  int reprint_line_manual;//line number that will be reprinted on envelope if manual reprint button is clicked
  void update_reprint_buttons();
  
  macro_screen* macro_screen_p;
  QString count_string;				//seed count access from gpio_keyboard
  QTimer* run_timer_p;
  
  manual_operation_window* manual_operation_window_p;
};

class manual_operation_window : public QWidget
{
  Q_OBJECT
  
  public:
  manual_operation_window(QWidget* parent, batch_mode_driver* batch_mode_driver_p_s);
  
  private:
  batch_mode_driver* batch_mode_driver_p;
  
  button* previous_p;
  button* print_line_1_p;
  button* print_line_2_p;
  button* print_line_3_p;
  button* print_line_4_p;
  button* print_line_5_p;
  button* repeat_line_1_p;
  button* repeat_line_2_p;
  button* repeat_line_3_p;
  button* repeat_line_4_p;
  button* repeat_line_5_p;
  button* next_p;
  button* close_p;
  
  QGridLayout* layout_p;
  
  int first_line;
  
  void update_buttons();
  
  private slots:
  void previous_clicked();
  void print_line_1_clicked();
  void print_line_2_clicked();
  void print_line_3_clicked();
  void print_line_4_clicked();
  void print_line_5_clicked();
  void repeat_line_1_clicked();
  void repeat_line_2_clicked();
  void repeat_line_3_clicked();
  void repeat_line_4_clicked();
  void repeat_line_5_clicked();
  void next_clicked();
};

#endif

