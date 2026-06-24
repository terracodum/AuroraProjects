// SPDX-FileCopyrightText: 2024 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include "avclient.h"

#include <errno.h>
#include <unistd.h>

#include <QDebug>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QUrl>

#include <nemonotifications-qt5/notification.h>

static auto ServerService = QStringLiteral("ru.auroraos.AntivirusDemo");
static auto ServerObject = QStringLiteral("/");
static auto ServerInterface = QStringLiteral("ru.auroraos.AntivirusDemo");

AVClient::AVClient(QObject *parent)
    : QObject(parent),
      m_interface(new QDBusInterface(ServerService, ServerObject, ServerInterface,
                                     QDBusConnection::sessionBus()))
{
    auto call = QDBusMessage::createMethodCall(ServerService, ServerObject, ServerInterface,
                                               "StartApplication");

    QDBusPendingCall async = QDBusConnection::sessionBus().asyncCall(call);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this,
            [=](QDBusPendingCallWatcher *watcher) {
                QDBusPendingReply<> reply = *watcher;
                if (reply.isError() || !reply.isValid()) {
                    qWarning() << reply.error().message();
                }
                watcher->deleteLater();
            });

    QDBusConnection::sessionBus().connect(QString(), QString(), ServerInterface, "ScanFinished",
                                          this, SLOT(onScanFinished(QString, bool)));
}

QString AVClient::filePath() const
{
    return m_filePath;
}

void AVClient::setFilePath(const QString &filePath)
{
    if (filePath == m_filePath)
        return;
    const QUrl url(filePath);
    if (!(url.isValid() || url.isLocalFile())) {
        qWarning() << "Url is not valid and not local file!";
        return;
    }
    m_filePath = url.toLocalFile();
    emit filePathChanged();
}

void AVClient::publish(const QString &msg)
{
    Notification *notification = new Notification(this);

    notification->setAppName("ru.auroraos.AntivirusDemo");
    notification->setBody(msg);
    notification->setUrgency(Notification::Normal);
    notification->setIsTransient(false);
    notification->setItemCount(1);
    notification->setExpireTimeout(10000);

    connect(notification, &Notification::closed, [=](uint reason) { notification->deleteLater(); });

    notification->publish();
}

void AVClient::scanStarted(const QString &filePath)
{
    auto call = QDBusMessage::createMethodCall(ServerService, ServerObject, ServerInterface,
                                               "ScanStarted");

    QVariantList args = { QVariant::fromValue<QString>(filePath) };
    call.setArguments(args);

    QDBusPendingCall async = QDBusConnection::sessionBus().asyncCall(call);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this,
            [=](QDBusPendingCallWatcher *watcher) {
                QDBusPendingReply<> reply = *watcher;
                if (reply.isError() || !reply.isValid()) {
                    qWarning() << reply.error().message();
                }
                watcher->deleteLater();
            });
}

void AVClient::onScanFinished(const QString &filePath, bool isThreated)
{
    emit scanFileResult(filePath, isThreated);
}
