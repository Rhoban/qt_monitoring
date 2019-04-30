#pragma once

#include <QComboBox>
#include <QLayout>
#include <QWidget>

namespace qt_monitoring
{
/**
 * Point of view manager: allow to choose between the different point of views available and to select a player or a
 * team which should have the focus.
 */
class POVManager : public QWidget
{
  Q_OBJECT
public:
  enum class PointOfView
  {
    global,
    team,
    player
  };
  POVManager();

  PointOfView getPOV() const;
  int getTeamIdx() const;
  /**
   * Player Id, starting from 1
   */
  int getPlayerId() const;

  /**
   * team_idx in {0,1}
   * team_id: the official team_id @see hl_monitoring::TeamManager
   */
  void setTeamId(int team_idx, int team_id);

public slots:
  void onPOVUpdate(const QString& text);
  void onTeamUpdate(int idx);
  void onPlayerUpdate(int idx);

private:
  /**
   * Update visibility of widgets in POVManager based on current pov_choice
   */
  void updateVisibility();

  QHBoxLayout* internal_layout;
  /**
   * Select the type of point of view
   */
  QComboBox* pov_choice;
  /**
   * Allows to choose the team with the focus
   */
  QComboBox* team_choice;
  /**
   * Allows to have choose the player with the focus
   */
  QComboBox* player_choice;

  PointOfView current_pov;

  int current_team_idx;

  int current_player_idx;

  static std::map<PointOfView, std::string> pov_to_str;
  static std::map<std::string, PointOfView> str_to_pov;
};

}  // namespace qt_monitoring
