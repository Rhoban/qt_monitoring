#include "pov_manager.h"

namespace qt_monitoring
{
std::map<POVManager::PointOfView, std::string> POVManager::pov_to_str = {
  { PointOfView::global, "global" },
  { PointOfView::team, "team" },
  { PointOfView::player, "player" },
};
std::map<std::string, POVManager::PointOfView> POVManager::str_to_pov = {
  { "global", PointOfView::global },
  { "team", PointOfView::team },
  { "player", PointOfView::player },
};

POVManager::POVManager() : current_pov(PointOfView::global), current_team_idx(0), current_player_idx(0)
{
  internal_layout = new QHBoxLayout();
  pov_choice = new QComboBox();
  pov_choice->insertItem(0, POVManager::pov_to_str[PointOfView::global].c_str());
  pov_choice->insertItem(1, POVManager::pov_to_str[PointOfView::team].c_str());
  pov_choice->insertItem(2, POVManager::pov_to_str[PointOfView::player].c_str());
  QObject::connect(pov_choice, SIGNAL(activated(const QString&)), this, SLOT(onPOVUpdate(const QString&)));
  team_choice = new QComboBox();
  team_choice->insertItem(0, "Team A");
  team_choice->insertItem(1, "Team B");
  QObject::connect(team_choice, SIGNAL(activated(int)), this, SLOT(onTeamUpdate(int)));
  player_choice = new QComboBox();
  // TODO: add a method to set the number of robots
  for (int player_idx = 0; player_idx < 6; player_idx++)
  {
    player_choice->insertItem(player_idx, ("Player" + std::to_string(player_idx + 1)).c_str());
  }
  QObject::connect(player_choice, SIGNAL(activated(int)), this, SLOT(onPlayerUpdate(int)));
  internal_layout->addWidget(pov_choice);
  internal_layout->addWidget(team_choice);
  internal_layout->addWidget(player_choice);
  updateVisibility();
  this->setLayout(internal_layout);
}

POVManager::PointOfView POVManager::getPOV() const
{
  return current_pov;
}

int POVManager::getTeamIdx() const
{
  return current_team_idx;
}

int POVManager::getPlayerId() const
{
  return current_player_idx + 1;
}

void POVManager::onPOVUpdate(const QString& q_text)
{
  std::string text = q_text.toStdString();
  current_pov = str_to_pov.at(text);
  updateVisibility();
}

void POVManager::onTeamUpdate(int new_team_idx)
{
  current_team_idx = new_team_idx;
}

void POVManager::onPlayerUpdate(int new_player_idx)
{
  current_player_idx = new_player_idx;
}

void POVManager::updateVisibility()
{
  switch (current_pov)
  {
    case PointOfView::global:
      team_choice->hide();
      player_choice->hide();
      break;
    case PointOfView::team:
      team_choice->show();
      player_choice->hide();
      break;
    case PointOfView::player:
      team_choice->show();
      player_choice->show();
      break;
  }
}

}  // namespace qt_monitoring
