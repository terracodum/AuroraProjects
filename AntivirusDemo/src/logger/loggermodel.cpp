// SPDX-FileCopyrightText: 2024 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include "loggermodel.h"
#include "logger.h"

#include <QDebug>

#include <auroraapp/auroraapp.h>

namespace {
auto logFilePath()
{
    return QString("%1/%2").arg(Aurora::Application::filesDir(false).path(), "av.log");
}
} // namespace

LoggerModel::LoggerModel(QObject *parent) : QObject(parent)
{
    readFromLogFile();
    connect(&Logger::instance(), &Logger::readyData, this, &LoggerModel::onReadyData);
}

QString LoggerModel::logData() const
{
    return m_logData;
}

void LoggerModel::readFromLogFile()
{
    QFile file(logFilePath());
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "File " << logFilePath() << "is not open!";
    }

    QTextStream in(&file);
    m_logData = in.readAll();
    file.close();
}

void LoggerModel::onReadyData()
{
    readFromLogFile();
    emit logDataChanged();
}
