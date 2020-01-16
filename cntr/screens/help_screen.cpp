#include "help_screen.hpp"
#include "button.hpp"
#include <QTextEdit>
#include <QVBoxLayout>
#include <QString>

help_screen::help_screen()
  :screen(0)
{
//  setMaximumSize(800, 470);
//  setMinimumSize(800, 470);
  
  text_edit_p = new QTextEdit;
  text_edit_p -> setReadOnly(true);
  
  exit_button_p = new button("Exit");
  connect(exit_button_p, SIGNAL(clicked()), this, SLOT(deleteLater()));
  
  layout_p = new QVBoxLayout;
  layout_p->addWidget(text_edit_p);
  layout_p->addWidget(exit_button_p);
  setLayout(layout_p);
}

void help_screen::set_text(QString text)
{
  text_edit_p->setPlainText(text);
}

