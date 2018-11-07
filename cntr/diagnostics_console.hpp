#ifndef diagnostics_console_hpp
#define diagnostics_console_hpp

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QString>
#include <QLabel>

class centre;

class diagnostics_console : public QWidget
{
  Q_OBJECT
  
  public:  
  diagnostics_console(centre* centre_p_s, QWidget* parent = 0);
  
  public slots:
  void receive_message(QString message);
  
  private slots:
  void start_recording();
  void stop_recording();
//  void reset_time_tests_button_clicked();
  
  signals:
  void reset_time_tests_signal();
  
  private:
  centre* centre_p;
  QPushButton* start_button_p;
  QPushButton* stop_button_p;
  QPushButton* reset_time_tests_button_p;
  QLabel* message_p;
  QVBoxLayout* layout_p;
};

#endif
