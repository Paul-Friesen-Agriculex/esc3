#include <iostream>
#include <QPushButton>
#include <QString>
#include <QLabel>
#include <QGridLayout>

#include "keypad.hpp"

keypad::keypad()
{
  entry_line_p = new QString("");
  
  display_p = new QLabel;
  n0_button_p = new QPushButton("0");
  n0_button_p -> setMaximumWidth(50);
  n1_button_p = new QPushButton("1");
  n1_button_p -> setMaximumWidth(50);
  n2_button_p = new QPushButton("2");
  n2_button_p -> setMaximumWidth(50);
  n3_button_p = new QPushButton("3");
  n3_button_p -> setMaximumWidth(50);
  n4_button_p = new QPushButton("4");
  n4_button_p -> setMaximumWidth(50);
  n5_button_p = new QPushButton("5");
  n5_button_p -> setMaximumWidth(50);
  n6_button_p = new QPushButton("6");
  n6_button_p -> setMaximumWidth(50);
  n7_button_p = new QPushButton("7");
  n7_button_p -> setMaximumWidth(50);
  n8_button_p = new QPushButton("8");
  n8_button_p -> setMaximumWidth(50);
  n9_button_p = new QPushButton("9");
  n9_button_p -> setMaximumWidth(50);
  enter_button_p = new QPushButton("Enter");
  enter_button_p -> setMaximumWidth(50);
  
  layout_p = new QGridLayout;
  layout_p -> addWidget(display_p, 0, 0, 1, 3);
  layout_p -> addWidget(n7_button_p, 1, 0);
  layout_p -> addWidget(n8_button_p, 1, 1);
  layout_p -> addWidget(n9_button_p, 1, 2);
  layout_p -> addWidget(n4_button_p, 2, 0);
  layout_p -> addWidget(n5_button_p, 2, 1);
  layout_p -> addWidget(n6_button_p, 2, 2);
  layout_p -> addWidget(n1_button_p, 3, 0);
  layout_p -> addWidget(n2_button_p, 3, 1);
  layout_p -> addWidget(n3_button_p, 3, 2);
  layout_p -> addWidget(n0_button_p, 4, 1);
  layout_p -> addWidget(enter_button_p, 4, 2);
  setLayout(layout_p);
  
  connect(n0_button_p, SIGNAL(clicked()), this, SLOT(n0_clicked()));
  connect(n1_button_p, SIGNAL(clicked()), this, SLOT(n1_clicked()));
  connect(n2_button_p, SIGNAL(clicked()), this, SLOT(n2_clicked()));
  connect(n3_button_p, SIGNAL(clicked()), this, SLOT(n3_clicked()));
  connect(n4_button_p, SIGNAL(clicked()), this, SLOT(n4_clicked()));
  connect(n5_button_p, SIGNAL(clicked()), this, SLOT(n5_clicked()));
  connect(n6_button_p, SIGNAL(clicked()), this, SLOT(n6_clicked()));
  connect(n7_button_p, SIGNAL(clicked()), this, SLOT(n7_clicked()));
  connect(n8_button_p, SIGNAL(clicked()), this, SLOT(n8_clicked()));
  connect(n9_button_p, SIGNAL(clicked()), this, SLOT(n9_clicked()));
  connect(enter_button_p, SIGNAL(clicked()), this, SLOT(enter_clicked()));
}
  
keypad::~keypad() 
{
  delete display_p;
  delete n0_button_p;
  delete n1_button_p;
  delete n2_button_p;
  delete n3_button_p;
  delete n4_button_p;
  delete n5_button_p;
  delete n6_button_p;
  delete n7_button_p;
  delete n8_button_p;
  delete n9_button_p;
  delete enter_button_p;
  delete layout_p;
}

void keypad::clear()
{
  entry_line_p->clear();
  display_p->setText(*entry_line_p);
}

void keypad::n0_clicked()
{
  entry_line_p->append("0");
  display_p->setText(*entry_line_p);
}

void keypad::n1_clicked()
{
  entry_line_p->append("1");
  display_p->setText(*entry_line_p);
}

void keypad::n2_clicked()
{
  entry_line_p->append("2");
  display_p->setText(*entry_line_p);
}

void keypad::n3_clicked()
{
  entry_line_p->append("3");
  display_p->setText(*entry_line_p);
}

void keypad::n4_clicked()
{
  entry_line_p->append("4");
  display_p->setText(*entry_line_p);
}

void keypad::n5_clicked()
{
  entry_line_p->append("5");
  display_p->setText(*entry_line_p);
}

void keypad::n6_clicked()
{
  entry_line_p->append("6");
  display_p->setText(*entry_line_p);
}

void keypad::n7_clicked()
{
  entry_line_p->append("7");
  display_p->setText(*entry_line_p);
}

void keypad::n8_clicked()
{
  entry_line_p->append("8");
  display_p->setText(*entry_line_p);
}

void keypad::n9_clicked()
{
  entry_line_p->append("9");
  display_p->setText(*entry_line_p);
}

void keypad::enter_clicked()
{
  int result = entry_line_p->toInt();
  emit number_entered(result);
  entry_line_p->clear();
  display_p->setText(*entry_line_p);
}
  

