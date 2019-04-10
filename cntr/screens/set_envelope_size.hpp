#ifndef set_envelope_size_hpp
#define set_envelope_size_hpp

#include "centre.hpp"

class button;
class QGridLayout;
class QLabel;
class QSlider;
class QTimer;

class set_envelope_size : public screen
{
  Q_OBJECT
  
  public:
  set_envelope_size(centre*set_centre_p, batch_mode_driver* batch_mode_driver_p_s);
  ~set_envelope_size();
  
  private slots:
	  void back_button_clicked();
    void ok_button_clicked();
    void run();
  
  private:
    batch_mode_driver* batch_mode_driver_p;
	  QLabel* message_p;
    button* back_button_p;
    QLabel* width_label_p;
    QSlider* width_slider_p;
    QLabel* height_label_p;
    QSlider* height_slider_p;
	  button* ok_button_p;
	  QGridLayout* layout_p;
    QTimer* timer_p;
};
#endif
