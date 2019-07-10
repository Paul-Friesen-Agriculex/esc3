######################################################################
# Automatically generated by qmake (3.0) Tue Jul 9 19:40:42 2019
######################################################################

TEMPLATE = app
TARGET = cntr
INCLUDEPATH += . ./screens

# Input
HEADERS += batch_mode_driver.hpp \
           brother_envelope_feeder.hpp \
           button.hpp \
           centre.hpp \
           code_39_writer.hpp \
           crop.hpp \
           cutgate.hpp \
           diagnostics_console.hpp \
           display.hpp \
           dust_streak.hpp \
           endgate.hpp \
           envelope.hpp \
           envelope_feeder_brother.hpp \
           envelope_sensor.hpp \
           feeder.hpp \
           processor.hpp \
           processor_calcs.hpp \
           screens/batch.hpp \
           screens/batch_options.hpp \
           screens/batch_table.hpp \
           screens/calibrate.hpp \
           screens/crop_choice.hpp \
           screens/crop_delete.hpp \
           screens/crop_edit.hpp \
           screens/crop_name_exists.hpp \
           screens/enter_program.hpp \
           screens/keyboard.hpp \
           screens/keypad.hpp \
           screens/macro_screen.hpp \
           screens/message_box.hpp \
           screens/playback.hpp \
           screens/position_envelope_field.hpp \
           screens/program_choice.hpp \
           screens/program_delete.hpp \
           screens/select_envelope_field.hpp \
           screens/select_field_data_source.hpp \
           screens/set_envelope_size.hpp \
           screens/spreadsheet_choice.hpp \
           screens/ss_batch.hpp \
           screens/ss_batch_table.hpp \
           screens/ss_column_display_options.hpp \
           screens/ss_options.hpp \
           screens/ss_setup_choice.hpp \
           screens/ss_setup_entry.hpp \
           screens/start_screen.hpp \
           screens/table.hpp \
           screens/tool_choice.hpp \
           screens/totalize.hpp \
           screens/totalize_options.hpp
SOURCES += batch_mode_driver.cpp \
           brother_envelope_feeder.cpp \
           button.cpp \
           centre.cpp \
           code_39_writer.cpp \
           cutgate.cpp \
           diagnostics_console.cpp \
           display.cpp \
           dust_streak.cpp \
           endgate.cpp \
           envelope.cpp \
           envelope_feeder_brother.cpp \
           envelope_sensor.cpp \
           feeder.cpp \
           main.cpp \
           processor.cpp \
           screens/batch.cpp \
           screens/batch_options.cpp \
           screens/batch_table.cpp \
           screens/calibrate.cpp \
           screens/crop_choice.cpp \
           screens/crop_delete.cpp \
           screens/crop_edit.cpp \
           screens/crop_name_exists.cpp \
           screens/enter_program.cpp \
           screens/keyboard.cpp \
           screens/keypad.cpp \
           screens/macro_screen.cpp \
           screens/message_box.cpp \
           screens/playback.cpp \
           screens/position_envelope_field.cpp \
           screens/program_choice.cpp \
           screens/program_delete.cpp \
           screens/select_envelope_field.cpp \
           screens/select_field_data_source.cpp \
           screens/set_envelope_size.cpp \
           screens/spreadsheet_choice.cpp \
           screens/ss_batch.cpp \
           screens/ss_batch_table.cpp \
           screens/ss_column_display_options.cpp \
           screens/ss_options.cpp \
           screens/ss_setup_choice.cpp \
           screens/ss_setup_entry.cpp \
           screens/start_screen.cpp \
           screens/table.cpp \
           screens/tool_choice.cpp \
           screens/totalize.cpp \
           screens/totalize_options.cpp

QT += widgets
QT += printsupport
LIBS += -L"/usr/lib" -lSK91USB3-LX

