#ifndef FEEDER_HPP
#define FEEDER_HPP

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QTime>

class loop_timer:public QThread
{
  Q_OBJECT
  public:
  loop_timer(int* period_p_s, bool* running_p_s);
  void run();
  
  private:
  bool pulse_state;
  int* period_p;
  bool* running_p;
};


class feeder:public QObject
{
  Q_OBJECT
  
  public:
  feeder();
  ~feeder();
 
  public slots:
  void set_speed(int speed); //speed is pulses per sec.
  int get_speed() {return speed;};
  void stop() {set_speed(0);};
  
  private slots:
  void loop_timer_finished_slot();
  
  private:
  int speed;
  int period;//in microseconds
  bool running; //true -> pulse is turned on
  bool loop_timer_finished;//used to signal that the loop timer has finished in destructor
  loop_timer* loop_timer_p;
};


#endif

/*
For this code to work, some setup is needed:

Rootless GPIO for Odroid, Lubuntu 15.10

append to  /etc/udev/rules.d/10-odroid.rules


SUBSYSTEM=="gpio", KERNEL=="gpiochip*", ACTION=="add", PROGRAM="/bin/sh -c 'chown root:odroid /sys/class/gpio/export /sys/class/gpio/unexport ; chmod 220 /sys/class/gpio/export /sys/class/gpio/unexport'"
SUBSYSTEM=="gpio", KERNEL=="gpio*", ACTION=="add", PROGRAM="/bin/sh -c 'chown root:odroid /sys%p/active_low /sys%p/direction /sys%p/edge /sys%p/value ; chmod 660 /sys%p/active_low /sys%p/direction /sys%p/edge /sys%p/value'"


insert to /etc/rc.local before "exit 0"  note: modified 2017-06-26 after prototype assembly

echo 173 >> /sys/class/gpio/export
echo out >> /sys/class/gpio/gpio173/direction

echo 171 >> /sys/class/gpio/export
echo out >> /sys/class/gpio/gpio171/direction

echo 172 >> /sys/class/gpio/export
echo out >> /sys/class/gpio/gpio172/direction

echo 189 >> /sys/class/gpio/export
echo out >> /sys/class/gpio/gpio189/direction

echo 210 >> /sys/class/gpio/export
echo out >> /sys/class/gpio/gpio210/direction

echo 209 >> /sys/class/gpio/export
echo out >> /sys/class/gpio/gpio209/direction

echo 19  >> /sys/class/gpio/export
echo out  >> /sys/class/gpio/gpio19/direction

echo 28  >> /sys/class/gpio/export
echo out  >> /sys/class/gpio/gpio28/direction

echo 174  >> /sys/class/gpio/export
echo in  >> /sys/class/gpio/gpio174/direction

echo 192  >> /sys/class/gpio/export
echo in  >> /sys/class/gpio/gpio192/direction

echo 191  >> /sys/class/gpio/export
echo in  >> /sys/class/gpio/gpio191/direction

echo 190  >> /sys/class/gpio/export
echo in  >> /sys/class/gpio/gpio190/direction

*/

