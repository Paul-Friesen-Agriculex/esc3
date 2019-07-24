#ifndef BROTHER_ENVELOPE_FEEDER_HPP
#define BROTHER_ENVELOPE_FEEDER_HPP

#include <QObject>

class QTimer;

class brother_envelope_feeder:public QObject
{
  Q_OBJECT
  
  public:
  brother_envelope_feeder();
  ~brother_envelope_feeder();
  
  private:
  bool envelope_feeder_motor_running;
  bool envelope_feeder_motor_triggered;
  bool envelope_feeder_sensor_triggered;
  QTimer* delay_envelope_taken;
  QTimer* delay_sensor_triggered;
  
  public slots:
  void run();

  private slots:  
  void envelope_feeder_sensor_state();
  void envelope_feeder_motor_state();
  void motor_on();
  void motor_off();
};

#endif
