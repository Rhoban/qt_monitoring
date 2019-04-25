#include "player_widget.h"

#include <hl_communication/utils.h>
#include <rhoban_team_play/extra_team_play.pb.h>

using namespace rhoban_team_play;

namespace qt_monitoring
{
PlayerWidget::PlayerWidget() : player_id(0), robot_name("robot")
{
  layout = new QVBoxLayout();
  layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  robot_label = new QLabel();
  robot_label->setFont(QFont("Arial", 16, 3, false));
  referee_label = new QLabel("referee:");
  robocup_label = new QLabel("robocup:");
  playing_label = new QLabel("playing:");
  search_label = new QLabel("search:");
  hardware_label = new QLabel("hardware:");
  layout->addWidget(robot_label);
  layout->addWidget(referee_label);
  layout->addWidget(robocup_label);
  layout->addWidget(playing_label);
  layout->addWidget(search_label);
  layout->addWidget(hardware_label);

  this->setLayout(layout);
}

void PlayerWidget::treatMessage(const hl_communication::RobotMsg& robot_msg)
{
  std::string robot_text = "Player " + std::to_string(robot_msg.robot_id().robot_id());
  robot_label->setText(robot_text.c_str());
  if (!robot_msg.has_free_field())
  {
    throw std::logic_error(HL_DEBUG + "no free field (Rhoban standard)");
  }
  MiscExtra extra;
  extra.ParseFromString(robot_msg.free_field());
  // TODO:
  referee_label->setText(("referee: " + extra.referee()).c_str());
  robocup_label->setText(("robocup: " + extra.robocup()).c_str());
  playing_label->setText(("playing: " + extra.playing()).c_str());
  search_label->setText(("search: " + extra.search()).c_str());
  hardware_label->setText(("hardware: " + extra.hardware_warnings()).c_str());
}

}  // namespace qt_monitoring
