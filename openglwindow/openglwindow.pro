include(openglwindow.pri)

SOURCES += \
    main.cpp openglwindow.cpp

HEADERS += \
    openglwindow.h

target.path = $$[QT_INSTALL_EXAMPLES]/gui/openglwindow
INSTALLS += target
