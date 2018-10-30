#ifndef keypad_hpp
#define keypad_hpp

#include <QWidget>

class QPushButton;
class QString;
class QLabel;
class QGridLayout;

class keypad:public QWidget
{
  Q_OBJECT
  
  public:
  keypad();
  ~keypad();
  void clear();
  
  private slots:
  void n0_clicked();
  void n1_clicked();
  void n2_clicked();
  void n3_clicked();
  void n4_clicked();
  void n5_clicked();
  void n6_clicked();
  void n7_clicked();
  void n8_clicked();
  void n9_clicked();
  void enter_clicked();
  
  signals:
  void number_entered(int value);
  
  private:
  QLabel* display_p;
  QPushButton* n0_button_p;
  QPushButton* n1_button_p;
  QPushButton* n2_button_p;
  QPushButton* n3_button_p;
  QPushButton* n4_button_p;
  QPushButton* n5_button_p;
  QPushButton* n6_button_p;
  QPushButton* n7_button_p;
  QPushButton* n8_button_p;
  QPushButton* n9_button_p;
  QPushButton* enter_button_p;
  
  QGridLayout* layout_p;
  
  QString* entry_line_p;
};


#endif
