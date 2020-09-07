#include <iostream>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include "centre.hpp"
#include "totalize_options.hpp"
#include "button.hpp"

using namespace std;

totalize_options::totalize_options(centre*set_centre_p)
:screen(set_centre_p)
{
  centre_p=set_centre_p;
  
  back_button_p = new button("Back");
  macro_menu_button_p = new button("Communications Menu");
  zero_when_seed_discharged_p = new QRadioButton("Zero Count when Seed Discharged");

  number_of_barcode_columns_p = new QSpinBox;
  number_of_barcode_columns_label_p = new QLabel("Number of Barcode Columns");
  number_of_barcode_columns_group_p = new QGroupBox;
  number_of_barcode_columns_layout_p = new QHBoxLayout;
  number_of_barcode_columns_p->setMinimum(1);
  number_of_barcode_columns_p->setMaximum(4);
  number_of_barcode_columns_p->setValue(centre_p->tm_barcode_columns);

  autosave_limit_p = new QSpinBox;
  autosave_limit_label_p = new QLabel("Number of Counts before Autosave");
  autosave_limit_group_p = new QGroupBox;
  autosave_limit_layout_p = new QHBoxLayout;
  autosave_limit_p->setMinimum(1);
  autosave_limit_p->setMaximum(100);
  autosave_limit_p->setValue(centre_p->tm_autosave_count_limit);

  ok_button_p = new button("OK");

  main_layout_p=new QGridLayout;
  
  number_of_barcode_columns_layout_p->addWidget(number_of_barcode_columns_label_p);
  number_of_barcode_columns_layout_p->addWidget(number_of_barcode_columns_p);
  number_of_barcode_columns_group_p->setLayout(number_of_barcode_columns_layout_p);
  
  autosave_limit_layout_p->addWidget(autosave_limit_label_p);
  autosave_limit_layout_p->addWidget(autosave_limit_p);
  autosave_limit_group_p->setLayout(autosave_limit_layout_p);
  
  main_layout_p->addWidget(back_button_p,0,1);
  main_layout_p->addWidget(macro_menu_button_p,3,0);
  main_layout_p->addWidget(zero_when_seed_discharged_p, 0, 0);
  main_layout_p->addWidget(number_of_barcode_columns_group_p, 1, 0);
  main_layout_p->addWidget(autosave_limit_group_p, 2, 0);
  main_layout_p->addWidget(ok_button_p, 3, 1);
  
  setLayout(main_layout_p);
  
  zero_when_seed_discharged_p -> setChecked(centre_p->tm_zero_when_seed_discharged);
  number_of_barcode_columns_p -> setValue(centre_p->tm_barcode_columns);
  
  connect(back_button_p, SIGNAL(clicked()), this, SLOT(back_button_clicked()));
  connect(macro_menu_button_p, SIGNAL(clicked()), this, SLOT(macro_menu_button_clicked()));	//TEST~~~
  connect(zero_when_seed_discharged_p, SIGNAL(toggled(bool)), this, SLOT(zero_when_seed_discharged_changed(bool)));
  connect(number_of_barcode_columns_p, SIGNAL(valueChanged(int)), this, SLOT(number_of_barcode_columns_changed(int)));
  connect(autosave_limit_p, SIGNAL(valueChanged(int)), this, SLOT(autosave_limit_changed(int)));
  connect(ok_button_p, SIGNAL(clicked()), this, SLOT(ok_button_clicked()));
}

void totalize_options::back_button_clicked()
{
  centre_p->add_waiting_screen(centre_p->get_previous_screen());
  centre_p->screen_done=true;
}

void totalize_options::zero_when_seed_discharged_changed(bool value)
{
  centre_p->tm_zero_when_seed_discharged = value;
//  cout<<"centre_p->tm_zero_when_seed_discharged = "<<centre_p->tm_zero_when_seed_discharged<<endl;
}

void totalize_options::number_of_barcode_columns_changed(int value)
{
  centre_p->tm_barcode_columns = value;
}

void totalize_options::autosave_limit_changed(int value)
{
  centre_p->tm_autosave_count_limit = value;
}

void totalize_options::ok_button_clicked()
{
  centre_p->screen_done = true;
//  cout<<"ok clicked\n";
}

void totalize_options::macro_menu_button_clicked()	//TEST~~~
{
  //cout<<"macro_menu_button_click"<<endl;	//OMIT~~~
  centre_p->macro_type_for_entry = 0;//totalize macro
  centre_p->add_waiting_screen(5);//totalize
  centre_p->add_waiting_screen(28);//macro_screen
  centre_p->add_waiting_screen(40);//communications_menu
  centre_p->screen_done=true;
}

