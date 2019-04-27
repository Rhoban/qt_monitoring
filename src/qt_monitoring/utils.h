#pragma once

#include <opencv2/core.hpp>
#include <QImage>
#include <QLabel>

namespace qt_monitoring
{

QImage cvToQImage(const cv::Mat& mat);

/**
 * Change style of the label depending on if its critical or not
 */
void updateStyle(QLabel* label, bool critical);

}
