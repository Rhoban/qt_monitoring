#pragma once

#include <qt_monitoring/player_widget.h>

namespace qt_monitoring
{
class TeamPanel : public QWidget
{
  Q_OBJECT
public:
  TeamPanel();

  void treatMessages(const std::vector<hl_communication::RobotMsg>& robots_msg);

  void updateTeamData(const std::string& new_team_name, int new_score);

  void setNbActiveRobots(int nb_robots);

private:
  void updateTeamLabel();

  std::string team_name;

  int score;

  QLabel* team_label;

  QVBoxLayout* internal_layout;

  std::vector<PlayerWidget*> robots;
};

}  // namespace qt_monitoring
