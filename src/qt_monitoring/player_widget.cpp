#include "player_widget.h"

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
  // TODO:
}

}  // namespace qt_monitoring
