#include "main_window.h"

#include <qt_monitoring/globals.h>
#include <qt_monitoring/utils.h>

#include <hl_communication/utils.h>
#include <hl_communication/game_controller_utils.h>

#include <opencv2/imgproc.hpp>

#include <fstream>
#include <iostream>

using namespace cv;
using namespace hl_communication;
using namespace hl_monitoring;

namespace qt_monitoring
{
MainWindow::MainWindow(std::unique_ptr<hl_monitoring::MonitoringManager> manager_)
  : manager(std::move(manager_)), now(0), dt(30 * 1000), memory_duration(2 * 1000 * 1000), old_slider_value(0)
{
  Globals::team_manager = manager->getTeamManager();

  setWindowTitle(tr("QTMonitor"));

  zoneCentral = new QWidget;
  layout = new QGridLayout;

  video_widget = new VideoWidget();

  if (!manager->isLive())
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

  pov_manager = new POVManager();

  teams.push_back(new TeamPanel());
  teams.push_back(new TeamPanel());

  // TODO: update positions and make it easier to read
  layout->addWidget(teams[0], 0, 0, 9, 1);
  layout->addWidget(teams[1], 0, 5, 9, 1);
  layout->addWidget(pov_manager, 0, 1, 1, 4);
  layout->addWidget(video_widget, 1, 1, 6, 4);

  zoneCentral->setLayout(layout);
  setCentralWidget(zoneCentral);

  playing = manager->isLive();
  speed_ratio = 1;

  video_widget->updateAvailableSources(manager->getImageProvidersNames());
  updateSource();
  update();

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(update()));
  timer->start(dt / 1000);

  setStyleSheet("QMainWindow { background: white; }");
}

void MainWindow::updateSource()
{
  initial_time = manager->getMessageManager().getStart();
  end_time = manager->getMessageManager().getEnd();

  std::set<std::string> active_sources = video_widget->getStreamSelector().getActiveSources();
  for (const std::string& source_name : active_sources)
  {
    if (source_name == "TopView")
      continue;
    initial_time = std::min(initial_time, manager->getImageProvider(source_name).getStart());
    end_time = std::max(end_time, manager->getImageProvider(source_name).getEnd());
  }

  if (!manager->isLive())
  {
    // Slider has 1 sec step, time_stamps are micro_seconds
    slider->setRange(0, (end_time - initial_time) / std::pow(10, 6));
  }

  now = std::max(std::min(now, end_time), initial_time);
}

void MainWindow::updateTime()
{
  // If slider has moved: update 'now' based on it
  if (!manager->isLive() && slider->value() != old_slider_value)
  {
    now = initial_time + 1000 * 1000 * slider->value();
  }

  if (playing)
  {
    if (manager->isLive())
    {
      // Dirty hack: since live image_providers are not supporting request of frames in the past, always require a
      // frame from the future
      double anticipation_ms = 50;
      now = getTimeStamp() + anticipation_ms * 1000;
    }
    else
    {
      now += dt * speed_ratio;
    }
  }
  if (!manager->isLive())
  {
    old_slider_value = (now - initial_time) / (1000 * 1000);
    slider->setValue(old_slider_value);
  }
  char str[30];
  if (!manager->isLive())
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
  manager->update();
  status = manager->getMessageManager().getStatus(now, memory_duration);
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
    auto& firstTeam = status.gc_message.teams().Get(0);
    if (firstTeam.has_team_color())
    {
      QString redStyle = "background-color: #f6e8fa; border-radius:5px; color: #9a1cb9";
      QString blueStyle = "background-color: #e8f0fa; border-radius:5px; color: #124e97";

      if (firstTeam.team_color() == hl_communication::getBlueTeamColor())
      {
        teams[0]->setStyleSheet(blueStyle);
        teams[1]->setStyleSheet(redStyle);
      }
      else
      {
        teams[0]->setStyleSheet(redStyle);
        teams[1]->setStyleSheet(blueStyle);
      }
    }
  }

  for (size_t idx = 0; idx < 2; idx++)
  {
    if (pov_manager->getPOV() != POVManager::PointOfView::global && pov_manager->getTeamIdx() != (int)idx)
    {
      teams[idx]->hide();
    }
    else
    {
      teams[idx]->show();
    }
    if (has_gc_message)
    {
      const GCTeamMsg& team_msg = status.gc_message.teams(idx);
      uint32_t team_id = team_msg.team_number();
      index_by_team_id[team_id] = idx;
      teams[idx]->updateTeamData(team_id, team_msg.score());
      pov_manager->setTeamId(idx, team_id);
    }
  }
  // If some team id have not been set (it means game controller messages have not been read yet)
  // iterates over
  if (teams[0]->getTeamId() == 0 || teams[1]->getTeamId() == 0)
  {
    for (const auto& entry : status.getRobotsByTeam())
    {
      bool team_registered = false;
      for (int team_idx = 0; team_idx < 2 && !team_registered; team_idx++)
      {
        uint32_t actual_team_id = teams[team_idx]->getTeamId();
        if (actual_team_id == entry.first)
        {
          team_registered = true;
          continue;
        }
        if (actual_team_id == 0)
        {
          teams[team_idx]->updateTeamData(entry.first, 0);
          team_registered = true;
        }
      }
      if (!team_registered)
      {
        throw std::runtime_error(HL_DEBUG + " too much teams on the network, not accepting team " +
                                 std::to_string(entry.first));
      }
    }
  }

  std::map<uint32_t, std::vector<RobotMsg>> robots_by_team = status.getRobotsByTeam();
  if (robots_by_team.size() > 2)
  {
    throw std::logic_error("Too many teams in GC message");
  }
  for (int team_idx : { 0, 1 })
  {
    uint32_t team_id = teams[team_idx]->getTeamId();

    if (has_gc_message)
    {
      if (index_by_team_id.count(team_id) == 0)
      {
        throw std::logic_error(HL_DEBUG + "Unknown index for team " + std::to_string(team_id));
      }
      team_idx = index_by_team_id[team_id];
      teams[team_idx]->treatMessages(status.gc_message.teams(team_idx), robots_by_team[team_id]);
    }
    else
    {
      teams[team_idx]->updateTeamData(team_id, 0);
      teams[team_idx]->treatMessages(GCTeamMsg(), robots_by_team[team_id]);
    }
  }
}

void MainWindow::updatePOV()
{
  POVManager::PointOfView pov = pov_manager->getPOV();
  int team_idx = pov_manager->getTeamIdx();
  int team_focus = -1;
  int player_focus = -1;
  if (pov != POVManager::PointOfView::global)
  {
    team_focus = teams[team_idx]->getTeamId();
  }
  if (pov == POVManager::PointOfView::player)
  {
    player_focus = pov_manager->getPlayerId();
  }
  video_widget->updateFocus(team_focus, player_focus);
  for (int idx = 0; idx < 2; idx++)
  {
    int team_player_focus = team_idx == idx ? player_focus : -1;
    teams[idx]->update(now, team_player_focus);
  }
}

void MainWindow::update()
{
  updateSource();
  updateTime();
  updateManager();
  updatePOV();
  updateTeams();
  video_widget->updateContent(manager->getCalibratedImages(now), manager->getField(), status);
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
