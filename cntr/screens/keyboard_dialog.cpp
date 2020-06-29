#include "keyboard_dialog.hpp"
#include "keyboard.hpp"
#include <QString>
#include <iostream>

using namespace std;

keyboard_dialog::keyboard_dialog(QWidget* parent_p, centre* centre_p_s, QString* ret_string_p)
{
  setParent(parent_p);
  centre_p = centre_p_s;
  keyboard_p = new keyboard(centre_p, ret_string_p);
  keyboard_p -> setParent(this);
  keyboard_p -> show();
  keyboard_p -> raise();
  
  connect(keyboard_p, SIGNAL(finished(int)), this, SLOT(done(int)));
}
