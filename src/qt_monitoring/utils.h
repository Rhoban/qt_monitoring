#pragma once

#include <opencv2/core.hpp>
#include <QImage>

namespace qt_monitoring
{

QImage cvToQImage(const cv::Mat& mat);

}
