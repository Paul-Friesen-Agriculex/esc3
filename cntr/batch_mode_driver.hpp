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
class envelope;

struct bm_set //batch mode set of packs
{
  int packs;//number of packs
  int seeds;//seeds in each pack
};

enum mode_enum
{
  wait_for_seed_lot_barcode,
  slave_mode_entry,
  hi_open,
//  low_open,
//  hi_open_pulse,
  ramp_down,
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
  wait_for_dump_container_removal,
  substitution_wait_for_cleanout_open,
  substitution_wait_for_cleanout_close,
  substitution_wait_for_barcode, 
  cancel_substitution_wait_for_cleanout_open,
  cancel_substitution_wait_for_cleanout_close,
/*
  substitution_hi_open,
  substitution_low_open,
  substitution_gate_delay,
  substitution_hi_closed,
  substitution_wait_for_endgate_to_close,
  substitution_wait_for_pack,
  substitution_wait_for_bad_lot_cleanout
  */
};

enum barcode_entry_mode
{
  seed_lot,
  pack,
  substitution
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
  int dump_count_column;
  int substitution_column;
};

enum print_control_mode_e
{
  start_on_pack_collect,
  start_on_previous_pack_collect,
  preprint_batch
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
  static const int dump_end_qtime_limit = 1000;//millisecs.  If no change in count for this time, consider dump complete
  int old_count;
  int endgate_close_counter;
  QList<bm_set*> program;
  bm_set* set_p;
  int program_size;
  bool force_dump_out;
  envelope_feeder_brother* e_f_brother_p;
  
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
//  float count_rate_multiplier;
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
  int upper_chamber_count_limit;  //2021_03_19
  int lower_chamber_count_limit;  //2021_03_19
  
//  bool discharge_next_packet;
/*
  int slowdown_count_diff;//seed feeder slows down when count is this many seeds from limit
  bool slowdown_count_diff_set;//if this is false, run function will estimate a default starting value.  Otherwise, the set value will be used.  The set value will be adjusted continuously.
  QTime low_speed_mode_time;
  int stop_count_diff;//will stop feeder if reaches this in mode hi_closed
  */
  
  float slowdown_time;//seconds
  float pulseup_time;//seconds
  float hipulse_duration;//seconds
//  QTime hipulse_time;
  int count_rate_old_count;
  QTime count_rate_time;
  float count_rate_interval;//seconds
  float count_rate;//seeds/sec
  float hi_rate;//measure of count rate expected on high speed
  int slowdown_count_diff;//seed feeder slows down when count is this many seeds from limit
//  int pulseup_count_diff;//will produce a high speed pulse if count is farther from limit than this
  int stop_count_diff;//will stop feeder if reaches this in mode hi_closed
  int ramp_down_counter;//used to time ramp speed changes

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
  bool pack_complete;//used by batch screen to signal when it is finished
  bool use_cutgate;
  
  //batch barcode matching data
  barcode_entry_mode barcode_mode;
  QString seed_lot_barcode;
  QString pack_barcode;
  bool seed_lot_barcode_old;//seed lot it codes is finished
  bool pack_barcode_old;//pack that was scanned is finished
  bool seed_lot_barcode_ok;
  bool pack_barcode_ok;
  bool release_pack;//true signals to release counted seed, even if barcode matching not satisfied.  For use in case of lost packet.
  
  //saving files
  QString bm_save_program_filename;//new filename just entered.  Set back to blank when file is saved.
  QString bm_save_table_filename;//new filename just entered.  Includes directory info and .csv extension.  Set back to blank when file is saved.
  QString bm_last_table_filename;//stores name last used to save table.  No directory or extension.
  void save_program(QString filename);
  
  //spreadsheet handling
  bool use_spreadsheet;
  QString spreadsheet_filename;
  void load_spreadsheet(QString filename);
  int save_spreadsheet(QString filename);
  spreadsheet_column* ss_first_column_p;
  spreadsheet_column* ss_material_id_p;
  spreadsheet_column* ss_required_count_p;
  spreadsheet_column* ss_envelope_id_p;
  spreadsheet_column* ss_actual_count_p;
  spreadsheet_column* ss_print_time_p;
  spreadsheet_column* ss_fill_time_p;
  spreadsheet_column* ss_dump_count_p;
  spreadsheet_column* ss_substitution_p;
  spreadsheet_column* get_spreadsheet_column_pointer(int column_number);
  spreadsheet_column* get_spreadsheet_column_pointer(QString column_heading);
  int spreadsheet_number_of_lines;//does not include heading line
  int spreadsheet_number_of_columns;
  int spreadsheet_line_number;//number of the line for the packet being filled.  When cutgate is closed, this is the line for the sample in it.
  int end_valve_spreadsheet_line_number;//number of the line for the sample in the end gate.
  int get_next_spreadsheet_line_number();//look for next line number not filled for current seed_lot_barcode.  Return -1 if no more.
  int get_spreadsheet_line_number_after(int val);//look for line number not filled for current seed_lot_barcode, following val.  Return -1 if no more.
  int lines_left_to_fill;//number of packs for current seed_lot_bar_code still to be filled.  set by get_next_spreadsheet_line_number().
  ss_setup* ss_setup_p;
  QString ss_setup_path;
  int actual_count;//to record actual number of seeds in pack.  Might be less than required if seed is short
  void load_ss_setup();
  void clear_ss_setup();
  void fill_ss_column_pointers();
  QList<int> display_column_numbers;
  QString envelope_layout_path;
  bool ss_batch_exit_flag;

  //saving spreadsheet setup files
  QString bm_save_ss_setup_filename;//new filename just entered.  Set back to blank when file is saved.
  void save_ss_setup(QString filename);
  
  //envelope
  envelope* envelope_p;
  int sample_row;//row to be used for a sample during envelope setup
  bool print_envelope;
  void print_seed_lot_envelopes(QString mat_id);
  print_control_mode_e print_control_mode;
  char field_data_source_flag;//'d' -> data.  'h' -> heading.  't' -> text entry
  
  //extra pack handling
  bool fill_extra_pack;//setting true causes extra pack production
  bool extra_pack_filling;//signals that extra pack is filling
  int extra_pack_count_limit;
  int extra_pack_stored_count_limit;//store current_count_limit so it can be restored after extra pack
  bool extra_pack_finished;
  
  //substitute seed lot
  bool substitute_seed_lot;
  QString substitute_barcode;
  
  //slave mode
  bool slave_mode;
//  bool slave_mode_wait;
  
  //diagnostics
  void list_program();//in terminal - diagnostics
  int cout_counter;//use to count cycles to print diagnostic messages occasionally
  int cout_counter_limit;//cout_counter cycles back to zero after this number of runs
  
  private slots:
  void run();
  void set_high_feed_speed(int speed_s);
  void set_low_feed_speed(int speed_s);
  void set_dump_feed_speed(int speed_s);
  
  public slots:
  void barcode_entered(QString value);
  void cutgate_timing_error();
  void chamber_count_limit_calculation(); //calculates count limit for seed chambers for currently selected seed size //2021_03_19
  
  signals:
  void dumping();
  void pack_ready();
  void pack_collected(int count);
  void dump_complete(int dump_count);
  void seed_lot_barcode_entered(QString barcode);
  void pack_barcode_entered(QString barcode);
  void bad_lot_signal();//issued if count went over limit on this batch
  void refresh_screen();
  void send_extra_pack_message(QString message);
  void extra_pack_finished_signal();
  void slave_mode_set_finished();
  
  //testing
  void send_message2(QString);
  void send_message_time_to_end(QString);
};

#endif
