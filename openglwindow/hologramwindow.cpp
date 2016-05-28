#include "hologramwindow.h"

#include <QDebug>

HologramWindow::HologramWindow()
    : m_program(0)
    , m_frame(0)
    , m_initializedTexture (false)
{}

void HologramWindow::updateTexture()
{
//    m_texture = 0;
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
    "\n    float luma = 5.0 * (dot(color, direc) - 0.3);"
    "\n    center = nCoord - vec2(0.5);"
    "\n    float loc = max(0.0, 1.0 - 5.0 * dot(center, center));"
    "\n    gl_FragColor = max(0.0, luma) * color;"

//    "\n    gl_FragColor = color;"
    "\n }";

//static const char *fragmentShaderSource =
//    "\n uniform sampler2D Texture0;"
//    "\n varying lowp vec2 TexCoord;"
//    "\n void main() {"
//    "\n    gl_FragColor = texture2D(Texture0, TexCoord);"
//    "\n }";

void HologramWindow::initialize()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex  , vertexShaderSource  );
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    glGetUniformLocation(m_program->programId(), "Texture0");
}

void HologramWindow::render()
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

    m_program->setUniformValue("Texture0", 0);

    glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    m_program->setUniformValue("aspect", m_aspect);

    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);

    m_program->release();

    ++m_frame;
}

void HologramWindow::onFrameAvailable(const QVideoFrame &frame)
{
    if (frame.isMapped())
    {
        if (m_initializedTexture)
        {
            //m_context->makeCurrent();
//            glBindTexture(GL_TEXTURE_2D, m_texture);

            m_context->makeCurrent(this);
            m_texture->bind();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame.width(), frame.height(),0, GL_BGRA, GL_UNSIGNED_BYTE, frame.bits());
        }
        else
        {
            QImage::Format  format =  frame.imageFormatFromPixelFormat(frame.pixelFormat());
            QImage image = QImage(frame.bits(), frame.width(), frame.height(), frame.bytesPerLine(), format);
            m_context->makeCurrent(this);
            m_texture = new QOpenGLTexture(image);
            m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
            m_aspect = m_texture->width() / static_cast<GLfloat>(m_texture->height());
            m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
            m_texture->setWrapMode(QOpenGLTexture::ClampToBorder);
            m_initializedTexture = true;
        }
    }

    renderNow();
}
