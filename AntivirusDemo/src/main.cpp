// SPDX-FileCopyrightText: 2024 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include <auroraapp.h>

#include <glib.h>
#include <grp.h>

#include <glib.h>
#include <libomplog.h>
#include <memory>
#include <auroraapp.h>
#include <sys/types.h>
#include <unistd.h>

#include <QGuiApplication>
#include <QQuickView>
#include <QScopedPointer>
#include <QDebug>

#include "logger/logger.h"
#include "av/avclient.h"
#include "av/avserver.h"
#include "logger/loggermodel.h"

namespace {
bool hasGroup(const char *groupName)
{
    errno = 0;

    struct group *group = getgrnam(groupName);
    if (group == nullptr) {
        if (errno) {
            perror("getgrnam");
        } else {
            qWarning() << "No" << groupName << "group in system";
        }
        return false;
    }

    std::vector<gid_t> groups(128);
    int len = getgroups(groups.size(), groups.data());
    if (len == -1) {
        perror("getgroups");
        return false;
    }

    for (int i = 0; i < len; ++i) {
        if (groups[i] == group->gr_gid) {
            return true;
        }
    }

    return false;
}
} // namespace

int main(int argc, char *argv[])
{
    bool service = false;

    if (g_strv_contains(argv, "--service")) {
        Logger::instance().init();
        service = true;
    }

    if (hasGroup("die-hard")) {
        log_event(WARN_LEVEL, sdjd_event::AV_INFO,
                  service ? "Service has 'die-hard' supplementary group"
                          : "Application has 'die-hard' supplementary group");
    } else {
        log_event(WARN_LEVEL, sdjd_event::AV_ERROR,
                  service ? "Service has no 'die-hard' supplementary group"
                          : "Application has no 'die-hard' supplementary group");
    }

    if (service) {
        QScopedPointer<QCoreApplication> srv(new QCoreApplication(argc, argv));
        auto server = std::make_unique<AVServer>();
        return srv->exec();
    }

    QScopedPointer<QGuiApplication> app(Aurora::Application::application(argc, argv));
    QScopedPointer<QQuickView> view(Aurora::Application::createView());

    qmlRegisterType<AVClient>("ru.auroraos.AntivirusDemo", 1, 0, "AVClient");
    qmlRegisterType<LoggerModel>("ru.auroraos.AntivirusDemo", 1, 0, "LoggerModel");

    view->setSource(Aurora::Application::pathTo(QStringLiteral("qml/AntivirusDemo.qml")));
    view->show();

    return app->exec();
}
