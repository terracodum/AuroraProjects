// SPDX-FileCopyrightText: 2024 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include "logdata.h"
#include <QStringList>
#include <QDateTime>
#include <chrono>

namespace {
const auto formatDateTimeFileRecord = QStringLiteral("yyyy-MM-dd HH:mm:ss");

auto getTimeFromMicroseconds(quint64 microseconds)
{
    QDateTime date;
    date.setTime_t(microseconds / 1000000);
    return date.toString(formatDateTimeFileRecord);
}
} // namespace

LogData::LogData(quint32 type, quint64 usec, const QString &message, const QString &eventString)
    : m_type(type), m_usec(usec), m_message(message), m_eventString(eventString)
{
}

QString LogData::serialize() const
{
    QStringList tmp;
    tmp.append(QString::number(m_type));
    tmp.append(m_eventString);
    tmp.append(getTimeFromMicroseconds(m_usec));
    tmp.append(m_message);
    return tmp.join("; ");
}
