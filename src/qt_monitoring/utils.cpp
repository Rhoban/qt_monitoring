#include <qt_monitoring/utils.h>

#include <opencv2/imgproc.hpp>

namespace qt_monitoring
{

QImage cvToQImage(const cv::Mat& mat)
{
  cv::Mat rgb(mat.rows, mat.cols, mat.type());
  cvtColor(mat, rgb, CV_BGR2RGB);
  return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
}

}
