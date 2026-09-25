#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <sailfishapp.h>
#include <QDir>
#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickView>
#include <QScopedPointer>
#include <QStandardPaths>
#include <QtQml>

#include "constants.h"
#include "fussballbackend.h"

int main(int argc, char *argv[])
{
    // SailfishApp::main() will display "qml/harbour-fussball-de.qml", if you need more
    // control over initialization, you can use:
    //
    //   - SailfishApp::application(int, char *[]) to get the QGuiApplication *
    //   - SailfishApp::createView() to get a new QQuickView * instance
    //   - SailfishApp::pathTo(QString) to get a QUrl to a resource file
    //   - SailfishApp::pathToMainQml() to get a QUrl to the main QML file
    //
    // To display the view, call "show()" (will show fullscreen on device).

    //return SailfishApp::main(argc, argv);

    // The new location of the LocalStorage database
    QDir fontDir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
                  + QString("/%1/%2/fonts/").arg(ORGANISATION, APP_NAME));
    QDir logoDir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
                  + QString("/%1/%2/logos/").arg(ORGANISATION, APP_NAME));

    if (!fontDir.exists()) {
        fontDir.mkpath(fontDir.path());
    }
    if (!logoDir.exists()) {
        logoDir.mkpath(logoDir.path());
    }

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));

    app->setOrganizationDomain(ORGANISATION);
    app->setOrganizationName(ORGANISATION); // needed for Sailjail
    app->setApplicationName(APP_NAME);

    QScopedPointer<QQuickView> view(SailfishApp::createView());

    QQmlContext *context = view.data()->rootContext();
    FussballBackend fussballBackend(fontDir.path());
    context->setContextProperty("fussballBackend", &fussballBackend);

    context->setContextProperty("applicationVersion", QString(VERSION_NUMBER));

    view->setSource(SailfishApp::pathTo("qml/harbour-fussball-de.qml"));
    view->show();
    return app->exec();
}
