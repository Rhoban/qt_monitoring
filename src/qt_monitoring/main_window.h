#pragma once

#include <qt_monitoring/pov_manager.h>
#include <qt_monitoring/team_panel.h>
#include <qt_monitoring/video_widget.h>

#include <hl_communication/message_manager.h>
#include <hl_monitoring/monitoring_manager.h>

#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QTimer>

#include <memory>

namespace qt_monitoring
{
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(std::unique_ptr<hl_monitoring::MonitoringManager> manager);

public slots:
  /**
   * Update global properties based on chosen active_source (initial_time, end_time)
   */
  void updateSource();
  /**
   * Update of slider + time properties
   */
  void updateTime();
  void updateManager();
  void updateTeams();
  void updatePOV();
  void update();
  void clickPause();
  void clickFastForward();

private:
  hl_communication::MessageManager::Status status;
  std::unique_ptr<hl_monitoring::MonitoringManager> manager;

  VideoWidget* video_widget;

  QTimer* timer;
  /**
   * Current timestamp (micro-seconds)
   */
  uint64_t now;
  /**
   * Sequence start (micro-seconds)
   */
  uint64_t initial_time;
  /**
   * Sequence end (micro-seconds)
   */
  uint64_t end_time;
  /**
   * Time between two ticks (micro-seconds)
   */
  uint64_t dt;

  /**
   * Time to conserve messages from the robots (micro_seconds)
   */
  uint64_t memory_duration;

  QWidget* zoneCentral;
  QGridLayout* layout;

  QPushButton* buttonAnnotationChoice;
  QPushButton* buttonPause;
  QPushButton* buttonFastForward;

  QSlider* slider;
  QLabel* slider_value_label;
  int old_slider_value;

  bool playing;
  int speed_ratio;

  POVManager* pov_manager;
  std::vector<TeamPanel*> teams;

protected:
  void resizeEvent(QResizeEvent* event) override;
};

}  // namespace qt_monitoring
