#pragma once
#include <iostream>

#include <QObject>
#include <QLabel>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QPushButton>
#include <QGroupBox>

#include <set>

namespace qt_monitoring
{
class CalibrationTool : public QDialog
{
  Q_OBJECT
public:
  CalibrationTool();
  ~CalibrationTool();

  /**
   * Update content of the team panel for given time_stamp (microseconds since epoch) and chosen player focus
   * player focus is player_id, starting from 1, value below 1 indicates that no player has the focus
   */

  void setActiveSource(std::set<std::string> new_source);

public slots:
  void startCalibration();

private:
  QGroupBox* generalBox;

  QLabel* text_label;

  QGridLayout* layout;

  bool canCalibrate;

  std::set<std::string> active_source;
};

}  // namespace qt_monitoring
