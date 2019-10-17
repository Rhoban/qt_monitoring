#pragma once

#include <hl_communication/camera.pb.h>
#include <hl_monitoring/calibrated_image.h>
#include <hl_monitoring/field.h>
#include <hl_monitoring/drawers/team_drawer.h>
#include <hl_monitoring/monitoring_manager.h>
#include <hl_monitoring/top_view_drawer.h>
#include <qt_monitoring/stream_selector.h>

#include <QLabel>
#include <QSlider>

namespace qt_monitoring
{
class VideoWidget : public QWidget
{
  Q_OBJECT
public:
  /**
   * If timer is not provided
   */
  VideoWidget(std::unique_ptr<hl_monitoring::MonitoringManager> manager, QTimer* timer);
  ~VideoWidget();

  void update();

  void updateFocus(int team_focus, int player_focus);
  void updateContent(const std::map<std::string, hl_monitoring::CalibratedImage>& images,
                     const hl_monitoring::Field& field, const hl_communication::MessageManager::Status& status);

  /**
   * Only push new names to the list of available names, do not remove existing elements
   */
  void updateAvailableSources(const std::set<std::string>& stream_names);

  const StreamSelector& getStreamSelector() const;
  const hl_monitoring::MonitoringManager& getManager() const;
  const hl_communication::MessageManager::Status& getStatus() const;
  hl_monitoring::CalibratedImage getCalibratedImage(const std::string& provider_name, uint64_t time_stamp);

  /**
   * Return time_stamp of the current frame
   */
  uint64_t getTS() const;

  void setPose(const std::string& provider_name, int frame_idx, const hl_communication::Pose3D& pose);

  std::set<hl_communication::MessageManager::SourceIdentifier> getInterferingGCSource();

  std::set<std::string> getActiveSources();

public slots:
  void clickPause();
  void clickFastForward();

private:
  void addSource(const std::string& source_name);
  /**
   * Updates the positioning of internal widgets and visibility according to the current status of sources
   */
  void updateLayout();

  /**
   * Update global properties based on chosen active_source (initial_time, end_time)
   */
  void updateTimeBoundaries();

  /**
   * Update of slider + time properties
   */
  void updateTime();

  /**
   * Ticks an update from the manager
   */
  void updateManager();

  std::unique_ptr<hl_monitoring::MonitoringManager> manager;
  hl_communication::MessageManager::Status status;

  /**
   * Current timestamp (micro-seconds)
   */
  uint64_t now;

  /**
   * Time to conserve messages from the robots (micro_seconds)
   */
  uint64_t memory_duration;

  /**
   * Sequence start (micro-seconds)
   */
  uint64_t initial_time;
  /**
   * Sequence end (micro-seconds)
   */
  uint64_t end_time;

  QGridLayout* layout;

  struct SourceEntry
  {
    QLabel* label;
    bool displayed;
    cv::Mat img;
  };

  std::map<std::string, SourceEntry> labels;

  StreamSelector* stream_selector;

  QPushButton* buttonPause;
  QPushButton* buttonFastForward;

  QSlider* slider;
  QLabel* slider_value_label;
  int old_slider_value;

  hl_monitoring::TopViewDrawer top_view_drawer;
  hl_monitoring::TeamDrawer team_drawer;

  bool playing;
  int speed_ratio;

  /**
   * Timer is used to have access to the interval
   */
  QTimer* timer;

  /**
   * Size of video labels in px
   */
  cv::Size img_size;
};

}  // namespace qt_monitoring
