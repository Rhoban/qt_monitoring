#pragma once

#include <hl_communication/wrapper.pb.h>

#include <QLabel>
#include <QLayout>
#include <QWidget>

namespace qt_monitoring
{
class PlayerWidget : public QWidget
{
  Q_OBJECT
public:
  PlayerWidget();

  void treatMessage(const hl_communication::RobotMsg& robot_msg);

private:
  int player_id;

  std::string robot_name;

  QVBoxLayout* layout;

  QLabel* robot_label;
  QLabel* referee_label;
  QLabel* robocup_label;
  QLabel* playing_label;
  QLabel* search_label;
  QLabel* hardware_label;
};

}  // namespace qt_monitoring
