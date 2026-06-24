#include "processingworker.h"

#include <QElapsedTimer>
#include <QUrl>

void ProcessingWorker::process(const QString &filePath)
{
    QElapsedTimer timer;
    timer.start();

    QString path = filePath;
    if (path.startsWith(QStringLiteral("file://")))   // пикер отдаёт URL
        path = QUrl(path).toLocalFile();

    QImage src(path);
    if (src.isNull()) {
        emit error(QStringLiteral("Не удалось загрузить изображение"));
        return;
    }

    const QImage gray = src.convertToFormat(QImage::Format_Grayscale8);

    quint64 sum = 0;
    const int w = gray.width();
    const int h = gray.height();
    for (int y = 0; y < h; ++y) {
        const uchar *line = gray.constScanLine(y);   // 1 байт на пиксель
        for (int x = 0; x < w; ++x)
            sum += line[x];
    }
    const qreal mean = (w && h) ? qreal(sum) / (qreal(w) * h) : 0.0;

    emit finished(gray, mean, int(timer.elapsed()));
}
