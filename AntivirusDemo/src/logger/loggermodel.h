// SPDX-FileCopyrightText: 2024 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef LOGGERMODEL_H
#define LOGGERMODEL_H

#include <QObject>

class LoggerModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString logData READ logData NOTIFY logDataChanged)
public:
    explicit LoggerModel(QObject *parent = nullptr);

    QString logData() const;

private:
    void readFromLogFile();

private slots:
    void onReadyData();

signals:
    void logDataChanged();

private:
    QString m_logData;
};

#endif // LOGGERMODEL
