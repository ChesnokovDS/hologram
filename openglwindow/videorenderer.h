#pragma once

#include <QMediaPlayer>
#include <QVideoWidget>

namespace Ui{
    class Form;
}


class VideoRenderer : public QVideoWidget
{
public:
    VideoRenderer();

private:
    Ui::Form *m_ui;
};
