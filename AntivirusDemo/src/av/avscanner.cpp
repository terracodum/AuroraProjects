// SPDX-FileCopyrightText: 2024 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

#include "avscanner.h"
#include <QDir>
#include <QFileInfo>
#include <QFile>

AVScanner::AVScanner(QObject *parent) : QObject(parent), m_thread(new QThread)
{
    auto worker = new AVScannerWorker;
    worker->moveToThread(m_thread.data());

    connect(m_thread.data(), &QThread::finished, worker, &AVScannerWorker::deleteLater);

    connect(this, &AVScanner::operate, worker, &AVScannerWorker::onOperate);
    connect(worker, &AVScannerWorker::resultReady, this, &AVScanner::onResultReady);

    m_thread->start();
}

AVScanner::~AVScanner()
{
    m_thread->quit();
    m_thread->wait();
}

void AVScanner::scan(const QString &path)
{
    m_filePath = path;
    emit operate(m_filePath);
}

void AVScanner::onResultReady(bool isThreated)
{
    emit fileResult(m_filePath, isThreated);
}

void AVScannerWorker::onOperate(const QString &path)
{
    bool isThreated = false;
    const QFileInfo info(path);
    if (info.isFile() && !info.isSymLink()) {
        const auto suffix = info.suffix();
        if (suffix == "rpm") {
            isThreated = true;
        }
    }
    emit resultReady(isThreated);
}
