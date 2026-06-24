#ifndef NOTESMODEL_H
#define NOTESMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QList>

class NotesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum Roles {
        TextRole = Qt::UserRole + 1,
        CreatedAtRole
    };

    explicit NotesModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addNote(const QString &text);
    Q_INVOKABLE void removeNote(int row);

signals:
    void countChanged();

private:
    struct Note {
        QString text;
        QDateTime createdAt;
    };

    void load();
    void save() const;
    QString storagePath() const;

    QList<Note> m_notes;
};

#endif // NOTESMODEL_H
