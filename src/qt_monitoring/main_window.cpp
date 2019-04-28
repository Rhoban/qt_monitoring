#include "main_window.h"

#include <qt_monitoring/utils.h>

#include <hl_communication/utils.h>

#include <opencv2/imgproc.hpp>

#include <fstream>
#include <iostream>

using namespace cv;
using namespace hl_communication;
using namespace hl_monitoring;

namespace qt_monitoring
{
MainWindow::MainWindow(const std::string& manager_path, const std::string& field_path)
  : now(0), dt(30 * 1000), memory_duration(2 * 1000 * 1000), old_slider_value(0)
{
  field.loadFile(field_path);
  manager.loadConfig(manager_path);

  setWindowTitle(tr("QTMonitor"));

  zoneCentral = new QWidget;
  layout = new QGridLayout;

  label_video = new QLabel();
  label_video->setAlignment(Qt::AlignCenter);
  label_video->setScaledContents(false);
  label_top_view = new QLabel();
  label_top_view->setAlignment(Qt::AlignCenter);
  label_top_view->setScaledContents(false);


  if (!manager.isLive())
  {
    slider_value_label = new QLabel(this);
    slider_value_label->setText("0");
    slider = new QSlider(Qt::Horizontal, this);
    buttonPause = new QPushButton("PLAY");
    buttonFastForward = new QPushButton("x2");
    connect(buttonPause, SIGNAL(released()), this, SLOT(clickPause()));
    connect(buttonFastForward, SIGNAL(released()), this, SLOT(clickFastForward()));
    layout->addWidget(slider, 7, 1, 1, 4);
    layout->addWidget(slider_value_label, 8, 1, 1, 1);
    layout->addWidget(buttonPause, 8, 2, 1, 1);
    layout->addWidget(buttonFastForward, 8, 3, 1, 1);
  }

  teams.push_back(new TeamPanel());
  teams.push_back(new TeamPanel());

  // TODO: update positions and make it easier to read
  layout->addWidget(teams[0], 1, 0, 8, 1);
  layout->addWidget(teams[1], 1, 5, 8, 1);
  layout->addWidget(label_video, 1, 1, 3, 4);
  layout->addWidget(label_top_view, 4, 1, 3, 4);

  zoneCentral->setLayout(layout);
  setCentralWidget(zoneCentral);

  playing = manager.isLive();
  speed_ratio = 1;

  std::set<std::string> sources = manager.getImageProvidersNames();
  if (sources.size() > 0)
  {
    active_source = *(sources.begin());
    updateSource();
  }
  update();

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(update()));
  timer->start(dt / 1000);
}

void MainWindow::updateSource()
{
  if (active_source == "")
  {
    return;
  }
  std::set<std::string> image_provider = manager.getImageProvidersNames();

  if (image_provider.count(active_source) == 0)
  {
    throw std::logic_error(HL_DEBUG + " source '" + active_source + "' not found");
  }
  initial_time = manager.getImageProvider(active_source).getStart();
  end_time = manager.getImageProvider(active_source).getEnd();

  if (!manager.isLive())
  {
    // Slider has 1 sec step
    slider->setRange(0, (end_time - initial_time) / 1000000);
  }

  now = std::max(std::min(now, end_time), initial_time);
}

