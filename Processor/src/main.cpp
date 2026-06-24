#include <QtQuick>
#include <QQmlContext>
#include <QQmlEngine>
#include <auroraapp.h>
#include "imageprocessor.h"
#include "resultimageprovider.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> application(Aurora::Application::application(argc, argv));
    application->setOrganizationName(QStringLiteral("ru.template"));
    application->setApplicationName(QStringLiteral("Processor"));

    QScopedPointer<QQuickView> view(Aurora::Application::createView());

    ResultImageProvider *provider = new ResultImageProvider;   // владение забирает движок QML
    view->engine()->addImageProvider(QStringLiteral("result"), provider);

    ImageProcessor processor;
    processor.setProvider(provider);
    view->rootContext()->setContextProperty(QStringLiteral("imageProcessor"), &processor);

    view->setSource(Aurora::Application::pathTo(QStringLiteral("qml/Processor.qml")));
    view->show();

    return application->exec();
}
