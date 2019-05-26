#include <qt_monitoring/video_widget.h>

#include <hl_communication/utils.h>
#include <qt_monitoring/utils.h>

#include <opencv2/imgproc.hpp>

using namespace hl_monitoring;

namespace qt_monitoring
{
class VideoGridLayout : public QGridLayout
{
public:
  QSize minimumSize() const override
  {
    return QSize(1200, 800);
  }
  QSize maximumSize() const override
  {
    return QSize(1200, 800);
  }
};

VideoWidget::VideoWidget()
{
  layout = new VideoGridLayout;
  stream_selector = new StreamSelector();

  addSource("TopView");
  setLayout(layout);
}

VideoWidget::~VideoWidget()
{
}

void VideoWidget::updateFocus(int team_focus, int player_focus)
{
  team_drawer.setTeamFocus(team_focus);
  team_drawer.setPlayerFocus(player_focus);
}

void VideoWidget::updateContent(const std::map<std::string, CalibratedImage>& images, const Field& field,
                                const hl_communication::MessageManager::Status& status)
{
  updateLayout();
  for (const auto& entry : images)
  {
    const std::string& source_name = entry.first;
    const CalibratedImage& calibrated_img = entry.second;
    if (labels[source_name].displayed)
    {
      cv::Mat camera_img = calibrated_img.getImg().clone();
      if (calibrated_img.isFullySpecified())
      {
        const CameraMetaInformation& camera_information = calibrated_img.getCameraInformation();
        field.tagLines(camera_information, &camera_img, cv::Scalar(0, 0, 0), 1, 10);
        team_drawer.drawNatural(camera_information, status, &camera_img);
      }
      labels[source_name].img = camera_img;
    }
  }

  if (labels["TopView"].displayed)
  {
    cv::Mat top_view_img = cv::Mat(top_view_drawer.getImg(field));
    team_drawer.drawTopView(field, top_view_drawer, status, &top_view_img);
    labels["TopView"].img = top_view_img;
  }
  for (auto& entry : labels)
  {
    if (!entry.second.img.empty())
    {
      cv::cvtColor(entry.second.img, entry.second.img, CV_BGR2RGB);
      QPixmap camera_pixmap = QPixmap::fromImage(cvToQImage(entry.second.img));
      entry.second.label->setPixmap(camera_pixmap.scaled(img_size.width, img_size.height, Qt::KeepAspectRatio));
    }
  }
}

void VideoWidget::updateAvailableSources(const std::set<std::string>& stream_names)
{
  stream_selector->updateAvailableSources(stream_names);
  for (const std::string& stream_name : stream_names)
  {
    if (labels.count(stream_name) == 0)
    {
      addSource(stream_name);
    }
  }
}

const StreamSelector& VideoWidget::getStreamSelector() const
{
  return *stream_selector;
}

void VideoWidget::addSource(const std::string& source_name)
{
  QLabel* new_label = new QLabel();
  new_label->setAlignment(Qt::AlignCenter);
  new_label->setScaledContents(false);
  labels[source_name].label = new_label;
  labels[source_name].displayed = false;
}

void VideoWidget::updateLayout()
{
  // TODO: do the update only on changes
  int nb_videos = stream_selector->getActiveSources().size();
  int nb_cols = 1;
  int scale_down_ratio = 1;
  if (nb_videos > 1)
  {
    scale_down_ratio = 2;
  }
  if (nb_videos > 2)
  {
    nb_cols = 2;
  }
  if (nb_videos > 4)
  {
    throw std::logic_error(HL_DEBUG + "more than 4 videos is not supported now");
  }
  // Updating size
  img_size.width = layout->maximumSize().width() / scale_down_ratio;
  img_size.height = layout->maximumSize().height() / scale_down_ratio;
  top_view_drawer.setImgSize(img_size);
  // Stream selector takes the whole width of the widget, on first line
  layout->removeWidget(stream_selector);
  layout->addWidget(stream_selector, 0, 0, 1, nb_cols);
  for (auto& entry : labels)
  {
    if (entry.second.displayed)
    {
      layout->removeWidget(entry.second.label);
      entry.second.displayed = false;
      entry.second.label->setVisible(false);
    }
    entry.second.label->setFixedWidth(img_size.width);
  }
  int row = 1;
  int col = 0;
  for (auto& entry : labels)
  {
    if (stream_selector->isActive(entry.first))
    {
      layout->addWidget(entry.second.label, row, col, 1, 1);
      entry.second.displayed = true;
      entry.second.label->setVisible(true);
      col++;
      if (col >= nb_cols)
      {
        row++;
        col = 0;
      }
    }
  }
}

}  // namespace qt_monitoring