void MainWindow::updateTime()
{
  // If slider has moved: update 'now' based on it
  if (!manager.isLive() && slider->value() != old_slider_value)
  {
    now = initial_time + 1000 * 1000 * slider->value();
  }

  if (playing)
  {
    if (manager.isLive())
    {
      // Dirty hack: since live image_providers are not supporting request of frames in the past, always require a frame
      // from the future
      double anticipation_ms = 50;
      now = getTimeStamp() + anticipation_ms * 1000;
    }
    else
    {
      now += dt * speed_ratio;
    }
  }
  if (!manager.isLive())
  {
    old_slider_value = (now - initial_time) / (1000 * 1000);
    slider->setValue(old_slider_value);
  }
  char str[30];
  if (!manager.isLive())
  {
    if (now >= end_time && end_time != 0)
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
}

void MainWindow::updateManager()
{
  manager.update();
  status = manager.getMessageManager().getStatus(now);
}

void MainWindow::updateTeams()
{
  std::map<uint32_t, int> index_by_team_id;
  int nb_teams = status.gc_message.teams_size();
  bool has_gc_message = nb_teams != 0;
  if (has_gc_message && nb_teams != 2)
  {
    throw std::logic_error(HL_DEBUG + "Invalid number of teams in gc_message");
  }
  if (has_gc_message)
  {
    for (size_t idx = 0; idx < 2; idx++)
    {
      const GCTeamMsg& team_msg = status.gc_message.teams(idx);
      int team_id = team_msg.team_number();
      index_by_team_id[team_id] = idx;
      teams[idx]->updateTeamData("Team " + std::to_string(team_id), team_msg.score());
    }
  }

  int default_team_idx = 0;
  std::map<uint32_t, std::vector<RobotMsg>> robots_by_team = status.getRobotsByTeam();
  if (robots_by_team.size() > 2)
  {
    throw std::logic_error("Too many teams in GC message");
  }
  for (const auto& entry : robots_by_team)
  {
    uint32_t team_id = entry.first;

    int team_idx = default_team_idx;
    if (has_gc_message)
    {
      if (index_by_team_id.count(team_id) == 0)
      {
        throw std::logic_error(HL_DEBUG + "Unknown index for team " + std::to_string(team_id));
      }
      team_idx = index_by_team_id[team_id];
    }
    else
    {
      teams[team_idx]->updateTeamData("Team " + std::to_string(team_id), 0);
    }
    // TODO: store teams in a .json file
    teams[team_idx]->treatMessages(status.gc_message.teams(team_idx), entry.second);
    default_team_idx++;
  }
}

void MainWindow::updateAnnotations()
{
  if (active_source != "")
  {
    try
    {
      const CalibratedImage& calibrated_img = manager.getCalibratedImage(active_source, now);
      camera_img = cv::Mat(calibrated_img.getImg().clone());
      if (calibrated_img.isFullySpecified())
      {
        const CameraMetaInformation& camera_information = calibrated_img.getCameraInformation();
        field.tagLines(camera_information, &camera_img, cv::Scalar(0, 0, 0), 1, 10);
        team_drawer.drawNatural(camera_information, status, &camera_img);
      }
    }
    catch (const std::runtime_error& exc)
    {
      std::cerr << "Failed to retrieve annotation for '" << active_source << "' at " << now << std::endl
                << "\twhat(): " << exc.what() << std::endl;
    }
  }

  top_view_img = cv::Mat(top_view_drawer.getImg(field));
  team_drawer.drawTopView(field, top_view_drawer, status, &top_view_img);
}

void MainWindow::update()
{
  // TODO: width + height as parameters/updated on release
  int w = 1000;
  int h = 800;
  if (!camera_img.empty())
  {
    w /= 2;
    h /= 2;
  }
  updateSource();
  updateTime();
  updateManager();
  top_view_drawer.setImgSize(cv::Size(w, h));
  updateTeams();
  updateAnnotations();

  if (!camera_img.empty())
  {
    cvtColor(camera_img, camera_img, CV_BGR2RGB);
    QPixmap camera_pixmap = QPixmap::fromImage(cvToQImage(camera_img));
    label_video->setPixmap(camera_pixmap.scaled(w, h, Qt::KeepAspectRatio));
  }
  if (!top_view_img.empty())
  {
    cvtColor(top_view_img, top_view_img, CV_BGR2RGB);
    QPixmap top_view_pixmap = QPixmap::fromImage(cvToQImage(top_view_img));
    label_top_view->setPixmap(top_view_pixmap.scaled(w, h, Qt::KeepAspectRatio));
  }
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
}

}  // namespace qt_monitoring
