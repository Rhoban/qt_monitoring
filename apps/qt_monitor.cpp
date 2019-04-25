#include <QApplication>
#include <qt_monitoring/main_window.h>

using namespace qt_monitoring;

int main(int argc, char *argv[]){
  QApplication app(argc, argv);

  MainWindow window;
  window.show();
  
  return app.exec();
}
