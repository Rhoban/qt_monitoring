#pragma once

#include <qt_monitoring/pov_manager.h>
#include <qt_monitoring/team_panel.h>
#include <qt_monitoring/video_widget.h>

#include <hl_communication/message_manager.h>

#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QTimer>

#include <memory>

namespace qt_monitoring
{
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(std::unique_ptr<hl_monitoring::MonitoringManager> manager);

  const hl_communication::MessageManager::Status& getStatus() const;

public slots:
  void update();
  void updateTeams();
  void updatePOV();

private:
  VideoWidget* video_widget;

  /**
   * Timer ticking updates of the main_window
   */
  QTimer* timer;

  QWidget* zoneCentral;
  QGridLayout* layout;

  QPushButton* buttonAnnotationChoice;

  POVManager* pov_manager;
  std::vector<TeamPanel*> teams;

protected:
  void resizeEvent(QResizeEvent* event) override;
};

}  // namespace qt_monitoring
