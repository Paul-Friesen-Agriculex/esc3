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
  
  private:
	  QLabel* message_p;
	  button* cropAddButn_p;
	  button* cropDelButn_p;
	  button* cropEditButn_p;
	  button* cropRecalibrateButn_p;
	  button* backButn_p;
  
  
	  QGroupBox* top_box_p;
	  QGroupBox* buttonlist_box_p;
	  
	  QGridLayout* top_layout_p;
	  QGridLayout* buttonlist_layout_p;
	  
	  QVBoxLayout* main_layout_p;
  
	  int first_crop_displayed;
  
//  centre* centre_p;
};
#endif
