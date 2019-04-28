#include "player_group.h"

#include <iostream>

using namespace hl_communication;

namespace qt_monitoring
{
PlayerGroup::PlayerGroup(const std::string& group_name)
{
  internal_layout = new QVBoxLayout();
  header_label = new QLabel(group_name.c_str());
  header_label->setFont(QFont("Arial", 16, 3, false));
  internal_layout->addWidget(header_label);
  this->setLayout(internal_layout);
}

void PlayerGroup::treatMessages(const std::vector<RobotMsg>& robots_msg)
{
  setNbRobots(robots_msg.size());
  for (size_t idx = 0; idx < robots_msg.size(); idx++)
  {
    players[idx]->treatMessage(robots_msg[idx]);
  }
}

void PlayerGroup::clearRobots()
{
  // First remove widgets from label and delete objects
  for (size_t i = 0; i < players.size(); i++)
  {
    PlayerWidget* robot_widget = players[i];
    internal_layout->removeWidget(robot_widget);
    delete (robot_widget);
  }
  // Objects have already been deleted
  players.clear();
}

size_t PlayerGroup::getNbRobots() const
{
  return players.size();
}

void PlayerGroup::setNbRobots(int nb_robots)
{
  if ((size_t)nb_robots != players.size())
  {
    clearRobots();
    // Populating widgets again
    for (int i = 0; i < nb_robots; i++)
    {
      PlayerWidget* robot = new PlayerWidget();
      players.push_back(robot);
      internal_layout->addWidget(robot);
    }
  }
}

}  // namespace qt_monitoring
