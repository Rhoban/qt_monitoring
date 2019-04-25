#pragma once

#include <qt_monitoring/team_panel.h>

#include <hl_communication/message_manager.h>
#include <hl_monitoring/monitoring_manager.h>
#include <hl_monitoring/top_view_drawer.h>
#include <hl_monitoring/drawers/team_drawer.h>

#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QTimer>

namespace qt_monitoring
{

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();

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
  void updateAnnotations();
  void update();
  void clickPause();
  void clickFastForward();

private :
  hl_communication::MessageManager::Status status;
  hl_monitoring::MonitoringManager manager;
  hl_monitoring::TopViewDrawer top_view_drawer;
  hl_monitoring::Field field;
  hl_monitoring::TeamDrawer team_drawer;

  /**
   * Name of the active source
   */
  std::string active_source;

  QLabel * label_video;
  QLabel * label_top_view;
  cv::Mat * camera_img;
  cv::Mat * top_view_img;

  QTimer * timer;
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

  QWidget * zoneCentral;
  QGridLayout * layout;

  QPushButton * buttonAnnotationChoice;
  QPushButton * buttonPause;
  QPushButton * buttonFastForward;

  QSlider * slider;
  QLabel * slider_value_label;
  int old_slider_value;

  bool playing;
  int speed_ratio;

  std::vector<TeamPanel *> teams;

protected:
  void resizeEvent(QResizeEvent *event) override;

};

}
