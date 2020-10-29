#include <iostream>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include "batch_mode_driver.hpp"
#include "button.hpp"

#include "ss_options.hpp"

using namespace std;

ss_options::ss_options(centre*set_centre_p, batch_mode_driver* set_batch_mode_driver_p)
:screen(set_centre_p)
{
  batch_mode_driver_p = set_batch_mode_driver_p;
  
  title_p = new QLabel;
  require_pack_barcode_p = new QRadioButton("Require pack barcode");
  pack_match_lot_p = new QRadioButton("Pack barcode must match seed lot barcode");
  pack_contain_lot_p = new QRadioButton("Pack barcode must contain seed lot barcode");
  lot_contain_pack_p = new QRadioButton("Seed lot barcode must contain pack barcode");
  pack_match_spreadsheet_p = new QRadioButton("Pack barcode must match spreadsheet");
  
  start_on_pack_collect_p = new QRadioButton("Start printing when pack collected");
  start_on_previous_pack_collect_p = new QRadioButton("Start printing when previous pack collected");
  preprint_batch_p = new QRadioButton("Preprint all envelopes when seed lot scanned");

  
  record_only_p = new QRadioButton("Record barcodes only - no matching");

  back_button_p = new button("Back");
  envelope_layout_p = new button("Envelope layout");
  column_display_p = new button("Display columns");
  macro_menu_button_p = new button("Communications Macro Menu");
  done_button_p = new button("Done");

  barcode_matching_group_p = new QGroupBox;
  barcode_matching_group_layout_p = new QGridLayout;
  
  barcode_matching_group_layout_p -> addWidget(pack_match_lot_p, 0, 0);
  barcode_matching_group_layout_p -> addWidget(pack_contain_lot_p, 1, 0);
  barcode_matching_group_layout_p -> addWidget(lot_contain_pack_p, 2, 0);
  barcode_matching_group_layout_p -> addWidget(pack_match_spreadsheet_p, 0, 1);
  barcode_matching_group_layout_p -> addWidget(record_only_p, 1, 1);
  barcode_matching_group_p -> setLayout(barcode_matching_group_layout_p);
  
  print_control_group_p = new QGroupBox;
  print_control_group_layout_p = new QGridLayout;
  
  print_control_group_layout_p -> addWidget(start_on_pack_collect_p, 0, 0);
  print_control_group_layout_p -> addWidget(start_on_previous_pack_collect_p, 1, 0);
  print_control_group_layout_p -> addWidget(preprint_batch_p, 2, 0);
  print_control_group_p -> setLayout(print_control_group_layout_p);
  print_control_group_p -> setTitle("Print control");
  
  main_layout_p = new QGridLayout;
  main_layout_p -> addWidget(require_pack_barcode_p, 0, 0);
  main_layout_p -> addWidget(back_button_p, 0, 2);
  main_layout_p -> addWidget(barcode_matching_group_p, 1, 0, 1, 3);
  main_layout_p -> addWidget(print_control_group_p, 2, 0, 3, 1);
  main_layout_p -> addWidget(envelope_layout_p, 2, 1);
  main_layout_p -> addWidget(column_display_p, 3, 1);
  main_layout_p -> addWidget(macro_menu_button_p, 2, 2);
  main_layout_p -> addWidget(done_button_p, 3, 2);
  setLayout(main_layout_p);
  
  require_pack_barcode_p -> setAutoExclusive(false);
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(start_on_pack_collect_p, SIGNAL(toggled(bool)), this, SLOT(start_on_pack_collect_clicked(bool)));
  connect(start_on_previous_pack_collect_p, SIGNAL(toggled(bool)), this, SLOT(start_on_previous_pack_collect_clicked(bool)));
  connect(preprint_batch_p, SIGNAL(toggled(bool)), this, SLOT(preprint_batch_clicked(bool)));
  connect(envelope_layout_p, SIGNAL(clicked()), this, SLOT(envelope_layout_button_clicked()));
  connect(done_button_p, SIGNAL(clicked()), this, SLOT(done_button_clicked()));
  connect(column_display_p, SIGNAL(clicked()), this, SLOT(column_display_button_clicked()));
  connect(macro_menu_button_p, SIGNAL(clicked()), this, SLOT(macro_menu_button_clicked()));	

  require_pack_barcode_p -> setChecked(batch_mode_driver_p->require_pack_barcode);
  pack_match_lot_p -> setChecked(batch_mode_driver_p->pack_match_lot);
  pack_contain_lot_p -> setChecked(batch_mode_driver_p->pack_contain_lot);
  lot_contain_pack_p -> setChecked(batch_mode_driver_p->lot_contain_pack);
  pack_match_spreadsheet_p -> setChecked(batch_mode_driver_p->pack_match_spreadsheet);
  record_only_p -> setChecked(batch_mode_driver_p->record_only);
  
  if(batch_mode_driver_p->print_control_mode == start_on_pack_collect)
  {
    start_on_pack_collect_p->setChecked(true);
  }
  if(batch_mode_driver_p->print_control_mode == start_on_previous_pack_collect)
  {
    start_on_previous_pack_collect_p->setChecked(true);
  }
  if(batch_mode_driver_p->print_control_mode == preprint_batch)
  {
    preprint_batch_p->setChecked(true);
  }
  
  if(batch_mode_driver_p->print_envelope == false)
  {
    start_on_pack_collect_p->hide();
    start_on_previous_pack_collect_p->hide();
    preprint_batch_p->hide();
    envelope_layout_p->hide();
  }
}


