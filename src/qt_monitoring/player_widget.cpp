#include "player_widget.h"

#include <qt_monitoring/globals.h>
#include <qt_monitoring/utils.h>

#include <hl_communication/utils.h>

#include <sstream>

using namespace rhoban_team_play;

namespace qt_monitoring
{
PlayerWidget::PlayerWidget() : player_id(0), robot_name("robot")
{
  layout = new QVBoxLayout();
  layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  robot_label = new QLabel();
  robot_label->setFont(QFont("Arial", 14, 3, false));

  QFont player_font("Arial", 9, 3, false);

  ball_label = new QLabel("Ball:");
  field_label = new QLabel("Field:");
  referee_label = new QLabel("referee:");
  robocup_label = new QLabel("robocup:");
  playing_label = new QLabel("playing:");
  search_label = new QLabel("search:");
  hardware_label = new QLabel("hardware:");
  layout->addWidget(robot_label);
  layout->addWidget(ball_label);
  layout->addWidget(field_label);
  layout->addWidget(referee_label);
  layout->addWidget(robocup_label);
  layout->addWidget(playing_label);
  layout->addWidget(search_label);
  layout->addWidget(hardware_label);
  ball_label->setFont(player_font);
  field_label->setFont(player_font);
  referee_label->setFont(player_font);
  robocup_label->setFont(player_font);
  playing_label->setFont(player_font);
  search_label->setFont(player_font);
  hardware_label->setFont(player_font);

  this->setLayout(layout);
}

PlayerWidget::~PlayerWidget()
{
}

void PlayerWidget::treatMessage(const hl_communication::RobotMsg& robot_msg)
{
  updateRobotLabel(robot_msg.robot_id());
  if (!robot_msg.has_free_field())
  {
    throw std::logic_error(HL_DEBUG + "no free field (Rhoban standard)");
  }
  if (!robot_msg.perception().has_free_field())
  {
    throw std::logic_error(HL_DEBUG + "no free field (Rhoban standard)");
  }
  PerceptionExtra perception_extra;
  perception_extra.ParseFromString(robot_msg.perception().free_field());
  updateBallLabel(perception_extra);
  updateFieldLabel(perception_extra);
  MiscExtra extra;
  extra.ParseFromString(robot_msg.free_field());
  referee_label->setText(("referee: " + extra.referee()).c_str());
  robocup_label->setText(("robocup: " + extra.robocup()).c_str());
  playing_label->setText(("playing: " + extra.playing()).c_str());
  search_label->setText(("search: " + extra.search()).c_str());
  hardware_label->setText(("hardware: " + extra.hardware_warnings()).c_str());
};

void PlayerWidget::updateRobotLabel(const hl_communication::RobotIdentifier& identifier)
{
  uint32_t team_id = identifier.team_id();
  uint32_t player_id = identifier.robot_id();
  std::string robot_text = "Player " + std::to_string(player_id);
  if (Globals::team_manager.hasTeam(team_id) && Globals::team_manager.getTeam(team_id).hasPlayer(player_id))
  {
    robot_text = std::to_string(player_id) + " - " + Globals::team_manager.getTeam(team_id).getName(player_id);
  }
  robot_label->setText(robot_text.c_str());
}

void PlayerWidget::updateBallLabel(const rhoban_team_play::PerceptionExtra& extra)
{
  ball_label->setText(("Ball: Q=" + std::to_string(extra.ball().quality())).c_str());
  updateStyle(ball_label, !extra.ball().valid());
}

void PlayerWidget::updateFieldLabel(const rhoban_team_play::PerceptionExtra& extra)
{
  std::ostringstream oss;
  oss << "Field: Q=" << extra.field().quality() << " C=" << extra.field().consistency();
  field_label->setText(oss.str().c_str());
  updateStyle(field_label, !extra.field().valid());
}

}  // namespace qt_monitoring
