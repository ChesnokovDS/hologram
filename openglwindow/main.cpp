#include "openglwindow.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>
#include <QOpenGLTexture>

#include <QtCore/qmath.h>

class HalogramWindow : public OpenGLWindow
{
public:
    HalogramWindow();

    void initialize() Q_DECL_OVERRIDE;
    void render() Q_DECL_OVERRIDE;
    void updateTexture();

private:
    GLuint m_posAttr;

    QOpenGLShaderProgram *m_program;
    QOpenGLTexture       *m_texture;
    GLuint m_texture_from;
    GLfloat m_aspect;
    int m_frame;
};

HalogramWindow::HalogramWindow()
    : m_program(0)
    , m_frame(0)
{}

void HalogramWindow::updateTexture()
{
//    m_texture = 0;
}

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
    "\n     TexCoord = (vec2(1.0) + posAttr.xy) * 0.5;"
    "\n }";

static const char *fragmentShaderSource =
    "\n uniform sampler2D Texture0;"
    "\n uniform float aspect;"
    "\n varying lowp vec2 TexCoord;"
    "\n float disto(vec4 src, vec4 orient) {"
    "\n     vec3 norm = normalize(src.rgb);"
    "\n     vec3 ofs = vec3(0.5, 2.0, 0.5) * abs(norm.rgb - orient.rgb);"
    "\n     return 1.0 - dot(ofs, ofs);"
    "\n }"
    "\n vec2 rotate(vec2 pos) {"
    "\n     float M_PI  = 3.14;"
    "\n     float angle = 0.0;"
    "\n     angle = float(pos.x > pos.y);"
    "\n     angle = angle - float(float(pos.x > pos.y) + float(pos.x < -pos.y) == 1.0) * 0.5;"
    "\n     angle = M_PI * angle;"
    "\n     mat2 rotate = mat2(cos(angle), sin(angle), -sin(angle), cos(angle));"
    "\n     return rotate * pos;"
    "\n }"
    "\n void main() {"
    "\n    vec2 center = TexCoord - vec2(0.5);"
    "\n    vec2 nCoord = rotate(center * vec2(aspect, 1.0))*vec2(2.0) + vec2(0.5, 0.0);"
//    "\n    vec4 color = vec4(nCoord, 0.0, 1.0);"
    "\n    vec4 color = texture2D(Texture0, nCoord);"
    "\n    vec4 direc = vec4(1.0, -0.3, -0.2, 0.0);"
//    "\n    vec4 direc = vec4(0.48,  0.37, 0.25, 0.0);"
    "\n    vec4 brush = vec4(0.2,  0.5,  0.8, 1.0);"
    "\n    float luma = 5.0 * (dot(color, direc) - 0.5);"
    "\n    center = nCoord - vec2(0.5);"
    "\n    float loc = max(0.0, 1.0 - 5.0 * dot(center, center));"
    "\n    gl_FragColor = loc * max(0.0, luma) * color;"

//    "\n    gl_FragColor = color;"
    "\n }";

//static const char *fragmentShaderSource =
//    "\n uniform sampler2D Texture0;"
//    "\n varying lowp vec2 TexCoord;"
//    "\n void main() {"
//    "\n    gl_FragColor = texture2D(Texture0, TexCoord);"
//    "\n }";

void HalogramWindow::initialize()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex  , vertexShaderSource  );
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    glGetUniformLocation(m_program->programId(), "Texture0");
    m_texture = new QOpenGLTexture(QImage("/home/allrandom/Photos/Webcam/2015-01-30-162436.jpg").mirrored());
    m_aspect = m_texture->width() / static_cast<GLfloat>(m_texture->height());
    // Set nearest filtering mode for texture minification
    m_texture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    m_texture->setWrapMode(QOpenGLTexture::ClampToBorder);
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
    updateTexture();
    m_texture->bind();

    m_program->setUniformValue("Texture0", 0);

    glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    m_program->setUniformValue("aspect", m_aspect);

    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);

    m_program->release();

    ++m_frame;
}
