#pragma once

#include <QAbstractVideoSurface>


class CameraVideoSurface
    : public QAbstractVideoSurface
{
    Q_OBJECT

public:
    CameraVideoSurface(QObject *parent = 0);

    virtual QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType type/* = QAbstractVideoBuffer::NoHandle*/) const override;
    virtual bool present(const QVideoFrame &frame) override;

signals:
    void frameAvailable(QImage frame);
};
