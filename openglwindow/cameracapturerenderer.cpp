#include "cameracapturerenderer.h"

#include <QImageWriter>
#include <QDebug>

#include "ui_form.h"

CameraCaptureRenderer::CameraCaptureRenderer(QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::Form)
{
    m_ui->setupUi(this);
}

void CameraCaptureRenderer::onFrameAvailable(const QImage &image)
{
    //m_ui->label->setPixmap(QPixmap::fromImage(image));

//    QImageWriter writer("D:/test.jpg");
//    if(!writer.write(image))
//    {
//        qDebug() << writer.errorString();
//    }
}
