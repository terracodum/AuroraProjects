// SPDX-FileCopyrightText: 2024 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include "avserver.h"

#include "auroraapp/auroraapp.h"
#include <unistd.h>
#include <pwd.h>

#include <QDir>
#include <QDirIterator>
#include <QTextStream>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>

#include "../logger/logger.h"
#include <libomplog.h>

static const auto ServiceName = QStringLiteral("ru.auroraos.AntivirusDemo");

namespace {
quint32 createType(sdjd_event event)
{
    return static_cast<quint32>(event);
}
} // namespace

AVServer::AVServer(QObject *parent) : QObject(parent), m_scanner(new AVScanner(this))
{
    new AVServerAdaptor(this);

    if (!QDBusConnection::sessionBus().registerObject("/", this)) {
        qCritical() << QDBusConnection::sessionBus().lastError().message();
        return;
    }

    if (!QDBusConnection::sessionBus().registerService(ServiceName)) {
        qCritical() << QDBusConnection::sessionBus().lastError().message();
    }

    Logger::instance().sendEvent(createType(sdjd_event::AV_SERVICE_STARTED),
                                 QString("Service %1 started").arg(ServiceName));

    connect(m_scanner.data(), &AVScanner::fileResult, this, &AVServer::onFileResult,
            Qt::QueuedConnection);
}

AVServer::~AVServer()
{
    QDBusConnection::sessionBus().unregisterObject("/");
    QDBusConnection::sessionBus().unregisterService(ServiceName);
    Logger::instance().sendEvent(createType(sdjd_event::AV_SERVICE_STOPPED),
                                 QString("Service %1 stopped").arg(ServiceName));
}

void AVServer::startApplication()
{
    QDBusInterface iface(QStringLiteral("ru.omp.am"), QStringLiteral("/ru/omp/am"),
                         QStringLiteral("ru.omp.am.Identify"), QDBusConnection::sessionBus());

    const auto ret =
            iface.call(QDBus::AutoDetect, QStringLiteral("IdentifyByBusName"), message().service());

    QString binName, keyId;
    if (ret.type() != QDBusMessage::ReplyMessage) {
        qCritical() << "Authentication: Could not get" << message().service() << "credentials";
    } else {
        binName = ret.arguments().constFirst().toString();
        keyId = ret.arguments().constLast().toString();
    }

    iterateDir(binName, keyId);
}

void AVServer::scanStarted(const QString &filePath)
{
    const QFileInfo info(filePath);
    Logger::instance().sendEvent(createType(sdjd_event::AV_SCAN_STARTED),
                                 QString("%1 started.").arg(info.fileName()));
    m_scanner->scan(filePath);
}

void AVServer::iterateDir(const QString &binName, const QString &keyId)
{
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);

    QString dir = QStringLiteral("/home/%1/.local").arg(QString(pw->pw_name));
    QDirIterator it(dir, QDirIterator::Subdirectories);

    QStringList fileList;

    while (it.hasNext()) {
        QDir dir(it.next());
        dir.setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

        if (dir.isReadable())
            fileList << dir.path();
    }

    QFile file(dir + "/share/ru.auroraos/AntivirusDemo/antivirus.txt");

    if (file.exists())
        file.remove();

    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);

        stream << "[Directories]\n";
        for (const auto &item : fileList)
            stream << item << "\n";

        if (!binName.isEmpty() || !keyId.isEmpty()) {
            stream << "[Authentication]\n" << binName << "\n" << keyId << "\n";
        }

        stream << "[Shared]\n" << Aurora::Application::organizationFilesDir(true).path() << "\n";

        file.close();
    }
}

void AVServer::onFileResult(const QString &filePath, bool isThreated)
{
    const QFileInfo info(filePath);
    if (isThreated) {
        Logger::instance().sendEvent(createType(sdjd_event::AV_THREAT_DETECTED),
                                     QString("%1 is threat").arg(info.fileName()));
    }
    Logger::instance().sendEvent(createType(sdjd_event::AV_SCAN_FINISHED),
                                 QString("%1 is finished").arg(info.fileName()));
    emit ScanFinished(filePath, isThreated);
}

AVServerAdaptor::AVServerAdaptor(AVServer *parent) : QDBusAbstractAdaptor(parent), q(parent)
{
    setAutoRelaySignals(true);
}

void AVServerAdaptor::StartApplication()
{
    q->startApplication();
}

void AVServerAdaptor::ScanStarted(const QString &filePath)
{
    q->scanStarted(filePath);
}
