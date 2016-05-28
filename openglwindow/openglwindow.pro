include(openglwindow.pri)

CONFIG   += gui

QT += multimedia
QT += multimediawidgets

INCLUDEPATH += C:\Qt\vlc-qt\include\

LIBS += -lVLCQtCored -lVLCQtWidgetsd
LIBS += -LC:/Qt/vlc-qt/bin

SOURCES += \
    main.cpp \
    openglwindow.cpp \
    trianglewindow.cpp \
    videorenderer.cpp \
    cameravideosurface.cpp \
    cameracapturemanager.cpp \
    cameracapturerenderer.cpp \
    hologramwindow.cpp

HEADERS += \
    openglwindow.h \
    trianglewindow.h \
    videorenderer.h \
    cameravideosurface.h \
    cameracapturemanager.h \
    cameracapturerenderer.h \
    hologramwindow.h

target.path = $$[QT_INSTALL_EXAMPLES]/gui/openglwindow
INSTALLS += target

FORMS += \
    form.ui
