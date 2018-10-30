#ifndef button_hpp
#define button_hpp

#include <QObject>
#include <QPushButton>


class button : public QPushButton
{
  Q_OBJECT
  
  public:
  button(QWidget* parent);
  button(const QString & text, QWidget * parent = 0);
};
#endif

