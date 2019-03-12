#ifndef batch_hpp
#define batch_hpp

#include "batch_table.hpp"
#include "centre.hpp"
#include "batch_mode_driver.hpp"
#include "macro_screen.hpp"

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

class barcode_line : public QLineEdit  
{
  Q_OBJECT
  
  protected:
  void keyPressEvent(QKeyEvent* event);
  
  signals:
  void barcode_entered(QString barcode);
};

enum end_valve_mode_enum
{
  closed_empty,
  opened_while_empty,
  closed_filling,
  closed_full,
  open_emptying,
  open_pass_through,
  open_empty,
  pack_removed_too_soon,
  closed_bad_lot,
  open_emptying_bad_lot,
  dump_closed_empty,
  dump_opened_while_empty,
  dump_closed_filling,
  dump_pass_through,
  dump_open_empty,
  dump_container_removed_too_soon
};

class batch : public screen
{
  Q_OBJECT
  
  public:
  batch(centre*set_centre_p, batch_mode_driver* set_batch_mode_driver_p);
  ~batch();
  batch_mode_driver* batch_mode_driver_p;
  
  public slots:
//  void pack_ready();
  void pack_collected(int count);
  void dump_complete(int dump_count);
  void dumping();
  void focus_on_barcode();
  void bad_lot_slot();
  
  private slots:
  void options_clicked();
  void back_clicked();
  void rescan_clicked();
  void restart_clicked();
  void save_program_clicked();
  void save_table_clicked();
  void clear_table_clicked();
  void quit_clicked();
  void run();
  
  private:
  QLabel* count_message_p;
  button* options_button_p;
  button* back_button_p;
  barcode_line* barcode_line_p;
  button* rescan_button_p;
  button* restart_button_p;
  QLabel* high_speed_label_p;
  QLabel* low_speed_label_p;
  QLabel* dump_speed_label_p;
  QSlider* high_speed_set_p;
  QSlider* low_speed_set_p;
  QSlider* dump_speed_set_p;
  batch_table* table_p;
  message_box* status_box_p;
  button* save_program_button_p;
  button* save_table_button_p;
  button* clear_table_button_p;
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
  end_valve_mode_enum end_valve_mode;
  end_valve_mode_enum old_end_valve_mode;
  int end_valve_empty_counter;//use to time valve emptying
  
  
  
//  bool pack_was_placed;
//  bool pack_was_removed;
//  bool seed_for_pack_ready;
//  bool pack_can_be_removed;
  QString pack_ready_message;
//  bool pack_removed_too_soon;
  QString pack_removed_too_soon_message;
//  bool dump_container_was_placed;
//  bool dump_container_can_be_removed;
  QString dump_container_needed_message;
  QString dump_container_ready_message;
//  int dump_container_ready_count;
//  bool dump_container_removed_too_soon;
  QString dump_container_removed_too_soon_message;
  QString bad_seed_lot_message;
//  bool dump_flag;//true -> dumping
  
  barcode_entry_mode old_barcode_mode;
  
  macro_screen* macro_screen_p;
  QString count_string;				//seed count access from gpio_keyboard
  QTimer* run_timer_p;
};

#endif

