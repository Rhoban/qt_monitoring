#pragma once

#include <hl_monitoring/calibrated_image.h>
#include <hl_monitoring/field.h>
#include <hl_monitoring/top_view_drawer.h>
#include <hl_monitoring/drawers/team_drawer.h>
#include <qt_monitoring/stream_selector.h>

#include <QLabel>

namespace qt_monitoring
{
class VideoWidget : public QWidget
{
  Q_OBJECT
public:
  VideoWidget();
  ~VideoWidget();

  void updateFocus(int team_focus, int player_focus);
  void updateContent(const std::map<std::string, hl_monitoring::CalibratedImage>& images,
                     const hl_monitoring::Field& field, const hl_communication::MessageManager::Status& status);

  /**
   * Only push new names to the list of available names, do not remove existing elements
   */
  void updateAvailableSources(const std::set<std::string>& stream_names);

  const StreamSelector& getStreamSelector() const;

private:
  void addSource(const std::string& source_name);
  /**
   * Updates the positioning of internal widgets and visibility according to the current status of sources
   */
  void updateLayout();

  QGridLayout* layout;

  struct SourceEntry
  {
    QLabel* label;
    bool displayed;
    cv::Mat img;
  };

  std::map<std::string, SourceEntry> labels;

  StreamSelector* stream_selector;

  hl_monitoring::TopViewDrawer top_view_drawer;
  hl_monitoring::TeamDrawer team_drawer;

  /**
   * Size of video labels in px
   */
  cv::Size img_size;
};
}  // namespace qt_monitoring
