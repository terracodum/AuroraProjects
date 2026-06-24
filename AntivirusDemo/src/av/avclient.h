// SPDX-FileCopyrightText: 2024 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef AVCLIENT_H
#define AVCLIENT_H

#include <QObject>
#include <QScopedPointer>
#include <QDBusInterface>

class AVClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
public:
    explicit AVClient(QObject *parent = nullptr);
    ~AVClient() override = default;

    QString filePath() const;
    void setFilePath(const QString &filePath);

    Q_INVOKABLE void publish(const QString &msg);
    Q_INVOKABLE void scanStarted(const QString &filePath);

private:
public slots:
    void onScanFinished(const QString &filePath, bool isThreated);

signals:
    void filePathChanged();

    void scanFileResult(const QString &filePath, bool isThreated);

private:
    QString m_filePath;
    QScopedPointer<QDBusInterface> m_interface;
};

#endif // AVCLIENT_H
