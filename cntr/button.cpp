
#include "button.hpp"

button::button(QWidget* parent)
   :QPushButton(parent)
{
  setMinimumHeight(50);
}

button::button(const QString & text, QWidget * parent)
 :QPushButton(text, parent)
{
  setMinimumHeight(50);
}
