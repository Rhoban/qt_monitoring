#include "main_window.h"

#include <qt_monitoring/globals.h>
#include <qt_monitoring/utils.h>

#include <hl_communication/utils.h>
#include <hl_communication/game_controller_utils.h>

#include <opencv2/imgproc.hpp>

#include <fstream>

using namespace cv;
using namespace hl_communication;
using namespace hl_monitoring;

namespace qt_monitoring
{
MainWindow::MainWindow(std::unique_ptr<hl_monitoring::MonitoringManager> manager)
{
  Globals::team_manager = manager->getTeamManager();

  setWindowTitle(tr("QTMonitor"));

  zoneCentral = new QWidget;
  layout = new QGridLayout;

  timer = new QTimer(this);
  video_widget = new VideoWidget(std::move(manager), timer);

  pov_manager = new POVManager();

  tools_bar = new ToolsBar(std::move(manager));

  teams.push_back(new TeamPanel());
  teams.push_back(new TeamPanel());

  // TODO: update positions and make it easier to read
  layout->addWidget(teams[0], 0, 0, 2, 1);
  layout->addWidget(teams[1], 0, 2, 2, 1);
  layout->addWidget(pov_manager, 0, 1, 1, 1);
  layout->addWidget(video_widget, 1, 1, 1, 1);

  /*
  layout->addWidget(tools_bar, 0, 1, 1, 1);
  layout->addWidget(pov_manager, 1, 1, 1, 1);
  layout->addWidget(video_widget, 2, 1, 1, 1);*/

  teams[0]->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  teams[1]->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  zoneCentral->setLayout(layout);
  setCentralWidget(zoneCentral);

  timer->start(30);
  connect(timer, SIGNAL(timeout()), this, SLOT(update()));
  setStyleSheet("QMainWindow { background: white; }");
  update();
}

const hl_communication::MessageManager::Status& MainWindow::getStatus() const
{
  return video_widget->getStatus();
}

void MainWindow::updateTeams()
{
  std::map<uint32_t, int> index_by_team_id;
  int nb_teams = getStatus().gc_message.teams_size();
  bool has_gc_message = nb_teams != 0;
  if (has_gc_message && nb_teams != 2)
  {
    throw std::logic_error(HL_DEBUG + "Invalid number of teams in gc_message");
  }

  if (has_gc_message)
  {
    auto& firstTeam = getStatus().gc_message.teams().Get(0);
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
      const GCTeamMsg& team_msg = getStatus().gc_message.teams(idx);
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
    for (const auto& entry : getStatus().getRobotsByTeam())
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

  std::map<uint32_t, std::vector<RobotMsg>> robots_by_team = getStatus().getRobotsByTeam();
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
      teams[team_idx]->treatMessages(getStatus().gc_message.teams(team_idx), robots_by_team[team_id]);
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
    teams[idx]->update(video_widget->getTS(), team_player_focus);
  }
}

void MainWindow::updateToolsBar()
{
  tools_bar->updateGCSource(video_widget->getTS(), video_widget->getInterferingGCSource());
  tools_bar->updateCameraSource(video_widget->getActiveSources());
}

void MainWindow::update()
{
  updatePOV();
  updateTeams();
  video_widget->update();
  updateToolsBar();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
  QMainWindow::resizeEvent(event);
}

}  // namespace qt_monitoring
