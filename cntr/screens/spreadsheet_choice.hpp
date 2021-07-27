#ifndef spreadsheet_choice_hpp
#define spreadsheet_choice_hpp

#include <QString>
#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QVBoxLayout;
class QLabel;
class QDir;
class QStringList;
class help_screen;

class spreadsheet_choice : public screen
{
  Q_OBJECT
  
  public:
  spreadsheet_choice(centre*set_centre_p, batch_mode_driver* batch_mode_driver_p_s);
  ~spreadsheet_choice();
  
  private slots:
  void choice1_clicked();
  void choice2_clicked();
  void choice3_clicked();
  void choice4_clicked();
  void choice5_clicked();
  void choice6_clicked();
  
  void more_spreadsheets_clicked();
  void back_clicked();
  void help_clicked();
  
  private:
  QLabel* message_p;
  button* choice1_p;
  button* choice2_p;
  button* choice3_p;
  button* choice4_p;
  button* choice5_p;
  button* choice6_p;
  
  button* more_spreadsheets_p;
  button* add_spreadsheet_p;
  button* delete_spreadsheet_p;
  button* back_p;
  button* help_p;
  
  void display_spreadsheets();
  
  QGroupBox* top_box_p;
  QGroupBox* middle_box_p;
  QGroupBox* bottom_box_p;
  
  QGridLayout* top_layout_p;
  QGridLayout* middle_layout_p;
  QGridLayout* bottom_layout_p;
  
  QVBoxLayout* main_layout_p;
  
  int first_spreadsheet_displayed;
  
//  centre* centre_p;
  batch_mode_driver* batch_mode_driver_p;
  
  QDir* dir_p;
  QStringList* spreadsheet_list_p;
  QString spreadsheet_path;
  
  help_screen* help_screen_p;
};
#endif
