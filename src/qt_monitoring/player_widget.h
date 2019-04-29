#pragma once

#include <hl_communication/wrapper.pb.h>

#include <QLabel>
#include <QLayout>
#include <QWidget>

#include <rhoban_team_play/extra_team_play.pb.h>

namespace qt_monitoring
{
class PlayerWidget : public QWidget
{
  Q_OBJECT
public:
  PlayerWidget();
  ~PlayerWidget();

  void treatMessage(const hl_communication::RobotMsg& robot_msg);

private:
  void updateRobotLabel(const hl_communication::RobotIdentifier& identifier);
  void updateBallLabel(const rhoban_team_play::PerceptionExtra& extra);
  void updateFieldLabel(const rhoban_team_play::PerceptionExtra& extra);

  int player_id;

  std::string robot_name;

  QVBoxLayout* layout;

  QLabel* ball_label;
  QLabel* field_label;
  QLabel* robot_label;
  QLabel* referee_label;
  QLabel* robocup_label;
  QLabel* playing_label;
  QLabel* search_label;
  QLabel* hardware_label;
};

}  // namespace qt_monitoring
