#include "cameravideosurface.h"

#include "QDebug"

CameraVideoSurface::CameraVideoSurface(QObject *parent/* = 0*/)
    : QAbstractVideoSurface(parent)
{

}

QList<QVideoFrame::PixelFormat> CameraVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType type/* = QAbstractVideoBuffer::NoHandle*/) const
{
    Q_UNUSED(type);
        return QList<QVideoFrame::PixelFormat>()
            << QVideoFrame::Format_RGB32;
}

bool CameraVideoSurface::present(const QVideoFrame &frame)
{
    if ( !frame.isValid())
        return false;

    qDebug() << frame.handleType();

    QVideoFrame cloneFrame(frame);
    cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
    const QImage image(cloneFrame.bits(),
                       cloneFrame.width(),
                       cloneFrame.height(),
                       QVideoFrame::imageFormatFromPixelFormat(cloneFrame .pixelFormat()));

    emit frameAvailable(image);

    cloneFrame.unmap();
}
