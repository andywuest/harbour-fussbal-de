#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <sailfishapp.h>

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

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));

    app->setOrganizationDomain(ORGANISATION);
    app->setOrganizationName(ORGANISATION); // needed for Sailjail
    app->setApplicationName(APP_NAME);

    QScopedPointer<QQuickView> view(SailfishApp::createView());

    QQmlContext *context = view.data()->rootContext();
    FussballBackend fussballBackend;
    context->setContextProperty("fussballBackend", &fussballBackend);

    view->setSource(SailfishApp::pathTo("qml/harbour-fullball-de.qml"));
    view->show();
    return app->exec();
}
