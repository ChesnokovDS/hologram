#include "openglwindow.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>

#include <QtCore/qmath.h>

class HalogramWindow : public OpenGLWindow
{
public:
    HalogramWindow();

    void initialize() Q_DECL_OVERRIDE;
    void render() Q_DECL_OVERRIDE;

private:
    GLuint m_posAttr;
    GLuint m_colAttr;

    QOpenGLShaderProgram *m_program;
    int m_frame;
};

HalogramWindow::HalogramWindow()
    : m_program(0)
    , m_frame(0)
{}

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    format.setSamples(16);

    HalogramWindow window;
    window.setFormat(format);
    window.resize(640, 480);
    window.show();

    window.setAnimating(true);

    return app.exec();
}

static const char *vertexShaderSource =
    "\n attribute highp vec4 posAttr;"
    "\n varying lowp vec2 TexCoord;"
    "\n void main() {"
    "\n     gl_Position = posAttr;"
    "\n     TexCoord = posAttr.xy;"
    "\n }";

static const char *fragmentShaderSource =
    "\n varying lowp vec2 TexCoord;"
    "\n void main() {"
    "\n    gl_FragColor = vec4((vec2(1.0) + TexCoord) * 0.5, 0.0, 1.0);"
    "\n }";

void HalogramWindow::initialize()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex  , vertexShaderSource  );
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    m_colAttr = m_program->attributeLocation("colAttr");
}

void HalogramWindow::render()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT);

    m_program->bind();

    GLfloat vertices[] = {
        -1.0f,  1.0f,
         1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f, -1.0f,
        -1.0f,  1.0f
    };

    glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    m_program->release();

    ++m_frame;
}
