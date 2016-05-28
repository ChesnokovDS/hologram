#include "openglwindow.h"

#include <QtGui/QGuiApplication>

#include "cameracapturemanager.h"
#include "cameravideosurface.h"
#include "hologramwindow.h"


int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    CameraCaptureManager manager;

    QSurfaceFormat format;
    format.setSamples(16);

    HologramWindow window;
    window.setFormat(format);
    window.resize(640, 480);
    window.show();

    window.setAnimating(true);

    QObject::connect(manager.getSurface(), &CameraVideoSurface::frameAvailable,
                     &window, &HologramWindow::onFrameAvailable);

    return app.exec();
}
