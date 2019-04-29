#pragma once

#include <qt_monitoring/player_group.h>

namespace qt_monitoring
{
class TeamPanel : public QWidget
{
  Q_OBJECT
public:
  /**
   * In teams panels, players are separated in three categories:
   * Active, penalized and substitutes
   */
  enum RobotStatus : int
  {
    Active = 0,
    Penalized = 1,
    Substitute = 2
  };

  TeamPanel();

  void treatMessages(const hl_communication::GCTeamMsg& team,
                     const std::vector<hl_communication::RobotMsg>& robots_msg);

  void updateTeamData(uint32_t new_team_id, int new_score);

private:
  void updateTeamLabel();

  uint32_t team_id;

  int score;

  QLabel* team_label;

  QVBoxLayout* internal_layout;

  std::map<RobotStatus, PlayerGroup*> robots_by_status;
};

}  // namespace qt_monitoring
