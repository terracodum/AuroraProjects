// SPDX-FileCopyrightText: 2024 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include "logger.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QTextStream>

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <auroraapp/auroraapp.h>
#include <libomplog.h>

#include "logdata.h"

namespace {
auto appDirPath()
{
    return Aurora::Application::filesDir(false).path();
}

const auto systemLogService = QStringLiteral("ru.omprussia.SystemLogs");
const auto systemLogPath = QStringLiteral("/ru/omprussia/SystemLogs");
const auto systemLogInterface = QStringLiteral("ru.omprussia.SystemLogs");

const auto sigNewEventFiltered = QStringLiteral("newEventFiltered");
const auto AVEventFilter = QStringLiteral("type=52|64,!62;");
} // namespace

static void msgOutErr(QtMsgType type, const QMessageLogContext &context, const QString &msg);

Logger &Logger::instance()
{
    static Logger logger;
    return logger;
}

Logger::Logger(QObject *parent)
    : QObject(parent),
      m_logFile(new QFile()),
      m_ifaceSystemLog(new QDBusInterface(systemLogService, systemLogPath, systemLogInterface,
                                          QDBusConnection::systemBus()))
{
    if (m_ifaceSystemLog->connection().interface()->isServiceRegistered(systemLogService).value()) {
        applyFilter();
        m_ifaceSystemLog->connection().connect(
                systemLogService, systemLogPath, systemLogInterface, "newEventFiltered", this,
                SLOT(onNewEventFiltered(const QMap<QString, QVariant> &)));
    }
}

void Logger::init()
{
    const auto path = appDirPath();

    QDir dir;
    if (!dir.exists(path)) {
        if (!dir.mkpath(path)) {
            qWarning("Directory %s is not created!!!", path.toLocal8Bit().constData());
        }
    }
    dir.setPath(path);
    const auto filePath = QString("%1/%2").arg(dir.absolutePath(), "av.log");
    m_logFile->setFileName(filePath);
    if (m_logFile->open(QIODevice::WriteOnly)) {
        QTextStream out(m_logFile.data());
        m_logFile->close();
    }

    qInstallMessageHandler(msgOutErr);
}

void Logger::writeLog(const QString &msg)
{
    if (m_logFile->open(QIODevice::Append | QFile::Text)) {
        QTextStream out(m_logFile.data());
        out << msg;
        m_logFile->close();
    } else {
        qWarning("File %s is not open to append!", m_logFile->fileName().toLocal8Bit().constData());
    }
}

void Logger::sendEvent(quint32 type, const QString &message)
{
    const auto level = QVariant::fromValue(static_cast<uchar>(WARN_LEVEL));
    if (!m_ifaceSystemLog->connection().isConnected()) {
        qWarning() << Q_FUNC_INFO << ": Interface disconnected!";
        return;
    }
    const auto dbusMessage =
            m_ifaceSystemLog->call("sendEvent", QVariant(type), level, QVariant(message));
    if (dbusMessage.type() != QDBusMessage::ReplyMessage) {
        qWarning("%s: %s", dbusMessage.errorName().toUtf8().data(),
                 dbusMessage.errorMessage().toUtf8().data());
    }
}

void Logger::applyFilter()
{
    const auto message = m_ifaceSystemLog->call("applyFilter", AVEventFilter);
    if (message.type() != QDBusMessage::ReplyMessage) {
        qWarning("%s: %s", message.errorName().toUtf8().data(),
                 message.errorMessage().toUtf8().data());
    }
}

void Logger::onNewEventFiltered(const QMap<QString, QVariant> &event)
{
    LogData logData(event["type"].toUInt(), event["usec"].toULongLong(),
                    event["message"].toString(), event["event_string"].toString());
    qInfo() << "EVENT: " << logData.serialize();
}

void msgOutErr(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    const auto createLine = [&](const QString &msgType) {
        return QString("[%1]: %2\n").arg(msgType, msg);
    };

    QString line;
    switch (type) {
    case QtDebugMsg:
        line = createLine("DD");
        break;
    case QtInfoMsg:
        line = createLine("II");
        break;
    case QtWarningMsg:
        line = createLine("WW");
        break;
    case QtCriticalMsg:
        line = createLine("CC");
        break;
    case QtFatalMsg:
        abort();
    }

    if (msg.startsWith("EVENT"))
        Logger::instance().writeLog(line);
}
