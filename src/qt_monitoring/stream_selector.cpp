#include <qt_monitoring/stream_selector.h>

#include <hl_communication/utils.h>

namespace qt_monitoring
{
StreamSelector::StreamSelector()
{
  layout = new QHBoxLayout();

  addCheckbox("TopView");
  this->setLayout(layout);
}

StreamSelector::~StreamSelector()
{
}

void StreamSelector::updateAvailableSources(const std::set<std::string>& stream_names)
{
  for (const std::string& stream_name : stream_names)
  {
    if (streams.count(stream_name) == 0)
    {
      addCheckbox(stream_name, false);
    }
  }
}

std::set<std::string> StreamSelector::getActiveSources() const
{
  std::set<std::string> result;
  for (const auto& entry : streams)
  {
    if (isActive(entry.first))
    {
      result.insert(entry.first);
    }
  }
  return result;
}

bool StreamSelector::isActive(const std::string& stream_name) const
{
  return streams.at(stream_name)->isChecked();
}

void StreamSelector::addCheckbox(const std::string& name, bool enabled)
{
  if (streams.count(name) > 0)
    throw std::logic_error(HL_DEBUG + "duplicated stream name: " + name);
  streams[name] = new QCheckBox(name.c_str());
  streams[name]->setChecked(enabled);
  layout->addWidget(streams[name]);
}

}  // namespace qt_monitoring
