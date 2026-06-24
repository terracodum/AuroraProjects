// SPDX-FileCopyrightText: 2024 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QDBusInterface>
#include <QScopedPointer>

class Logger : public QObject
{
    Q_OBJECT
public:
    static Logger &instance();

    void init();

    void writeLog(const QString &msg);
    void sendEvent(quint32 type, const QString &message);

private:
    Q_DISABLE_COPY(Logger)
    explicit Logger(QObject *parent = nullptr);
    ~Logger() override = default;

    void applyFilter();

private slots:
    void onNewEventFiltered(const QMap<QString, QVariant> &event);

signals:
    void readyData();

private:
    QScopedPointer<QFile> m_logFile;
    QScopedPointer<QDBusInterface> m_ifaceSystemLog;
};

#endif // LOGGER_H
