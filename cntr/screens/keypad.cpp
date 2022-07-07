/***********

Copyright 2021 Agriculex Inc.

This file is part of the Agriculex ESC-3 program.

The Agriculex ESC-3 program is free software: you can redistribute it 
and/or modify it under the terms of the GNU General Public License as 
published by the Free Software Foundation, GPL-3.0-or-later.

The Agriculex ESC-3 program is distributed in the hope that it will be 
useful, but, except as noted below, WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with The Agriculex ESC-3 program.  If not, see 
<https://www.gnu.org/licenses/>.

Agriculex Inc. warrants that executable files of the ESC-3 program 
compiled and installed by Agriculex Inc. will function as advertized in 
ESC-3 machines produced by Agriculex Inc.

**********/

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
  //n0_button_p -> setMaximumWidth(50);
  n0_button_p -> setMinimumSize(50,50);
  n1_button_p = new QPushButton("1");
  //n1_button_p -> setMaximumWidth(50);
  n1_button_p -> setMinimumSize(50,50);
  n2_button_p = new QPushButton("2");
  //n2_button_p -> setMaximumWidth(50);
  n2_button_p -> setMinimumSize(50,50);
  n3_button_p = new QPushButton("3");
  //n3_button_p -> setMaximumWidth(50);
  n3_button_p -> setMinimumSize(50,50);
  n4_button_p = new QPushButton("4");
  //n4_button_p -> setMaximumWidth(50);
  n4_button_p -> setMinimumSize(50,50);
  n5_button_p = new QPushButton("5");
  //n5_button_p -> setMaximumWidth(50);
  n5_button_p -> setMinimumSize(50,50);
  n6_button_p = new QPushButton("6");
  //n6_button_p -> setMaximumWidth(50);
  n6_button_p -> setMinimumSize(50,50);
  n7_button_p = new QPushButton("7");
  //n7_button_p -> setMaximumWidth(50);
  n7_button_p -> setMinimumSize(50,50);
  n8_button_p = new QPushButton("8");
  //n8_button_p -> setMaximumWidth(50);
  n8_button_p -> setMinimumSize(50,50);
  n9_button_p = new QPushButton("9");
  //n9_button_p -> setMaximumWidth(50);
  n9_button_p -> setMinimumSize(50,50);
  enter_button_p = new QPushButton("Enter");
  //enter_button_p -> setMaximumWidth(50);
  enter_button_p -> setMinimumSize(50,50);
  backspace_button_p = new QPushButton("<-");
  backspace_button_p -> setMinimumSize(50,50);
  
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
  layout_p -> addWidget(backspace_button_p, 4, 0);
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
  connect(backspace_button_p, SIGNAL(clicked()), this, SLOT(backspace_clicked()));
  
  display_p -> setStyleSheet( "font-size: 15pt;");
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
  delete backspace_button_p;
  delete layout_p;
}

void keypad::set_number(int num)
{
  entry_line_p->setNum(num);
  display_p->setText(*entry_line_p);
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
  
void keypad::backspace_clicked()
{
  if(!entry_line_p->isEmpty()) entry_line_p->chop(1);
  display_p->setText(*entry_line_p);
}
