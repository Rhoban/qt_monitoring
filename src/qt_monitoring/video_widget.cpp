#include <qt_monitoring/video_widget.h>

#include <hl_communication/utils.h>
#include <hl_communication/game_controller_utils.h>
#include <qt_monitoring/utils.h>

#include <opencv2/imgproc.hpp>
#include <QTimer>

using namespace hl_communication;
using namespace hl_monitoring;

namespace qt_monitoring
{
class VideoGridLayout : public QGridLayout
{
public:
  QSize minimumSize() const override
  {
    return QSize(1100, 950);
  }
  QSize maximumSize() const override
  {
    return QSize(1100, 950);
  }
};

VideoWidget::VideoWidget(std::unique_ptr<hl_monitoring::MonitoringManager> manager_, QTimer* timer_)
  : manager(std::move(manager_)), now(0), memory_duration(5 * 1000 * 1000), old_slider_value(0), timer(timer_)
{
  layout = new VideoGridLayout;
  stream_selector = new StreamSelector();

  addSource("TopView");
  if (!manager->isLive())
  {
    slider_value_label = new QLabel(this);
    slider_value_label->setText("0");
    slider = new QSlider(Qt::Horizontal, this);
    buttonPause = new QPushButton("PLAY");
    buttonFastForward = new QPushButton("x2");
    connect(buttonPause, SIGNAL(released()), this, SLOT(clickPause()));
    connect(buttonFastForward, SIGNAL(released()), this, SLOT(clickFastForward()));
  }

  playing = manager->isLive();
  speed_ratio = 1;
  updateAvailableSources(manager->getImageProvidersNames());
  setLayout(layout);
}

VideoWidget::~VideoWidget()
{
}

void VideoWidget::update()
{
  updateLayout();
  updateTimeBoundaries();
  updateTime();
  updateManager();
  updateContent(manager->getCalibratedImages(now), manager->getField(), status);
}

void VideoWidget::updateFocus(int team_focus, int player_focus)
{
  team_drawer.setTeamFocus(team_focus);
  team_drawer.setPlayerFocus(player_focus);
}

void VideoWidget::updateContent(const std::map<std::string, CalibratedImage>& images, const Field& field,
                                const hl_communication::MessageManager::Status& status)
{
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
    TopViewDrawer::GoalsDisposition goals_disposition = TopViewDrawer::GoalsDisposition::GoalsNeutral;
    auto& gc_msg = status.gc_message;
    if (gc_msg.teams_size() > 0)
    {
      auto& firstTeam = gc_msg.teams().Get(0);
      if (firstTeam.has_team_color())
      {
        goals_disposition = (firstTeam.team_color() == hl_communication::getBlueTeamColor()) ?
                                TopViewDrawer::GoalsDisposition::GoalsBlueLeft :
                                TopViewDrawer::GoalsDisposition::GoalsBlueRight;
      }
    }
    top_view_drawer.setGoalsDisposition(goals_disposition);

    cv::Mat top_view_img = cv::Mat(top_view_drawer.getImg(field));
    team_drawer.drawTopView(field, top_view_drawer, status, &top_view_img);
    labels["TopView"].img = top_view_img;
  }
  for (auto& entry : labels)
  {
    if (!entry.second.img.empty())
    {
      cv::cvtColor(entry.second.img, entry.second.img, cv::COLOR_BGR2RGB);
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

const hl_monitoring::MonitoringManager& VideoWidget::getManager() const
{
  return *manager;
}

const hl_communication::MessageManager::Status& VideoWidget::getStatus() const
{
  return status;
}

CalibratedImage VideoWidget::getCalibratedImage(const std::string& provider_name, uint64_t time_stamp)
{
  return manager->getCalibratedImage(provider_name, time_stamp);
}

uint64_t VideoWidget::getTS() const
{
  return now;
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
      layout->addWidget(entry.second.label, row, col, 1, 1, Qt::AlignCenter);
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
  if (!manager->isLive())
  {
    layout->addWidget(slider, row++, 0, 1, nb_cols);
    layout->addWidget(slider_value_label, row++, 0, 1, 1);
    layout->addWidget(buttonPause, row++, 0, 1, 1);
    layout->addWidget(buttonFastForward, row++, 0, 1, 1);
  }
}

void VideoWidget::updateTimeBoundaries()
{
  initial_time = manager->getMessageManager().getStart();
  end_time = manager->getMessageManager().getEnd();

  for (const std::string& source_name : manager->getImageProvidersNames())
  {
    if (source_name == "TopView")
      continue;
    initial_time = std::min(initial_time, manager->getImageProvider(source_name).getStart());
    end_time = std::max(end_time, manager->getImageProvider(source_name).getEnd());
  }

  if (!manager->isLive())
  {
    // Slider has 1 sec step, time_stamps are micro_seconds
    slider->setRange(0, (end_time - initial_time) / std::pow(10, 6));
  }

  now = std::max(std::min(now, end_time), initial_time);
}

void VideoWidget::updateTime()
{
  // If slider has moved: update 'now' based on it
  if (!manager->isLive() && slider->value() != old_slider_value)
  {
    now = initial_time + 1000 * 1000 * slider->value();
  }

  if (playing)
  {
    if (manager->isLive())
    {
      // Dirty hack: since live image_providers are not supporting request of frames in the past, always require a frame
      // from the future
      double anticipation_ms = 500;
      now = getUTCTimeStamp() + anticipation_ms * 1000;
    }
    else
    {
      now += timer->interval() * 1000 * speed_ratio;
    }
  }
  if (!manager->isLive())
  {
    old_slider_value = (now - initial_time) / (1000 * 1000);
    slider->setValue(old_slider_value);
  }
  char str[30];
  if (!manager->isLive())
  {
    if (now >= end_time && end_time != 0)
    {
      now = end_time;
      sprintf(str, "end of video");
    }
    else
    {
      uint64_t elapsed_since_start = now - initial_time;
      uint64_t elapsed_ms = elapsed_since_start / 1000;
      uint64_t elapsed_seconds = elapsed_ms / 1000;
      uint64_t elapsed_minutes = elapsed_seconds / 60;
      elapsed_ms = elapsed_ms % 1000;
      elapsed_seconds = elapsed_seconds % 60;

      sprintf(str, "%.02lu:%.02lu:%.03lu\n", elapsed_minutes, elapsed_seconds, elapsed_ms);
    }
    slider_value_label->setText(str);
  }
}

void VideoWidget::setPose(const std::string& provider_name, int frame_idx, const Pose3D& pose)
{
  manager->setPose(provider_name, frame_idx, pose);
}

void VideoWidget::updateManager()
{
  manager->update();
  status = manager->getMessageManager().getStatus(now, memory_duration);
}

void VideoWidget::clickPause()
{
  playing = !playing;
  if (playing)
  {
    this->buttonPause->setText("PAUSE");
  }
  else
  {
    this->buttonPause->setText("PLAY");
  }
}

void VideoWidget::clickFastForward()
{
  speed_ratio *= 2;
  // Maximal Speed
  int max_speed = 16;
  if (speed_ratio > max_speed)
  {
    speed_ratio = 1;
  }
  int next_speed = speed_ratio * 2;
  if (next_speed > max_speed)
  {
    next_speed = 1;
  }
  this->buttonFastForward->setText(("x" + std::to_string(speed_ratio) + " -> x" + std::to_string(next_speed)).c_str());
}

}  // namespace qt_monitoring
