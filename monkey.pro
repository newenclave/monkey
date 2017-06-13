TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    check_lexer.cpp

INCLUDEPATH += etool/include/ \
               catch

HEADERS += \
    lexer.h \
    parser.h \
    ast.h

