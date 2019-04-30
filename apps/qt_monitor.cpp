#include <qt_monitoring/main_window.h>

#include <hl_monitoring/utils.h>

#include <tclap/CmdLine.h>

#include <QApplication>

#include <locale>

using namespace hl_monitoring;
using namespace qt_monitoring;

int main(int argc, char* argv[])
{
  TCLAP::CmdLine cmd("qt_monitor", ' ', "0.1");
  TCLAP::ValueArg<std::string> manager("m", "manager", "Manager", false, "manager.json", "manager", cmd);
  cmd.parse(argc, argv);

  QApplication app(argc, argv);
  std::setlocale(LC_ALL, "C");  // Qt tend to change the locale which causes issues for parsing numbers

  MainWindow window(manager.getValue());
  window.show();

  return app.exec();
}
