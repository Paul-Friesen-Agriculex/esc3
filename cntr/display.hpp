#ifndef DISPLAY_H
#define DISPLAY_H

#include <QLabel>
#include <QBitmap>

class display:public QLabel
{
  Q_OBJECT
  
  public:
  display(int lines_to_display_set, QWidget* parent = 0);
  QImage image;
  
  protected:
  void paintEvent(QPaintEvent* event);
  
  private:
  int lines_to_display;
  
};

#endif
