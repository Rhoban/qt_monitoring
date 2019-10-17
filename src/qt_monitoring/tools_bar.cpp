#include "tools_bar.h"

#include <QStylePainter>
#include <qt_monitoring/video_widget.h>
#include <qt_monitoring/calib_tool.h>
#include <hl_communication/utils.h>
#include <hl_communication/game_controller_utils.h>
#include <hl_monitoring/replay_viewer.h>
#include <qt_monitoring/utils.h>

#include <opencv2/imgproc.hpp>
#include <iostream>
#include <QTimer>

using namespace hl_communication;
using namespace hl_monitoring;

namespace qt_monitoring
{
ToolsBar::ToolsBar(std::unique_ptr<hl_monitoring::MonitoringManager> manager_) : manager(std::move(manager_))
{
  internal_layout = new QGridLayout();
  internal_layout->setAlignment(Qt::AlignCenter | Qt::AlignTop);
  gc_source_label = new QLabel();
  gc_source_label->setFont(QFont("Arial", 8, 3, false));
  gc_source_label->setStyleSheet("font-weight: bold;");
  gc_source_label->setAlignment(Qt::AlignCenter | Qt::AlignTop);
  gc_source_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  text_log_label = new QLabel();

  internal_layout->addWidget(gc_source_label, 0, 1, 1, 1);

  buttonCalibration = new QPushButton("Calibration");

  connect(buttonCalibration, SIGNAL(released()), this, SLOT(Calibrate()));

  //  internal_layout->addWidget(buttonCalibration, 0, 0, 1, 1);

  updateSourceIDLabel();

  this->setLayout(internal_layout);

  // calibration_init = false;
}

ToolsBar::~ToolsBar()
{
}

void ToolsBar::updateGCSource(uint64_t time_stamp,
                              std::set<hl_communication::MessageManager::SourceIdentifier> new_source_id)
{
  source_id = new_source_id;
}

void ToolsBar::updateCameraSource(std::set<std::string> new_active_source)
{
  std::cout << "update with nb source : " << new_active_source.size() << std::endl;

  active_source = new_active_source;
  //  setActiveSource = active_source
}

uint32_t ToolsBar::getSizeSouceId() const
{
  return source_id.size();
}

void ToolsBar::Calibrate()
{
  CalibrationTool* msgBox = new CalibrationTool();
  std::cout << " source " << active_source.size() << std::endl;
  // msgBox->setActiveSource(active_source);

  msgBox->exec();

  // calibration_init = true;
}

void ToolsBar::updateSourceIDLabel()
{
  std::string text = "interfering GC ";

  for (auto it = source_id.begin(); it != source_id.end(); it++)
  {
    text = text + " ip : " + ipToString(it->src_ip) + " port : " + std::to_string(it->src_port) + "\n";
  }

  text_log_label->setText(text.c_str());
}

}  // namespace qt_monitoring
