#include "team_panel.h"

using namespace hl_communication;

namespace qt_monitoring
{

TeamPanel::TeamPanel() : team_name("team_name"), score(0)
{
  internal_layout = new QVBoxLayout();
  team_label = new QLabel();
  team_label->setFont(QFont("Arial",18,3,false));
  team_label->setAlignment(Qt::AlignCenter | Qt::AlignTop);
  internal_layout->addWidget(team_label);
  internal_layout->setAlignment(Qt::AlignCenter | Qt::AlignTop);
  
  updateTeamLabel();
  setNbActiveRobots(0);

  this->setLayout(internal_layout);
}

void TeamPanel::treatMessages(const std::vector<RobotMsg>& robots_msg)
{
  setNbActiveRobots(robots_msg.size());
}

void TeamPanel::updateTeamData(const std::string& new_team_name, int new_score)
{
  score = new_score;
  team_name = new_team_name;
  updateTeamLabel();
}
void TeamPanel::setNbActiveRobots(int nb_robots)
{
  for (size_t i = 0; i < robot_labels.size(); i++)
  {
    internal_layout->removeWidget(robot_labels[i]);
    delete(robot_labels[i]);
  }
  robot_labels.clear();
  for (int i = 0; i < nb_robots; i++)
  {
    QLabel * robot_label = new QLabel();
    robot_label->setFont(QFont("Arial",12,3,false));
    robot_label->setAlignment(Qt::AlignLeft);
    robot_label->setText(("Robot " + std::to_string(i+1)).c_str());
    robot_labels.push_back(robot_label);
    internal_layout->addWidget(robot_label);
  }
}

void TeamPanel::updateTeamLabel()
{
  std::string new_text = std::to_string(score) + " : " + team_name;
  team_label->setText(new_text.c_str());
}

}
