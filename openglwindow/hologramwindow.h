#pragma once

#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>
#include <QOpenGLTexture>

#include "openglwindow.h"

#include <QVideoFrame>

#include <QtCore/qmath.h>

class HologramWindow : public OpenGLWindow
{
    Q_OBJECT

public:
    HologramWindow();

    void initialize() Q_DECL_OVERRIDE;
    void render() Q_DECL_OVERRIDE;
    void updateTexture();

public slots:
    void onFrameAvailable(const QVideoFrame &frame);

private:
    GLuint m_posAttr;

    QOpenGLShaderProgram *m_program;
    QOpenGLTexture       *m_texture;
    GLuint m_texture_from;
    GLfloat m_aspect;
    int m_frame;
    bool m_initializedTexture;
};
