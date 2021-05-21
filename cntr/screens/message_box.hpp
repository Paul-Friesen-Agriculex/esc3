#include <QWidget>
#include <QString>
#include <QColor>

class message_box:public QWidget
{
  Q_OBJECT
  
  public:
  message_box(QWidget* parent = 0);
  void set_text(QString text_s);
  void set_background(int r, int g, int b);
  void set_background(QColor background_s);
  void set_foreground(int r, int g, int b);
  void set_foreground(QColor foreground_s);
  void set_text_size(int size_s);
  
  protected:
  void paintEvent(QPaintEvent* event);
  
  private:
  QColor foreground;
  QColor background;
  QString text;
  int text_size;
};
