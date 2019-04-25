#include "main_window.h"

#include <hl_communication/utils.h>

#include <opencv2/imgproc.hpp>

#include <fstream>
#include <iostream>

#define CV_IMG_WIDTH 480
#define CV_IMG_HEIGHT 640
#define SPD_INTERVAL 30
#define SPD_INTERVAL_FF 1
#define SLIDER_INTERVAL 100.
#define SECONDS_TO_US 1000000

using namespace cv;
using namespace hl_communication;
using namespace hl_monitoring;

namespace qt_monitoring
{
MainWindow::MainWindow() : active_source("webcam"), camera_img(nullptr), top_view_img(nullptr), dt(30 * 1000)
{
  setWindowTitle(tr("INTERFACE"));

  zoneCentral = new QWidget;
  layout = new QGridLayout;

  labelVideo = new QLabel();
  labelVideo->setAlignment(Qt::AlignCenter);
  labelVideo->setStyleSheet("QLabel { background-color : red}");
  labelVideo->setScaledContents(true);

  slider_value_label = new QLabel(this);
  slider_value_label->setText("0");
  slider = new QSlider(Qt::Horizontal, this);

  buttonPause = new QPushButton("PLAY");
  buttonFastForward = new QPushButton("x2");

  connect(buttonPause, SIGNAL(released()), this, SLOT(clickPause()));
  connect(buttonFastForward, SIGNAL(released()), this, SLOT(clickFastForward()));

  // TODO: update positions
  layout->addWidget(labelVideo, 1, 1, 5, 4);
  layout->addWidget(slider, 7, 1, 1, 4);
  layout->addWidget(slider_value_label, 8, 1, 1, 1);
  layout->addWidget(buttonPause, 8, 2, 1, 1);
  layout->addWidget(buttonFastForward, 8, 3, 1, 1);

  zoneCentral->setLayout(layout);
  setCentralWidget(zoneCentral);

  playing = false;
  speed_ratio = 1;

  // TODO: add as parameters
  field.loadFile("eirlab.json");
  manager.loadConfig("replay.json");

  updateSource();
  update();

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(update()));
  timer->start(dt / 1000);
}

void MainWindow::updateSource()
{
  std::set<std::string> image_provider = manager.getImageProvidersNames();

  if (image_provider.count(active_source) == 0)
  {
    throw std::logic_error(HL_DEBUG + " source '" + active_source + "' not found");
  }
  initial_time = manager.getImageProvider(active_source).getStart();
  end_time = manager.getImageProvider(active_source).getEnd();

  // Slider has 1 sec step
  slider->setRange(0, (end_time - initial_time) / 1000000);
}

void MainWindow::updateTime()
{
  uint64_t total_duration = end_time - initial_time;
  // If slider has moved: update 'now' based on it
  if (slider->value() != old_slider_value)
  {
    now = initial_time + 1000 * 1000 * slider->value();
  }

  if (playing)
  {
    if (manager.isLive())
    {
      now = getTimeStamp();
    }
    else
    {
      now += dt * speed_ratio;
    }
  }
  old_slider_value = (now - initial_time) / (1000*1000);
  slider->setValue(old_slider_value);
  char str[20];
  if (!manager.isLive() && now >= end_time && end_time != 0)
  {
    now = end_time;
    sprintf(str, "end of video");
  }
  else
  {
    uint64_t elapsed_since_start = now - initial_time;
    uint64_t elapsed_ms = elapsed_since_start / 1000;
    uint64_t elapsed_seconds = elapsed_ms / 1000;
    uint64_t elapsed_minutes = elapsed_seconds / 60;
    elapsed_ms = elapsed_ms % 1000;
    elapsed_seconds = elapsed_seconds % 60;

    sprintf(str, "%.02lu:%.02lu:%.03lu\n", elapsed_minutes, elapsed_seconds, elapsed_ms);
  }
  slider_value_label->setText(str);
}

void MainWindow::updateManager()
{
  if (playing)
  {
    manager.update();
  }
}

void MainWindow::updateAnnotations()
{
  MessageManager::Status status = manager.getMessageManager().getStatus(now);
  std::map<std::string, CalibratedImage> images_by_source = manager.getCalibratedImages(now);

  if (images_by_source.count(active_source) == 0)
  {
    throw std::runtime_error(HL_DEBUG + " no source named '" + active_source + "'");
  }

  const CalibratedImage& calibrated_img = images_by_source.at(active_source);
  camera_img = new cv::Mat(calibrated_img.getImg().clone());
  top_view_img = new cv::Mat(top_view_drawer.getImg(field));
  if (calibrated_img.isFullySpecified())
  {
    const CameraMetaInformation& camera_information = calibrated_img.getCameraInformation();

    team_drawer.drawNatural(camera_information, status, camera_img);
    team_drawer.drawTopView(field, top_view_drawer, status, top_view_img);
  }

  cvtColor(*camera_img, *camera_img, CV_BGR2RGB);
  this->labelVideo->setPixmap(QPixmap::fromImage(
      QImage(camera_img->data, camera_img->cols, camera_img->rows, camera_img->step, QImage::Format_RGB888)));
}

void MainWindow::update()
{
  updateTime();
  updateManager();
  updateAnnotations();

  labelVideo->setPixmap(QPixmap::fromImage(
      QImage(camera_img->data, camera_img->cols, camera_img->rows, camera_img->step, QImage::Format_RGB888)));
}

void MainWindow::clickPause()
{
  playing = !playing;
  if (playing)
  {
    this->buttonPause->setText("PAUSE");
  }
  else
  {
    this->buttonPause->setText("PLAY");
  }
}

void MainWindow::clickFastForward()
{
  speed_ratio *= 2;
  // Maximal Speed
  int max_speed = 16;
  if (speed_ratio > max_speed)
  {
    speed_ratio = 1;
  }
  int next_speed = speed_ratio * 2;
  if (next_speed > max_speed)
  {
    next_speed = 1;
  }
  this->buttonFastForward->setText(("x" + std::to_string(next_speed)).c_str());
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
  QMainWindow::resizeEvent(event);
  //  for (auto it : teamPanels)
  //  {
  //    (it.second)->setSizeRobotArea();
  //  }
}

}  // namespace qt_monitoring
