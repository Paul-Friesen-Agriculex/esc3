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
//  pack_match_spreadsheet_p = new QRadioButton("Pack barcode must match spreadsheet");
  record_only_p = new QRadioButton("Record barcodes only - no matching");
  done_button_p = new button("Done");
  macro_menu_button_p = new button("Macro Menu (test)");	//TEST~~~

  
  barcode_matching_group_p = new QGroupBox;
  barcode_matching_group_layout_p = new QGridLayout;
  main_layout_p = new QGridLayout;
  
  barcode_matching_group_layout_p -> addWidget(pack_match_lot_p,0,0);
  barcode_matching_group_layout_p -> addWidget(pack_contain_lot_p,0,1);
  barcode_matching_group_layout_p -> addWidget(lot_contain_pack_p,1,0);
//  barcode_matching_group_layout_p -> addWidget(pack_match_spreadsheet_p);
  barcode_matching_group_layout_p -> addWidget(record_only_p,1,1);
  barcode_matching_group_p -> setLayout(barcode_matching_group_layout_p);
  
  main_layout_p -> addWidget(require_seed_lot_barcode_p,0,0);
  main_layout_p -> addWidget(require_pack_barcode_p,0,1);
  main_layout_p -> addWidget(barcode_matching_group_p,1,0,1,2);
  main_layout_p -> addWidget(done_button_p,2,1);
//  main_layout_p -> addWidget(macro_menu_button_p);	//TEST~~~
  setLayout(main_layout_p);
  
  require_seed_lot_barcode_p -> setAutoExclusive(false);
  require_pack_barcode_p -> setAutoExclusive(false);
  
  connect(done_button_p, SIGNAL(clicked()), this, SLOT(done_button_clicked()));
  connect(require_seed_lot_barcode_p, SIGNAL(toggled(bool)), this, SLOT(barcode_switch_toggled(bool)));
  connect(require_pack_barcode_p, SIGNAL(toggled(bool)), this, SLOT(barcode_switch_toggled(bool)));
  connect(macro_menu_button_p, SIGNAL(clicked()), this, SLOT(macro_menu_button_clicked()));	//TEST~~~

  require_seed_lot_barcode_p -> setChecked(batch_mode_driver_p->require_seed_lot_barcode);
  require_pack_barcode_p -> setChecked(batch_mode_driver_p->require_pack_barcode);
  pack_match_lot_p -> setChecked(batch_mode_driver_p->pack_match_lot);
  pack_contain_lot_p -> setChecked(batch_mode_driver_p->pack_contain_lot);
  lot_contain_pack_p -> setChecked(batch_mode_driver_p->lot_contain_pack);
//  pack_match_spreadsheet_p -> setChecked(batch_mode_driver_p->pack_match_spreadsheet);
  record_only_p -> setChecked(batch_mode_driver_p->record_only);
}

void batch_options::done_button_clicked()
{
  batch_mode_driver_p->require_seed_lot_barcode = require_seed_lot_barcode_p->isChecked();
  batch_mode_driver_p->require_pack_barcode = require_pack_barcode_p->isChecked();
  batch_mode_driver_p->pack_match_lot = pack_match_lot_p->isChecked();
  batch_mode_driver_p->pack_contain_lot = pack_contain_lot_p->isChecked();
  batch_mode_driver_p->lot_contain_pack = lot_contain_pack_p->isChecked();
//  batch_mode_driver_p->pack_match_spreadsheet = pack_match_spreadsheet_p->isChecked();
  batch_mode_driver_p->record_only = record_only_p->isChecked();
  
  
  cout<<"batch_options::done_button_clicked.  about to list program\n";
//  batch_mode_driver_p->list_program();
  
  
  centre_p->screen_done = true;
}

void batch_options::barcode_switch_toggled(bool checked)
{
  if(checked == false)
  {
    record_only_p->setChecked(true);
  }
  bool barcode_matching_possible = (  require_seed_lot_barcode_p->isChecked()  &&  require_pack_barcode_p->isChecked()  );
  pack_match_lot_p->setEnabled(barcode_matching_possible);
  pack_contain_lot_p->setEnabled(barcode_matching_possible);
  lot_contain_pack_p->setEnabled(barcode_matching_possible);
}
  
void batch_options::macro_menu_button_clicked()	//TEST~~~
{
  cout<<"macro_menu_button_click"<<endl;	//OMIT~~~
  centre_p->enter_totalize_macro = true;
  centre_p->add_waiting_screen(28);//macro_screen
  centre_p->screen_done=true;
}
