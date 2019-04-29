#include "team_panel.h"

#include <qt_monitoring/globals.h>

using namespace hl_communication;

namespace qt_monitoring
{
TeamPanel::RobotStatus getRobotStatus(const GCRobotMsg& robot_msg)
{
  if (!robot_msg.has_penalty() || robot_msg.penalty() == 0)
  {
    return TeamPanel::RobotStatus::Active;
  }
  else if (robot_msg.penalty() == 14)
  {
    return TeamPanel::RobotStatus::Substitute;
  }
  return TeamPanel::RobotStatus::Penalized;
}

TeamPanel::TeamPanel() : team_id(0), score(0)
{
  internal_layout = new QVBoxLayout();
  team_label = new QLabel();
  team_label->setFont(QFont("Arial", 18, 3, false));
  team_label->setAlignment(Qt::AlignCenter | Qt::AlignTop);
  internal_layout->addWidget(team_label);
  internal_layout->setAlignment(Qt::AlignCenter | Qt::AlignTop);

  robots_by_status = { { RobotStatus::Active, new PlayerGroup("Active players") },
                       { RobotStatus::Penalized, new PlayerGroup("Penalized players") },
                       { RobotStatus::Substitute, new PlayerGroup("Substitutes") } };
  for (auto& entry : robots_by_status)
  {
    internal_layout->addWidget(entry.second);
  }

  updateTeamLabel();

  this->setLayout(internal_layout);
}

void TeamPanel::treatMessages(const GCTeamMsg& team_msg, const std::vector<RobotMsg>& robots_msg)
{
  std::map<RobotStatus, std::vector<RobotMsg>> messages_by_status = { { RobotStatus::Active, {} },
                                                                      { RobotStatus::Penalized, {} },
                                                                      { RobotStatus::Substitute, {} } };
  for (const RobotMsg& robot_msg : robots_msg)
  {
    int player_id = robot_msg.robot_id().robot_id();
    RobotStatus status = RobotStatus::Active;
    if (team_msg.robots_size() >= player_id)
    {
      getRobotStatus(team_msg.robots(player_id - 1));
    }
    messages_by_status[status].push_back(robot_msg);
  }
  // If changes occurs, first all robots are removed in order to avoid increasing the height of the layout
  bool has_changed = false;
  for (const auto& entry : messages_by_status)
  {
    if (robots_by_status[entry.first]->getNbRobots() != entry.second.size())
    {
      has_changed = true;
    }
  }
  if (has_changed)
  {
    for (auto& entry : robots_by_status)
    {
      entry.second->clearRobots();
    }
  }
  for (const auto& entry : messages_by_status)
  {
    robots_by_status[entry.first]->treatMessages(entry.second);
  }
}

void TeamPanel::updateTeamData(uint32_t new_team_id, int new_score)
{
  if (score != new_score || team_id != new_team_id)
  {
    score = new_score;
    team_id = new_team_id;
    updateTeamLabel();
  }
}

void TeamPanel::updateTeamLabel()
{
  std::string team_name;
  if (Globals::team_manager.hasTeam(team_id))
  {
    team_name = Globals::team_manager.getTeam(team_id).getName();
  }
  else
  {
    team_name = "Team " + std::to_string(team_id);
  }
  std::string new_text = std::to_string(score) + " : " + team_name;
  team_label->setText(new_text.c_str());
}

}  // namespace qt_monitoring
