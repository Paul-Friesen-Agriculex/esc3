
#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP

#include <QWidget>
#include <QString>

class QImage;

enum envelope_field_type{Ubuntu_mono, code_39};

struct envelope_field
{
  int column;
  envelope_field_type type;
  QString text;
  int x;
  int y;
  int height;
  envelope_field* next;
};

class envelope : public QWidget
{
  Q_OBJECT

  private:
  QImage* image_p;
  envelope_field* first_field;
  static const float pixels_per_mm = 25;
  
  
  protected:
  void paintEvent(QPaintEvent* event);

  public:
  envelope(QWidget *parent = 0);
  ~envelope();
  void set_size(int width, int height);//dimensions mm
  //values to be entered in next field. measurements mm
  int column;
  envelope_field_type type;
  QString text;
  int x;
  int y;
  int height;

  public slots:
  void enter_field();
  void print();
};

#endif

