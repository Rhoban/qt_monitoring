#pragma once

#include <hl_communication/message_manager.h>
#include <hl_monitoring/monitoring_manager.h>

#include <QObject>
#include <QLabel>
#include <QDialogButtonBox>
#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QAbstractButton>

namespace qt_monitoring
{
class ToolsBar : public QWidget
{
  Q_OBJECT
public:
  ToolsBar(std::unique_ptr<hl_monitoring::MonitoringManager> manager);
  ~ToolsBar();

  /**
   * Update content of the team panel for given time_stamp (microseconds since epoch) and chosen player focus
   * player focus is player_id, starting from 1, value below 1 indicates that no player has the focus
   */
  void updateGCSource(uint64_t time_stamp, std::set<hl_communication::MessageManager::SourceIdentifier> new_source_id);

  void updateCameraSource(std::set<std::string> new_active_source);

  uint32_t getSizeSouceId() const;

public slots:

  void Calibrate();

private:
  QLabel* gc_source_label;

  QLabel* text_log_label;

  QGridLayout* internal_layout;

  QPushButton* buttonCalibration;

  bool calibration_init;

  void updateSourceIDLabel();

  std::unique_ptr<hl_monitoring::MonitoringManager> manager;

  std::set<std::string> active_source;

  std::set<hl_communication::MessageManager::SourceIdentifier> source_id;
};

}  // namespace qt_monitoring
