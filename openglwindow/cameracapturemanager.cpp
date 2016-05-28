#include "cameracapturemanager.h"

#include <QCameraInfo>

#include "cameravideosurface.h"

CameraCaptureManager::CameraCaptureManager()
{
    m_camera = new QCamera(QCameraInfo::defaultCamera());
    m_surface = new CameraVideoSurface();

    m_camera->setViewfinder(m_surface);

    m_camera->start();
}

CameraVideoSurface* CameraCaptureManager::getSurface() const
{
    return m_surface;
}
