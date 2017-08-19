TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    format.cpp \
    ui.cpp \
    tokenization.cpp \
    notes.cpp \
    table_navigation.cpp \
    table_metadata.cpp \
    data.cpp \
    apply.cpp \
    injection_protection.cpp \
    sql_query_generation.cpp \
    sql_create_generation.cpp \
    sql_command_generation.cpp

HEADERS += \
    format.h \
    shared.h

QMAKE_CXXFLAGS_WARN_ON += -Wno-sign-compare

