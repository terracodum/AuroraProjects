#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>
#include <QThread>
#include <QImage>

class ResultImageProvider;

class ImageProcessor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(qreal meanBrightness READ meanBrightness NOTIFY resultChanged)
    Q_PROPERTY(int elapsedMs READ elapsedMs NOTIFY resultChanged)
    Q_PROPERTY(QString resultSource READ resultSource NOTIFY resultChanged)

public:
    explicit ImageProcessor(QObject *parent = nullptr);
    ~ImageProcessor() override;

    void setProvider(ResultImageProvider *provider) { m_provider = provider; }

    bool busy() const { return m_busy; }
    qreal meanBrightness() const { return m_meanBrightness; }
    int elapsedMs() const { return m_elapsedMs; }
    QString resultSource() const { return m_resultSource; }

    Q_INVOKABLE void process(const QString &filePath);

signals:
    void busyChanged();
    void resultChanged();
    void errorOccurred(const QString &message);
    void requestProcess(const QString &filePath);   // запрос к worker через очередь

private slots:
    void onWorkerFinished(const QImage &result, qreal meanBrightness, int elapsedMs);
    void onWorkerError(const QString &message);

private:
    QThread m_thread;
    ResultImageProvider *m_provider = nullptr;
    bool m_busy = false;
    qreal m_meanBrightness = 0.0;
    int m_elapsedMs = 0;
    QString m_resultSource;
    int m_version = 0;
};

#endif // IMAGEPROCESSOR_H
