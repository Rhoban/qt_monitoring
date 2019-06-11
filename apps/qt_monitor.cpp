#include <qt_monitoring/main_window.h>

#include <hl_communication/utils.h>

#include <tclap/CmdLine.h>

#include <QApplication>

#include <locale>

using namespace hl_monitoring;
using namespace qt_monitoring;

int main(int argc, char* argv[])
{
  TCLAP::CmdLine cmd("qt_monitor", ' ', "0.1");
  TCLAP::ValueArg<std::string> manager_arg("m", "manager", "Manager", false, "manager.json", "manager");
  TCLAP::SwitchArg default_live_arg("l", "live", "Default live mode");
  TCLAP::SwitchArg default_replay_arg("r", "replay", "Default replay mode");
  std::vector<TCLAP::Arg*> mandatory_args = { &manager_arg, &default_live_arg, &default_replay_arg };
  cmd.xorAdd(mandatory_args);
  cmd.parse(argc, argv);

  QApplication app(argc, argv);
  std::setlocale(LC_ALL, "C");  // Qt tend to change the locale which causes issues for parsing numbers

  std::unique_ptr<MonitoringManager> manager(new MonitoringManager());
  if (default_live_arg.getValue())
  {
    manager->autoLiveStart();
  }
  else if (default_replay_arg.getValue())
  {
    manager->loadConfig("replay.json");
  }
  else
  {
    manager->loadConfig(manager_arg.getValue());
  }
  MainWindow window(std::move(manager));
  window.show();

  return app.exec();
}
