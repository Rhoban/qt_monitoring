#pragma once

#include <qt_monitoring/player_widget.h>

namespace qt_monitoring
{
/**
 * A widget containing a number of players
 */
class PlayerGroup : public QWidget
{
  Q_OBJECT
public:
  PlayerGroup(const std::string& group_name);

  void treatMessages(const std::vector<hl_communication::RobotMsg>& robots_msg);

  void clearRobots();
  size_t getNbRobots() const;
  void setNbRobots(int nb_robots);

private:
  QVBoxLayout* internal_layout;
  QLabel* header_label;
  std::vector<PlayerWidget*> players;
  int nb_players;
};
}  // namespace qt_monitoring
