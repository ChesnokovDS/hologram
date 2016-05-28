#pragma once

#include <QCamera>

#include "cameravideosurface.h"


class CameraCaptureManager
{
public:
    CameraCaptureManager();
    CameraVideoSurface* getSurface() const;

private:
    QCamera *m_camera;

    CameraVideoSurface *m_surface;
};
