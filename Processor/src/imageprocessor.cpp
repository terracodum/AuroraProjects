#include "imageprocessor.h"
#include "processingworker.h"
#include "resultimageprovider.h"

ImageProcessor::ImageProcessor(QObject *parent)
    : QObject(parent)
{
    ProcessingWorker *worker = new ProcessingWorker;   // без parent — живёт в потоке
    worker->moveToThread(&m_thread);

    connect(&m_thread, &QThread::finished, worker, &QObject::deleteLater);

    // запрос к worker только сигналом — соединение через поток автоматически Queued
    connect(this, &ImageProcessor::requestProcess, worker, &ProcessingWorker::process);

    connect(worker, &ProcessingWorker::finished, this, &ImageProcessor::onWorkerFinished);
    connect(worker, &ProcessingWorker::error, this, &ImageProcessor::onWorkerError);

    m_thread.start();
}

ImageProcessor::~ImageProcessor()
{
    m_thread.quit();   // иначе падение при закрытии приложения
    m_thread.wait();
}

void ImageProcessor::process(const QString &filePath)
{
    if (m_busy)                       // повторное нажатие во время обработки игнорируем
        return;

    m_busy = true;
    emit busyChanged();
    emit requestProcess(filePath);    // уйдёт в worker-поток
}

void ImageProcessor::onWorkerFinished(const QImage &result, qreal meanBrightness, int elapsedMs)
{
    if (m_provider)
        m_provider->setImage(result);

    m_meanBrightness = meanBrightness;
    m_elapsedMs = elapsedMs;
    // меняющийся суффикс-версия — иначе Image не перечитает провайдер
    m_resultSource = QStringLiteral("image://result/processed/%1").arg(++m_version);

    m_busy = false;
    emit busyChanged();
    emit resultChanged();
}

void ImageProcessor::onWorkerError(const QString &message)
{
    m_busy = false;
    emit busyChanged();
    emit errorOccurred(message);
}
