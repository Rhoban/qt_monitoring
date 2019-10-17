#include "calib_tool.h"

#include <QStylePainter>
#include <qt_monitoring/utils.h>

#include <qt_monitoring/video_widget.h>
#include <qt_monitoring/calib_tool.h>
#include <hl_communication/utils.h>
#include <hl_communication/game_controller_utils.h>

#include <opencv2/imgproc.hpp>
#include <iostream>
namespace qt_monitoring
{
CalibrationTool::CalibrationTool()
{
  std::cout << "ok push" << std::endl;

  QVBoxLayout* layoutGeneralBox = new QVBoxLayout;

  QGroupBox* generalBox = new QGroupBox;
  generalBox->setTitle("Calibration");

  QGridLayout* layout = new QGridLayout;

  QLabel* text_label = new QLabel();
  QLabel* text_label_general = new QLabel();

  canCalibrate = false;

  std::cout << "nnb source " << active_source.size() << std::endl;

  if (active_source.size() > 1)
  {
    text_label->setText("Please choose only 1 view");
  }
  else if (active_source.size() == 0)
  {
    text_label->setText("Please choose 1 view");
  }

  else
  {
    auto it = active_source.begin();
    std::string source = "Start Calibration for " + *it;

    text_label->setText(QString::fromStdString(source));
    canCalibrate = true;
  }

  text_label_general->setText("ok");
  layoutGeneralBox->addWidget(text_label_general);

  QDialogButtonBox* buttonDialog = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

  connect(buttonDialog, SIGNAL(accepted()), this, SLOT(startCalibration()));
  connect(buttonDialog, SIGNAL(rejected()), this, SLOT(reject()));

  generalBox->setLayout(layoutGeneralBox);

  layout->addWidget(generalBox, 0, 0, 1, 1);
  layout->addWidget(text_label, 0, 1, 1, 1);
  layout->addWidget(buttonDialog, 0, 2, 1, 1);

  this->setLayout(layout);

  std::cout << "finished" << std::endl;
}

CalibrationTool::~CalibrationTool()
{
}

void CalibrationTool::startCalibration()
{
  std::cout << "start calib" << std::endl;
}

void CalibrationTool::setActiveSource(std::set<std::string> new_source)
{
  active_source = new_source;
  std::cout << "new = " << new_source.size() << std::endl;

  if (active_source.size() > 1)
  {
    text_label->setText("Please choose only 1 view");
  }
  else if (active_source.size() == 0)
  {
    text_label->setText("Please choose 1 view");
  }

  else
  {
    auto it = active_source.begin();
    std::string source = "Start Calibration for " + *it;

    text_label->setText(QString::fromStdString(source));
    canCalibrate = true;
  }
}

}  // namespace qt_monitoring
