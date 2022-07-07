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

#include "help_screen.hpp"
#include "button.hpp"
#include <QTextEdit>
#include <QVBoxLayout>
#include <QString>

help_screen::help_screen()
  :screen(0)
{
//  setMaximumSize(800, 470);
//  setMinimumSize(800, 470);
  
//  setWindowState(Qt::WindowFullScreen);



  text_edit_p = new QTextEdit;
  text_edit_p -> setReadOnly(true);
  
  exit_button_p = new button("Exit");
  connect(exit_button_p, SIGNAL(clicked()), this, SLOT(deleteLater()));
  
  layout_p = new QVBoxLayout;
  layout_p->addWidget(text_edit_p);
  layout_p->addWidget(exit_button_p);
  setLayout(layout_p);
}

void help_screen::set_text(QString text)
{
  text_edit_p->setPlainText(text);
}

