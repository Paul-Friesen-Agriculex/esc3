#ifndef keyboard_hpp
#define keyboard_hpp

#include "centre.hpp"
#include "batch_mode_driver.hpp"

#include <QPushButton>

class QGridLayout;
class QGroupBox;
class QVBoxLayout;
//class QLabel;
class QTextEdit;
class QLineEdit;
class QString;
class button;

class kb_key : public QPushButton
{
  Q_OBJECT
  
  public:
  kb_key(const QString & text, QWidget* parent = 0);
  
};

class keyboard : public screen
{
  Q_OBJECT
  
  public:
  keyboard(centre*set_centre_p, QString* out_string_s = 0);
  
  private slots:
  void back_clicked();

  void n1_clicked();
  void n2_clicked();
  void n3_clicked();
  void n4_clicked();
  void n5_clicked();
  void n6_clicked();
  void n7_clicked();
  void n8_clicked();
  void n9_clicked();
  void n0_clicked();
  void Q_clicked();
  void W_clicked();
  void E_clicked();
  void R_clicked();
  void T_clicked();
  void Y_clicked();
  void U_clicked();
  void I_clicked();
  void O_clicked();
  void P_clicked();
  void A_clicked();
  void S_clicked();
  void D_clicked();
  void F_clicked();
  void G_clicked();
  void H_clicked();
  void J_clicked();
  void K_clicked();
  void L_clicked();
  void Z_clicked();
  void X_clicked();
  void C_clicked();
  void V_clicked();
  void B_clicked();
  void N_clicked();
  void M_clicked();
  void space_clicked();
  
  void enter_clicked();
  void backspace_clicked();
  
  signals:
  void external_keyboard_enter();
  void finished(int r);
  
  protected:
//  QLabel* message_p;
  QTextEdit* message_p;
  QString* entry_line_p;
  QLineEdit* entry_line_display_p;
  button* back_button_p;
  QPushButton* enter_key_p;
  QString* out_string_p;
  
  
  
  virtual void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
  
  private:
  QGroupBox* top_box_p;
  QGridLayout* top_box_layout_p;
  
  kb_key* n1_key_p;
  kb_key* n2_key_p;
  kb_key* n3_key_p;
  kb_key* n4_key_p;
  kb_key* n5_key_p;
  kb_key* n6_key_p;
  kb_key* n7_key_p;
  kb_key* n8_key_p;
  kb_key* n9_key_p;
  kb_key* n0_key_p;
  kb_key* Q_key_p;
  kb_key* W_key_p;
  kb_key* E_key_p;
  kb_key* R_key_p;
  kb_key* T_key_p;
  kb_key* Y_key_p;
  kb_key* U_key_p;
  kb_key* I_key_p;
  kb_key* O_key_p;
  kb_key* P_key_p;
  kb_key* A_key_p;
  kb_key* S_key_p;
  kb_key* D_key_p;
  kb_key* F_key_p;
  kb_key* G_key_p;
  kb_key* H_key_p;
  kb_key* J_key_p;
  kb_key* K_key_p;
  kb_key* L_key_p;
  kb_key* Z_key_p;
  kb_key* X_key_p;
  kb_key* C_key_p;
  kb_key* V_key_p;
  kb_key* B_key_p;
  kb_key* N_key_p;
  kb_key* M_key_p;
  kb_key* backspace_key_p;
  QPushButton* space_key_p;
  
  
  
  QGroupBox* key_box_p;
  QGridLayout* key_box_layout_p;
  
  QVBoxLayout* main_layout_p;
  
};

//=======================================================================================// TEST~~~ 11_13_2018//
class macro_name_entry : public keyboard
{
  Q_OBJECT
  
  public:
  macro_name_entry(centre* set_centre_p);
  
  public slots:
  void macro_name_entered();
};
//=======================================================================================//

class crop_name_entry : public keyboard
{
  Q_OBJECT
  
  public:
  crop_name_entry(centre* set_centre_p);
  
  public slots:
  void enter_clicked();
};

class totalize_save_file : public keyboard
{
  Q_OBJECT
  
  public:
  totalize_save_file(centre* set_centre_p);
  
  public slots:
  void enter_clicked();
};

class batch_save_program : public keyboard
{
  Q_OBJECT
  
  batch_mode_driver* batch_mode_driver_p;
  
  public:
  batch_save_program(centre* set_centre_p, batch_mode_driver* batch_mode_driver_p_s);
  
  public slots:
  void enter_clicked();
};

class batch_save_table : public keyboard
{
  Q_OBJECT
  
  batch_mode_driver* batch_mode_driver_p;
  
  public:
  batch_save_table(centre* set_centre_p, batch_mode_driver* batch_mode_driver_p_s);
  
  public slots:
  void enter_clicked();
};

class batch_save_ss_setup : public keyboard
{
  Q_OBJECT
  
  batch_mode_driver* batch_mode_driver_p;
  
  public:
  batch_save_ss_setup(centre* set_centre_p, batch_mode_driver* batch_mode_driver_p_s);
  
  public slots:
  void enter_clicked();
};

class enter_field_text : public keyboard
{
  Q_OBJECT
  
  batch_mode_driver* batch_mode_driver_p;
  
  public:
  enter_field_text(centre* set_centre_p, batch_mode_driver* batch_mode_driver_p_s);
  
  public slots:
  void enter_clicked();
};


#endif

