#include "team_panel.h"

#include <qt_monitoring/globals.h>
#include <QStyle>
#include <QStyleOption>
#include <QStylePainter>

#include <iostream>

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
  internal_layout = new QGridLayout();
  internal_layout->setAlignment(Qt::AlignCenter | Qt::AlignTop);
  team_label = new QLabel();
  team_label->setFont(QFont("Arial", 18, 3, false));
  team_label->setStyleSheet("font-weight: bold;");
  team_label->setAlignment(Qt::AlignCenter | Qt::AlignTop);
  text_log_label = new QLabel();
  text_log_label->setFont(QFont("Arial", 10, 3, false));
  text_log_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  robots_by_status = { { RobotStatus::Active, new PlayerGroup("Active players") },
                       { RobotStatus::Penalized, new PlayerGroup("Penalized players") },
                       { RobotStatus::Substitute, new PlayerGroup("Substitutes") } };

  int idx = 0;
  internal_layout->addWidget(team_label, idx++, 0, 1, 1);
  for (auto& entry : robots_by_status)
  {
    entry.second->setStyleSheet("font-weight:bold");
    internal_layout->addWidget(entry.second, idx++, 0, 1, 1);
  }
  internal_layout->addWidget(text_log_label, 0, 1, idx, 1);
  text_log_label->hide();

  updateTeamLabel();

  this->setLayout(internal_layout);
}

void TeamPanel::update(uint64_t time_stamp, int player_focus)
{
  if (player_focus <= 0)
  {
    text_log_label->hide();
    return;
  }
  text_log_label->show();
  std::string new_text = "LOG for " + std::to_string(player_focus) + " at " + std::to_string(time_stamp);
  text_log_label->setText(new_text.c_str());
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
      status = getRobotStatus(team_msg.robots(player_id - 1));
    }
    else if (team_msg.robots_size() != 0)  // Message received from GameController
    {
      std::cerr << "Received message with invalid player id: " << player_id << "team_msg # robot"
                << team_msg.robots_size() << std::endl;
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
    for (auto& entry : robots_by_status)
    {
      entry.second->clearRobots();
    }
    updateTeamLabel();
  }
}

uint32_t TeamPanel::getTeamId() const
{
  return team_id;
}

void TeamPanel::updateTeamLabel()
{
  std::string team_name = Globals::team_manager.getTeamName(team_id);
  std::string new_text = std::to_string(score) + " : " + team_name;
  team_label->setText(new_text.c_str());
}

void TeamPanel::paintEvent(QPaintEvent* event)
{
  QStyleOption opt;
  opt.init(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

  QWidget::paintEvent(event);
}

}  // namespace qt_monitoring
