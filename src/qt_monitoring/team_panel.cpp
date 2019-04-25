#include "team_panel.h"

using namespace hl_communication;

namespace qt_monitoring
{
TeamPanel::TeamPanel() : team_name("team_name"), score(0)
{
  internal_layout = new QVBoxLayout();
  team_label = new QLabel();
  team_label->setFont(QFont("Arial", 18, 3, false));
  team_label->setAlignment(Qt::AlignCenter | Qt::AlignTop);
  internal_layout->addWidget(team_label);
  internal_layout->setAlignment(Qt::AlignCenter | Qt::AlignTop);

  updateTeamLabel();
  setNbActiveRobots(0);

  this->setLayout(internal_layout);
}

void TeamPanel::treatMessages(const std::vector<RobotMsg>& robots_msg)
{
  if (robots.size() != robots_msg.size())
  {
    setNbActiveRobots(robots_msg.size());
  }
  for (size_t idx = 0; idx < robots_msg.size(); idx++)
  {
    robots[idx]->treatMessage(robots_msg[idx]);
  }
}

void TeamPanel::updateTeamData(const std::string& new_team_name, int new_score)
{
  score = new_score;
  team_name = new_team_name;
  updateTeamLabel();
}
void TeamPanel::setNbActiveRobots(int nb_robots)
{
  for (size_t i = 0; i < robots.size(); i++)
  {
    internal_layout->removeWidget(robots[i]);
    delete (robots[i]);
  }
  robots.clear();
  for (int i = 0; i < nb_robots; i++)
  {
    PlayerWidget* robot = new PlayerWidget();
    robots.push_back(robot);
    internal_layout->addWidget(robot);
  }
}

void TeamPanel::updateTeamLabel()
{
  std::string new_text = std::to_string(score) + " : " + team_name;
  team_label->setText(new_text.c_str());
}

}  // namespace qt_monitoring
