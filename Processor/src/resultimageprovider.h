#ifndef RESULTIMAGEPROVIDER_H
#define RESULTIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QImage>
#include <QMutex>

class ResultImageProvider : public QQuickImageProvider
{
public:
    ResultImageProvider() : QQuickImageProvider(QQuickImageProvider::Image) {}

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override
    {
        Q_UNUSED(id)
        QMutexLocker lock(&m_mutex);   // requestImage вызывается из потока загрузки Qt
        QImage result = m_image;       // QImage неявно шарится — копия дешёвая

        if (size)
            *size = result.size();

        if (!requestedSize.isEmpty())
            return result.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        return result;
    }

    void setImage(const QImage &image)   // зовём только из главного потока
    {
        QMutexLocker lock(&m_mutex);
        m_image = image;
    }

private:
    QMutex m_mutex;
    QImage m_image;
};

#endif // RESULTIMAGEPROVIDER_H
