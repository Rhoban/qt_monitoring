#pragma once

#include <QLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QWidget>

#include <set>

namespace qt_monitoring
{
class StreamSelector : public QWidget
{
  Q_OBJECT
public:
  StreamSelector();
  ~StreamSelector();

  /**
   * Only push new names to the list of available names, do not remove existing elements
   */
  void updateAvailableSources(const std::set<std::string>& stream_names);

  std::set<std::string> getActiveSources() const;

  bool isActive(const std::string& stream_name) const;

  // public slots:
  //  void toggle(bool checked);

private:
  void addCheckbox(const std::string& name, bool enabled = true);

  /**
   * Global layout of the widget
   */
  QHBoxLayout* layout;
  /**
   * Buttons used to control visibility of video streams
   */
  std::map<std::string, QCheckBox*> streams;
};

}  // namespace qt_monitoring
