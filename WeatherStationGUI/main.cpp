#include <QtGui/QGuiApplication>
#include <QtQuick>
#include "qtquick2applicationviewer.h"
#include "tcpsocketclient.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<TCPsocketClient>("TCPsocketClient", 1, 0, "TCPsocketClient");

    QtQuick2ApplicationViewer viewer;
    viewer.setMainQmlFile(QStringLiteral("qml/WeatherStationGUI/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
