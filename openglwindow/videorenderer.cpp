#include "videorenderer.h"

#include <VLCQtCore/Instance.h>
#include <VLCQtCore/MediaPlayer.h>
#include <VLCQtWidgets/WidgetVideo.h>
#include <VLCQtCore/Common.h>
#include <VLCQtCore/Media.h>

#include "ui_form.h"

VideoRenderer::VideoRenderer()
    : m_ui(new Ui::Form)
{
    m_ui->setupUi(this);

    auto instance = new VlcInstance(VlcCommon::args(), this);
    auto mediaPlayer = new VlcMediaPlayer(instance);
    auto videoWidget = new VlcWidgetVideo(this);

    mediaPlayer->setVideoWidget(videoWidget);
    videoWidget->setMediaPlayer(mediaPlayer);

    m_ui->horizontalLayout_2->addWidget(videoWidget);

    auto vlcMedia = new VlcMedia("http://192.168.43.1:8080/video", instance);
    mediaPlayer->open(vlcMedia);
    mediaPlayer->play();
}
