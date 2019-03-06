#ifndef envelope_layout_choice_hpp
#define envelope_layout_choice_hpp

#include <QString>
#include "centre.hpp"

class button;
class QGridLayout;
class QGroupBox;
class QVBoxLayout;
class QLabel;
class QDir;
class QStringList;

class envelope_layout_choice : public screen
{
  Q_OBJECT
  
  public:
  envelope_layout_choice(centre*set_centre_p, batch_mode_driver* batch_mode_driver_p_s);
  ~envelope_layout_choice();
  
  private slots:
  void choice1_clicked();
  void choice2_clicked();
  void choice3_clicked();
  void choice4_clicked();
  void choice5_clicked();
  void choice6_clicked();
  
  void more_envelope_layouts_clicked();
  void add_envelope_layout_clicked();
  void delete_envelope_layout_clicked();
  void no_envelope_layout_clicked();
  void back_clicked();
  
  private:
  QLabel* message_p;
  button* choice1_p;
  button* choice2_p;
  button* choice3_p;
  button* choice4_p;
  button* choice5_p;
  button* choice6_p;
  
  button* more_envelope_layouts_p;
  button* add_envelope_layout_p;
  button* delete_envelope_layout_p;
  button* no_envelope_layout_p;
  button* back_p;
  
  void display_envelope_layouts();
  
  QGroupBox* top_box_p;
  QGroupBox* middle_box_p;
  QGroupBox* bottom_box_p;
  
  QGridLayout* top_layout_p;
  QGridLayout* middle_layout_p;
  QGridLayout* bottom_layout_p;
  
  QVBoxLayout* main_layout_p;
  
  int first_envelope_layout_displayed;
  
//  centre* centre_p;
  batch_mode_driver* batch_mode_driver_p;
  
  QDir* dir_p;
  QStringList* envelope_layout_list_p;
  QString envelope_layout_path;
};
#endif
