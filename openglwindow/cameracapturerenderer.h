#include <QWidget>


namespace Ui{
    class Form;
}

class CameraCaptureRenderer : public QWidget
{
    Q_OBJECT

public:
    CameraCaptureRenderer(QWidget *parent = 0);

public slots:
    void onFrameAvailable(const QImage &image);

private:
    Ui::Form *m_ui;
};
