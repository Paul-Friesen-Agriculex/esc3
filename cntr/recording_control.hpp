#ifndef recording_control_hpp
#define recording_control_hpp

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

class centre;

class recording_control : public QWidget
{
  Q_OBJECT
  
  public:  
  recording_control(centre* centre_p_s, QWidget* parent = 0);
  
  private slots:
  void start_recording();
  void stop_recording();
  
  private:
  centre* centre_p;
  QPushButton* start_button_p;
  QPushButton* stop_button_p;
  QVBoxLayout* layout_p;
};

#endif
