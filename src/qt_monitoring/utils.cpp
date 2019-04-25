#include <qt_monitoring/utils.h>

namespace qt_monitoring
{

QImage cvToQImage(const cv::Mat& mat)
{
  return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
}

}
