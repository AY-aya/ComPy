#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "controller.h"   // expose controller to QML

int main(int argc, char *argv[])
{

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    app.setWindowIcon(QIcon(":/Python_Compiler/assets/logo.png"));

    Controller *controller = new Controller(&app);
    engine.rootContext()->setContextProperty("controller", controller);

    const QUrl url(QStringLiteral("qrc:/Python_Compiler/main.qml"));
    engine.load(url);

    return app.exec();



}


