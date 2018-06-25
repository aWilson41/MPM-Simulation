#include <QApplication>
#include <QLabel>
#include <QSurfaceFormat>

#include "MainWidget.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setSamples(4);
	format.setRenderableType(QSurfaceFormat::OpenGL);
	QSurfaceFormat::setDefaultFormat(format);
	app.setApplicationName("SpringMassSim");
	MainWidget widget;
	widget.show();
	return app.exec();
}