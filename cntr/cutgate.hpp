#ifndef CUTGATE_HPP
#define CUTGATE_HPP

#include <QObject>

class QTimer;

enum CUTGATE_STATE{CUTGATE_OPEN, CUTGATE_CLOSED};

class cutgate:public QObject
{
  Q_OBJECT
  
  private:
  QTimer* open_pulse_timer_p;
  QTimer* close_pulse_timer_p;
  CUTGATE_STATE state;
  
  public:
  cutgate();
  ~cutgate();
  
  public slots:
  void open(void);
  void close(void);
  CUTGATE_STATE get_state(void) {return state;};
  
  private slots:
  void end_open(void);
  void end_close(void);
  
  signals:
  void closed_while_opening();
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
