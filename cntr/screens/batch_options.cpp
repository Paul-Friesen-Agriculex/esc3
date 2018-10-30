#include <iostream>
#include "button.hpp"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include "batch_mode_driver.hpp"

#include "batch_options.hpp"

using namespace std;

batch_options::batch_options(centre*set_centre_p, batch_mode_driver* set_batch_mode_driver_p)
:screen(set_centre_p)
{
  batch_mode_driver_p = set_batch_mode_driver_p;
  
  title_p = new QLabel;
  require_seed_lot_barcode_p = new QRadioButton("Require seed lot barcode");
  require_pack_barcode_p = new QRadioButton("Require pack barcode");
  pack_match_lot_p = new QRadioButton("Pack barcode must match seed lot barcode");
  pack_contain_lot_p = new QRadioButton("Pack barcode must contain seed lot barcode");
  lot_contain_pack_p = new QRadioButton("Seed lot barcode must contain pack barcode");
  record_only_p = new QRadioButton("Record barcodes only - no matching");
  done_button_p = new button("Done");
  macro_menu_button_p = new button("Macro Menu (test)");	//TEST~~~

  
  barcode_matching_group_p = new QGroupBox;
  barcode_matching_group_layout_p = new QVBoxLayout;
  main_layout_p = new QVBoxLayout;
  
  barcode_matching_group_layout_p -> addWidget(pack_match_lot_p);
  barcode_matching_group_layout_p -> addWidget(pack_contain_lot_p);
  barcode_matching_group_layout_p -> addWidget(lot_contain_pack_p);
  barcode_matching_group_layout_p -> addWidget(record_only_p);
  barcode_matching_group_p -> setLayout(barcode_matching_group_layout_p);
  
  main_layout_p -> addWidget(require_seed_lot_barcode_p);
  main_layout_p -> addWidget(require_pack_barcode_p);
  main_layout_p -> addWidget(barcode_matching_group_p);
  main_layout_p -> addWidget(done_button_p);
  main_layout_p -> addWidget(macro_menu_button_p);	//TEST~~~
  setLayout(main_layout_p);
  
  require_seed_lot_barcode_p -> setAutoExclusive(false);
  require_pack_barcode_p -> setAutoExclusive(false);
  
  connect(done_button_p, SIGNAL(clicked()), this, SLOT(done_button_clicked()));
  connect(macro_menu_button_p, SIGNAL(clicked()), this, SLOT(macro_menu_button_clicked()));	//TEST~~~

  require_seed_lot_barcode_p -> setChecked(batch_mode_driver_p->require_seed_lot_barcode);
  require_pack_barcode_p -> setChecked(batch_mode_driver_p->require_pack_barcode);
  pack_match_lot_p -> setChecked(batch_mode_driver_p->pack_match_lot);
  pack_contain_lot_p -> setChecked(batch_mode_driver_p->pack_contain_lot);
  lot_contain_pack_p -> setChecked(batch_mode_driver_p->lot_contain_pack);
  record_only_p -> setChecked(batch_mode_driver_p->record_only);
}

void batch_options::done_button_clicked()
{
  batch_mode_driver_p->require_seed_lot_barcode = require_seed_lot_barcode_p->isChecked();
  batch_mode_driver_p->require_pack_barcode = require_pack_barcode_p->isChecked();
  batch_mode_driver_p->pack_match_lot = pack_match_lot_p->isChecked();
  batch_mode_driver_p->pack_contain_lot = pack_contain_lot_p->isChecked();
  batch_mode_driver_p->lot_contain_pack = lot_contain_pack_p->isChecked();
  batch_mode_driver_p->record_only = record_only_p->isChecked();
  
  
  cout<<"batch_options::done_button_clicked.  about to list program\n";
  batch_mode_driver_p->list_program();
  
  
  centre_p->screen_done = true;
}
  
void batch_options::macro_menu_button_clicked()	//TEST~~~
{
  cout<<"macro_menu_button_click"<<endl;	//OMIT~~~
  centre_p->add_waiting_screen(28);
  centre_p->screen_done=true;
}
