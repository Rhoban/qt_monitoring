#include <qt_monitoring/utils.h>

namespace qt_monitoring
{
QImage cvToQImage(const cv::Mat& mat)
{
  return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
}

void updateStyle(QLabel* label, bool critical)
{
  if (critical)
  {
    label->setStyleSheet("QLabel { font-weight: bold; color : red; }");
  }
  else
  {
    label->setStyleSheet("QLabel {  }");
  }
}

}  // namespace qt_monitoring
