#ifndef keyboard_dialog_hpp
#define keyboard_dialog_hpp

#include <QDialog>

class QString;
class centre;
class keyboard;

class keyboard_dialog:public QDialog
{
  Q_OBJECT
  
  QString* ret_string_p;
  centre* centre_p;
  keyboard* keyboard_p;
  
  public:
  keyboard_dialog(QWidget* parent_p, centre* centre_p_s, QString* ret_string_p_s);
};
  

#endif
