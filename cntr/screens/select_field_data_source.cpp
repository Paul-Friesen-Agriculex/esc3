#include <iostream>
#include <QVBoxLayout>
#include <QLabel>
#include "centre.hpp"
#include "batch_mode_driver.hpp"
#include "select_field_data_source.hpp"
#include "button.hpp"

using namespace std;

select_field_data_source::select_field_data_source(centre* set_centre_p, batch_mode_driver* set_batch_mode_driver_p)
:screen(set_centre_p)
{
//  cout<<"start select_field_data_source::select_field_data_source\n";
  batch_mode_driver_p = set_batch_mode_driver_p;
  message_p = new QLabel("Select type of data in the field.");
  ss_data_button_p = new button("Spreadsheet data.  Different for each envelope.");
  ss_heading_button_p = new button("Spreadsheet heading.  Same for each envelope.");
  text_entry_button_p = new button("Enter text.  Same text entered on each envelope.");
  layout_p = new QVBoxLayout;
  layout_p->addWidget(message_p);
  layout_p->addWidget(ss_data_button_p);
  layout_p->addWidget(ss_heading_button_p);
  layout_p->addWidget(text_entry_button_p);
  connect(ss_data_button_p, SIGNAL(clicked()), this, SLOT(ss_data_button_clicked()));
  connect(ss_heading_button_p, SIGNAL(clicked()), this, SLOT(ss_heading_button_clicked()));
  connect(text_entry_button_p, SIGNAL(clicked()), this, SLOT(text_entry_button_clicked()));
  setLayout(layout_p);
//  cout<<"end select_field_data_source::select_field_data_source\n";
}

void select_field_data_source::ss_data_button_clicked()
{
  batch_mode_driver_p->field_data_source_flag = 'd';
  centre_p->add_waiting_screen(34);//select_envelope_field
  centre_p->screen_done = true;
}

void select_field_data_source::ss_heading_button_clicked()
{
  batch_mode_driver_p->field_data_source_flag = 'h';
  centre_p->add_waiting_screen(34);//select_envelope_field
  centre_p->screen_done = true;
}

void select_field_data_source::text_entry_button_clicked()
{
  batch_mode_driver_p->field_data_source_flag = 't';
  centre_p->add_waiting_screen(37);//enter_field_text
  centre_p->screen_done = true;
}



