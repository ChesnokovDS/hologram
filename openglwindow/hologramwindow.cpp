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
    "\n const vec2  ortU = vec2(-0.565953,  0.824437);" // |ortU| == 1
    "\n const vec2  ortV = vec2(-0.824437, -0.565953);" // |ortV| == 1
    "\n const mat4  yuv2rgb = mat4( 1.164383561643836,  1.164383561643836,  1.164383561643836, 0.0,"
    "\n                             0.0              , -0.391762290094914,  2.017232142857142, 0.0,"
    "\n                             1.596026785714286, -0.812967647237771,  0.0              , 0.0,"
    "\n                            -0.874202217873451,  0.531667823499146, -1.085630789302022, 0.0);"
    "\n const mat4  rgb2yuv = mat4( 0.256788224, -0.148222893,  0.439215660, 0.0,"
    "\n                             0.504129410, -0.290992767, -0.367788285, 0.0,"
    "\n                             0.097905874,  0.439215690, -0.071427367, 0.0,"
    "\n                             0.062745086,  0.501960814,  0.501960754, 0.0);"
    "\n const float cos_alpha = 0.173648;"
    "\n const float ctg_alpha = 0.176327;"
    "\n const float lBound = 0.00001;"
    "\n const float uBound = 0.0001;"
    "\n "
/// @todo VEM-2146 Достойно отдельного шейдера
    "\n vec3 GetYUV_Black(        ) { return ( vec3( 16., 128., 128.)/255. ); }"
    "\n vec4 GetYUV_Black(float  a) { return ( vec4( GetYUV_Black(),  a )  ); }"
    "\n vec3 ClampYUV    (vec3 yuv) { return ( clamp(yuv, vec3(16./255, 0., 0.    ), vec3(235./255., 1., 1.    )) ); }"
    "\n vec4 ClampYUV    (vec4 yuv) { return ( clamp(yuv, vec4(16./255, 0., 0., 0.), vec4(235./255., 1., 1., 1.)) ); }"
    "\n vec4 YUV_To_RGB(vec4 yuv)"
    "\n {"
    "\n     vec4 rgb   =            yuv2rgb[3];"
    "\n          rgb  += yuv.rrrr * yuv2rgb[0];"
    "\n          rgb  += yuv.gggg * yuv2rgb[1];"
    "\n          rgb  += yuv.bbbb * yuv2rgb[2];"
    "\n          rgb.a = yuv.a;"
    "\n     return ( clamp(rgb, 0., 1.) );"
    "\n }"
    "\n vec4 gray_scale(vec4 rgba)"
    "\n {"
    "\n     float lum = dot(rgba, vec4(0.2126, 0.7152, 0.0722, 0.0));"
    "\n     return vec4(0, 0, 0, rgba.a);"
    "\n }"
    "\n vec4 RGB_To_YUV(vec4 rgb)"
    "\n {"
    "\n     vec4 yuv   =            rgb2yuv[3];"
    "\n          yuv  += rgb.rrrr * rgb2yuv[0];"
    "\n          yuv  += rgb.gggg * rgb2yuv[1];"
    "\n          yuv  += rgb.bbbb * rgb2yuv[2];"
    "\n          yuv.a = rgb.a;"
    "\n     return ( ClampYUV(yuv) );"
    "\n }"
    "\n "
    "\n float Hiperbolic(vec2 uv)"
    "\n {"
    "\n     return dot(vec2(1, -ctg_alpha * ctg_alpha), (uv * uv));"  // параметрическая функция гиперболлы вдоль вектора uv и с деректрисами отклонёнпыми от uv на угол alpha
    "\n }"
    "\n "
    "\n vec4 ProcessPixel(vec4 rgb)"
    "\n {"
    "\n     vec4 src = RGB_To_YUV(rgb);"
    "\n     vec2 uv = src.gb - GetYUV_Black().gb;"
    "\n     float uvLen = length(uv);"
    "\n     float l = dot(uv, ortU);"
    "\n     float h  = abs(dot(uv, ortV));"
    "\n     vec2  coord = vec2(l, h);"
    "\n     float hiper = Hiperbolic(coord);"
    "\n     "
    "\n     if (uvLen < 0.004 || l < uvLen * cos_alpha || hiper < lBound)"
    "\n         return gray_scale( rgb );"
    "\n     "
    "\n     if (hiper > uBound)"
    "\n         return rgb;"
    "\n     "
    "\n     return mix(gray_scale(rgb), rgb, (hiper - lBound) / (uBound - lBound));"
    "\n }"
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
    "\n    vec4 color = ProcessPixel(texture2D(Texture0, nCoord));"
//    "\n    vec4 direc = vec4(1.0, -0.3, -0.2, 0.0);"
//    "\n    vec4 direc = vec4(0.48,  0.37, 0.25, 0.0);"
//    "\n    vec4 brush = vec4(0.2,  0.5,  0.8, 1.0);"
//    "\n    float luma = 5.0 * (dot(color, direc) - 0.3);"
//    "\n    center = nCoord - vec2(0.5);"
//    "\n    float loc = max(0.0, 1.0 - 5.0 * dot(center, center));"
//    "\n    gl_FragColor = max(0.0, luma) * color;"

    "\n    gl_FragColor = color;"
    "\n }";


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
    if ( !m_context)
        return;

    if (frame.isMapped())
    {
        if (m_initializedTexture)
        {
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
