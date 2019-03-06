#ifndef BATCH_MODE_DRIVER_HPP
#define BATCH_MODE_DRIVER_HPP

#include <QString>
#include <QList>
#include <QWidget>
#include <QTime>

#include "centre.hpp"
#include "cutgate.hpp"
#include "envelope.hpp"

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
  wait_for_bad_lot_cleanout,
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

struct spreadsheet_column
{
  QString heading;
  QList <QString> data_list;
  spreadsheet_column* next;
};

struct ss_setup
{
  int material_id_column;
  int required_count_column;
  int envelope_id_column;
  int actual_count_column;
  int print_time_column;
  int fill_time_column;
  QString envelope_setup_file;
};
  

class batch_mode_driver : public QObject
{
  Q_OBJECT
  
	private:
  QTimer* timer_p;
  centre* centre_p;
  cutgate* cutgate_p;
  QTime cutoff_gate_close_time;
  static const int cutoff_gate_delay_time = 0;//milliseconds 
  static const int cutoff_gate_to_pack_removal_time = 2000;//milliseconds.  
  //After cutoff gate closes, user must wait this long before removing pack for seed to fall.
  bool pack_present;
  bool old_pack_present;
  bool pack_changed;
  QTime dump_into_cut_gate_time;
  static const int dump_into_cut_gate_time_limit = 2000;//millisecs.  Max time for dumping into cut gate
  QTime dump_into_end_time;
  static const int dump_into_end_time_limit = 4000;//millisecs. Max time for dumping into end gate
  QTime dump_end_qtime;
  static const int dump_end_qtime_limit = 2000;//millisecs.  If no change in count for this time, consider dump complete
  int old_count;
  int endgate_close_counter;
  QList<bm_set*> program;
  bm_set* set_p;
  int program_size;
  bool force_dump_out;
  
  //testing
//  int print_message_count;
  
  
  public:
  batch_mode_driver(centre* centre_p_s, cutgate* cutgate_p_s);
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
  bool pack_match_spreadsheet;
  bool record_only;
  bool next_seed_lot_bad;//used to signal count went over limit
  
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
  
  //spreadsheet handling
  bool use_spreadsheet;
  int spreadsheet_line;//line corresponding to pack currently being filled
  QString spreadsheet_filename;
  void load_spreadsheet(QString filename);
  spreadsheet_column* ss_first_column_p;
  spreadsheet_column* ss_material_id_p;
  spreadsheet_column* ss_required_count_p;
  spreadsheet_column* ss_envelope_id_p;
  spreadsheet_column* ss_actual_count_p;
  spreadsheet_column* ss_print_time_p;
  spreadsheet_column* ss_fill_time_p;
  spreadsheet_column* get_spreadsheet_column_pointer(int column_number);
  int spreadsheet_number_of_lines;//does not include heading line
  int spreadsheet_line_number;//number of the line for the packet being filled
  int get_next_spreadsheet_line_number();//look for next line number not filled for current seed_lot_barcode.  Return -1 if no more.
  int lines_left_to_fill;//number of packs for current seed_lot_bar_code still to be filled.  set by get_next_spreadsheet_line_number().
  QList<bool> pack_filled;//used to mark when a packet has been filled
  ss_setup* ss_setup_p;
  envelope* envelope_p;
  QString ss_setup_path;
  void load_ss_setup();
  void clear_ss_setup();
  void fill_ss_column_pointers();
  QString envelope_layout_path;
  void load_envelope_layout();
  void clear_envelope_layout();

  //saving spreadsheet setup files
  QString bm_save_ss_setup_filename;//new filename just entered.  Set back to blank when file is saved.
  void save_ss_setup(QString filename);
  
  
  void list_program();//in terminal - diagnostics
  
  private slots:
  void run();
  void set_high_feed_speed(int speed_s);
  void set_low_feed_speed(int speed_s);
  void set_dump_feed_speed(int speed_s);
  
  public slots:
  void barcode_entered(QString value);
  void cutgate_timing_error();
  
  signals:
  void dumping(bool value);
  void pack_ready();
  void pack_collected(int count);
  void dump_complete(int dump_count);
  void seed_lot_barcode_entered(QString barcode);
  void pack_barcode_entered(QString barcode);
  
  //testing
  void send_message2(QString);
  void send_message_time_to_end(QString);
};
	
class count_rate_predictor : public QObject
{
  Q_OBJECT
  
  centre* centre_p;
  float count_rate_multiplier;
  QTimer timer;
  int old_count;
  const static float averaging_weight = .01;
  
  private slots:
  void run();
  
  public:
  count_rate_predictor(centre* centre_p_s);
  ~count_rate_predictor();
//  void add_counts(int to_add, int feed_speed_s);
  void set_initial_count_rate_multiplier(float multiplier_s) {count_rate_multiplier = multiplier_s;};
  float get_rate();
  
  signals:
  void send_message(QString message);
};
#endif
