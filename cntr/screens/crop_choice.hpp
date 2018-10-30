#ifndef crop_choice_hpp
#define crop_choice_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QVBoxLayout;
class QLabel;

class crop_choice : public screen
{
  Q_OBJECT
  
  public:
  crop_choice(centre*set_centre_p);
  
  private slots:
  void choice1_clicked();
  void choice2_clicked();
  void choice3_clicked();
  void choice4_clicked();
  void choice5_clicked();
  void choice6_clicked();
  
  void more_crops_clicked();
  void edit_crops_clicked();
  void back_clicked();
  
  private:
  QLabel* message_p;
  button* choice1_p;
  button* choice2_p;
  button* choice3_p;
  button* choice4_p;
  button* choice5_p;
  button* choice6_p;
  
  button* more_crops_p;
  button* edit_crops_p;
  button* back_p;
  
  void display_crops();
  
  QGroupBox* top_box_p;
  QGroupBox* middle_box_p;
  QGroupBox* bottom_box_p;
  
  QGridLayout* top_layout_p;
  QGridLayout* middle_layout_p;
  QGridLayout* bottom_layout_p;
  
  QVBoxLayout* main_layout_p;
  
  int first_crop_displayed;
  
//  centre* centre_p;
};
#endif
