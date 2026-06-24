#include <QtQuick>
#include <QQmlContext>
#include <auroraapp.h>
#include "notesmodel.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> application(Aurora::Application::application(argc, argv));
    application->setOrganizationName(QStringLiteral("ru.template"));
    application->setApplicationName(QStringLiteral("Notes"));

    QScopedPointer<QQuickView> view(Aurora::Application::createView());

    NotesModel notesModel;
    view->rootContext()->setContextProperty(QStringLiteral("notesModel"), &notesModel);

    view->setSource(Aurora::Application::pathTo(QStringLiteral("qml/Notes.qml")));
    view->show();

    return application->exec();
}
