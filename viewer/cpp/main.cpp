#include <QtWidgets>
#include <sgmainwindow.hpp>

int main(int argc,
	 char ** args)
{
  QApplication app(argc,args);

  SGMainWindow window;

  window.show();

  return app.exec();
}