void ss_options::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void ss_options::done_button_clicked()
{
  batch_mode_driver_p->require_pack_barcode = require_pack_barcode_p->isChecked();
  batch_mode_driver_p->pack_match_lot = pack_match_lot_p->isChecked();
  batch_mode_driver_p->pack_contain_lot = pack_contain_lot_p->isChecked();
  batch_mode_driver_p->lot_contain_pack = lot_contain_pack_p->isChecked();
  batch_mode_driver_p->pack_match_spreadsheet = pack_match_spreadsheet_p->isChecked();
  batch_mode_driver_p->record_only = record_only_p->isChecked();
  
  centre_p->screen_done = true;
}

void ss_options::envelope_layout_button_clicked()
{
  centre_p->add_waiting_screen(25);
  centre_p->screen_done = true;
}

void ss_options::column_display_button_clicked()
{
  batch_mode_driver_p->require_pack_barcode = require_pack_barcode_p->isChecked();
  batch_mode_driver_p->pack_match_lot = pack_match_lot_p->isChecked();
  batch_mode_driver_p->pack_contain_lot = pack_contain_lot_p->isChecked();
  batch_mode_driver_p->lot_contain_pack = lot_contain_pack_p->isChecked();
  batch_mode_driver_p->pack_match_spreadsheet = pack_match_spreadsheet_p->isChecked();
  batch_mode_driver_p->record_only = record_only_p->isChecked();
  centre_p -> add_waiting_screen(32);//ss_column_display_options
  centre_p -> screen_done = true;
}  
  
void ss_options::macro_menu_button_clicked()
{
  centre_p->add_waiting_screen(33);//back to ss_batch
  centre_p->add_waiting_screen(50);//batch_macro_type_choice
  centre_p->screen_done=true;
}

void ss_options::start_on_pack_collect_clicked(bool val)
{
  if(val) batch_mode_driver_p -> print_control_mode = start_on_pack_collect;
}

void ss_options::start_on_previous_pack_collect_clicked(bool val)
{
  if(val) batch_mode_driver_p -> print_control_mode = start_on_previous_pack_collect;
}

void ss_options::preprint_batch_clicked(bool val)
{
  if(val) batch_mode_driver_p -> print_control_mode = preprint_batch;
}

