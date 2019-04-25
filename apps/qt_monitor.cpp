#include <QApplication>
#include <qt_monitoring/main_window.h>

#include <locale>

using namespace qt_monitoring;

int main(int argc, char *argv[]){
  QApplication app(argc, argv);
  std::setlocale(LC_ALL, "C");//Qt tend to change the locale which causes issues for parsing numbers

  MainWindow window;
  window.show();
  
  return app.exec();
}
