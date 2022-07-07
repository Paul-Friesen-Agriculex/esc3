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

//-------------------------------------------------------------------------------------------------------
//Class name: crop_edit
//Purpose: providing an ability to add new crop, delete exist crop or revise parameters of existing crop
//Created: August 8, 2015
//version 1.0
//--------------------------------------------------------------------------------------------------------
#ifndef crop_edit_hpp
#define crop_edit_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QVBoxLayout;
class QLabel;

class crop_edit : public screen
{
  Q_OBJECT
  
  public:
  crop_edit(centre*set_centre_p);
  
  private slots:
	  void cropAddButn_clicked();
	  void cropDelButn_clicked();
	  void cropEditButn_clicked();
	  void cropRecalibrate_clicked();
	  void backButn_clicked();
    void done_button_clicked();
  
  private:
	  QLabel* message_p;
	  button* cropAddButn_p;
	  button* cropDelButn_p;
	  button* cropEditButn_p;
	  button* cropRecalibrateButn_p;
	  button* backButn_p;
    button* done_button_p;
  
  
	  QGroupBox* top_box_p;
	  QGroupBox* buttonlist_box_p;
	  
	  QGridLayout* top_layout_p;
	  QGridLayout* buttonlist_layout_p;
	  
	  QVBoxLayout* main_layout_p;
  
	  int first_crop_displayed;
  
//  centre* centre_p;
};
#endif
