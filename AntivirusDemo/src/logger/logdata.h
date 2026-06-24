// SPDX-FileCopyrightText: 2024 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef LOGDATA_H
#define LOGDATA_H

#include <QString>

class LogData
{
public:
    LogData(quint32 type, quint64 usec, const QString &message, const QString &eventString);

    QString serialize() const;

private:
    quint32 m_type;
    quint64 m_usec;
    QString m_message;
    QString m_eventString;
};

#endif // LOGDATA_H
