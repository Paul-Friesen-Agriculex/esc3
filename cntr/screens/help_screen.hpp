#ifndef help_screen_hpp
#define help_screen_hpp

#include "centre.hpp"

class button;
class QTextEdit;
class QVBoxLayout;
class QString;

class help_screen : public screen
{
  Q_OBJECT
  
  public:
  help_screen();
  void set_text(QString text);
  
  private:
  QTextEdit* text_edit_p;
  button* exit_button_p;
  QVBoxLayout* layout_p;
};

#endif
  
