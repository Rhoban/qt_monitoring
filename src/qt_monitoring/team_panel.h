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

  /**
   * Update content of the team panel for given time_stamp (microseconds since epoch) and chosen player focus
   * player focus is player_id, starting from 1, value below 1 indicates that no player has the focus
   */
  void update(uint64_t time_stamp, int player_focus);

  void treatMessages(const hl_communication::GCTeamMsg& team,
                     const std::vector<hl_communication::RobotMsg>& robots_msg);

  void updateTeamData(uint32_t new_team_id, int new_score);

  uint32_t getTeamId() const;

  void paintEvent(QPaintEvent* event);

private:
  void updateTeamLabel();

  uint32_t team_id;

  int score;

  QLabel* team_label;

  QGridLayout* internal_layout;

  QLabel* text_log_label;

  std::map<RobotStatus, PlayerGroup*> robots_by_status;
};

}  // namespace qt_monitoring
