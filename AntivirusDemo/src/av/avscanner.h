// SPDX-FileCopyrightText: 2024 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef AVSCANNER_H
#define AVSCANNER_H

#include <QObject>
#include <QThread>

class AVScanner : public QObject
{
    Q_OBJECT
public:
    explicit AVScanner(QObject *parent = nullptr);
    ~AVScanner() override;

    void scan(const QString &path);

public slots:
    void onResultReady(bool isThreated);

signals:
    void operate(const QString &path);

    void fileResult(const QString &filePath, bool isThreated);

private:
    QScopedPointer<QThread> m_thread;
    QString m_filePath;
};

class AVScannerWorker : public QObject
{
    Q_OBJECT
public slots:
    void onOperate(const QString &path);

signals:
    void resultReady(bool isThreated);
};

#endif // AVSCANNER_H
