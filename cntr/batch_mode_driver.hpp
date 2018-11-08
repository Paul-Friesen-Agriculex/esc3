#ifndef BATCH_MODE_DRIVER_HPP
#define BATCH_MODE_DRIVER_HPP

#include <QString>
#include <QWidget>
#include <QTime>

#include "centre.hpp"

class QTimer;
class count_rate_predictor;

struct bm_set //batch mode set of packs
{
  int packs;//number of packs
  int seeds;//seeds in each pack
};

enum mode_enum
{
  wait_for_seed_lot_barcode,
  hi_open,
  low_open,
  gate_delay,
  hi_closed,
  wait_for_endgate_to_close,
  wait_for_pack,
  dump_into_cut_gate,
  dump_wait_for_endgate_to_close,
  wait_for_final_pack,
  dump_into_end,
  wait_for_dump_container,
  dump_into_container,
  wait_for_dump_container_removal
};

enum barcode_entry_mode
{
  seed_lot,
  pack
};

class batch_mode_driver : public QObject
{
  Q_OBJECT
  
	private:
  QTimer* timer_p;
  centre* centre_p;
  QTime cutoff_gate_close_time;
  static const int cutoff_gate_delay_time = 0;//milliseconds 
  static const int cutoff_gate_to_pack_removal_time = 2000;//milliseconds.  
  //After cutoff gate closes, user must wait this long before removing pack for seed to fall.
  bool pack_present;
  bool old_pack_present;
  bool pack_changed;
  QTime dump_into_cut_gate_time;
  static const int dump_into_cut_gate_time_limit = 10000;//millisecs.  Max time for dumping into cut gate
  QTime dump_into_end_time;
  static const int dump_into_end_time_limit = 20000;//millisecs. Max time for dumping into end gate
  QTime dump_end_qtime;
  static const int dump_end_qtime_limit = 2000;//millisecs.  If no change in count for this time, consider dump complete
  int old_count;
  int endgate_close_counter;
  QList<bm_set*> program;
  bm_set* set_p;
  int program_size;
  
  public:
  batch_mode_driver(centre* centre_p_s);
  ~batch_mode_driver();

  mode_enum mode;
  void load_program();//load the program indicated by program_path
  void reset_program();//start at beginning of current program
  void clear_program();
  void add_line(int packs, int seeds);
  bool program_is_empty();
  int get_program_size();
  int get_packs(int index);
  int get_seeds(int index);
  void program_remove_last();
  void start();
  void stop();
  void restart();//dump out seed and restart program

  QString program_path;
  float count_rate_multiplier;
  int high_feed_speed;
  int low_feed_speed;
  int dump_speed;
  int current_set;
  int current_pack;
  int current_count_limit;
  int current_pack_limit;
  //following numbers are for the last pack completed.  They are needed for display when current numbers have already been changed
  int pack_ready_pack;//number of the pack that has been completed in its set
  int pack_ready_count_limit;
  int pack_ready_pack_limit;
  bool discharge_next_packet;
  count_rate_predictor* count_rate_predictor_p;
  bool force_endgate_open;
  
  //batch option flags
  bool require_seed_lot_barcode;
  bool require_pack_barcode;
  bool pack_match_lot;
  bool pack_contain_lot;
  bool lot_contain_pack;
  bool record_only;
  
  //batch barcode matching data
  barcode_entry_mode barcode_mode;
  QString seed_lot_barcode;
  QString pack_barcode;
  bool seed_lot_barcode_old;//seed lot it codes is finished
  bool pack_barcode_old;//pack that was scanned is finished
  bool seed_lot_barcode_ok;
  bool pack_barcode_ok;
  
  //saving files
  QString bm_save_program_filename;//new filename just entered.  Set back to blank when file is saved.
  QString bm_save_table_filename;//new filename just entered.  Set back to blank when file is saved.
  void save_program(QString filename);
  
  void list_program();//in terminal - diagnostics
  
  private slots:
  void run();
  void set_high_feed_speed(int speed_s);
  void set_low_feed_speed(int speed_s);
  void set_dump_feed_speed(int speed_s);
  
  public slots:
  void barcode_entered(QString value);
  
  signals:
  void dumping(bool value);
  void pack_ready();
  void pack_collected(int count);
  void dump_complete(int dump_count);
  void seed_lot_barcode_entered(QString barcode);
  void pack_barcode_entered(QString barcode);
  
  //testing
  void send_message2(QString);
};
	
class count_rate_predictor : public QObject
{
  Q_OBJECT
  
  int count;
  QTimer timer;
  float smoothed_rate;
  int feed_speed;
  int old_feed_speed;
  const static float averaging_weight = .01;
  
  private slots:
  void run();
  
  public:
  count_rate_predictor();
  ~count_rate_predictor();
  void add_counts(int to_add, int feed_speed_s);
  int get_rate();
};

#endif
