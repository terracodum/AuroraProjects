// SPDX-FileCopyrightText: 2024 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef AVSERVER_H
#define AVSERVER_H

#include <QObject>
#include <QDBusAbstractAdaptor>
#include <QDBusContext>
#include <QScopedPointer>
#include <QThread>

#include "avscanner.h"

class AVServer : public QObject, public QDBusContext
{
    Q_OBJECT

public:
    explicit AVServer(QObject *parent = nullptr);
    ~AVServer() override;

    void startApplication();

    void scanStarted(const QString &filePath);

private:
    void iterateDir(const QString &binName, const QString &keyId);

public slots:
    void onFileResult(const QString &filePath, bool isThreated);

signals:
    void ScanFinished(const QString &filePath, bool isThreated);

private:
    QScopedPointer<AVScanner> m_scanner;
};

class AVServerAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "ru.auroraos.AntivirusDemo")
    Q_CLASSINFO("D-Bus Introspection",
                "<interface name=\"ru.omp.test.antivirus\">\n"
                "  <method name=\"StartApplication\">\n"
                "  </method>\n"
                "</interface>\n"
                "<interface name=\"ru.auroraos.AntivirusDemo\">\n"
                "  <method name=\"ScanStarted\">\n"
                "    <arg name=\"file_path\" type=\"s\" direction=\"in\"/>\n"
                "  </method>\n"
                "</interface>\n"
                "<interface name=\"ru.auroraos.AntivirusDemo\">\n"
                "  <signal name=\"ScanFinished\">\n"
                "    <arg name=\"file_path\" type=\"s\" direction=\"out\"/>\n"
                "    <arg name=\"is_threated\" type=\"b\" direction=\"out\"/>\n"
                "  </method>\n"
                "</interface>\n")

public:
    explicit AVServerAdaptor(AVServer *parent);

public slots:
    void StartApplication();

    void ScanStarted(const QString &filePath);

signals:
    void ScanFinished(const QString &filePath, bool isThreated);

private:
    AVServer *q;
};

#endif // AVSERVER_H
