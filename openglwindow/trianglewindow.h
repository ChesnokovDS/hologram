#pragma once

#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>

#include <OpenGLWindow.h>

#include <QtCore/qmath.h>

#include <QMediaPlayer>

class TriangleWindow : public OpenGLWindow
{
public:
    TriangleWindow();

    void initialize() Q_DECL_OVERRIDE;
    void render() Q_DECL_OVERRIDE;

private:
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;

    QOpenGLShaderProgram *m_program;
    int m_frame;
};
