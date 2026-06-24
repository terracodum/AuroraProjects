#include "notesmodel.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

NotesModel::NotesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    load();
}

int NotesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_notes.count();
}

QVariant NotesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_notes.count())
        return QVariant();

    const Note &note = m_notes.at(index.row());
    switch (role) {
    case TextRole:
        return note.text;
    case CreatedAtRole:
        return note.createdAt;   // QDateTime, не строка
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> NotesModel::roleNames() const
{
    return {
        { TextRole, "text" },
        { CreatedAtRole, "createdAt" }
    };
}

void NotesModel::addNote(const QString &text)
{
    const QString trimmed = text.trimmed();
    if (trimmed.isEmpty())   // защита от пустых и пробельных строк
        return;

    beginInsertRows(QModelIndex(), 0, 0);   // новая заметка — в начало списка
    m_notes.prepend(Note{ trimmed, QDateTime::currentDateTime() });
    endInsertRows();

    emit countChanged();
    save();
}

void NotesModel::removeNote(int row)
{
    if (row < 0 || row >= m_notes.count())
        return;

    beginRemoveRows(QModelIndex(), row, row);
    m_notes.removeAt(row);
    endRemoveRows();

    emit countChanged();
    save();
}

QString NotesModel::storagePath() const
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return dir + QStringLiteral("/notes.json");
}

void NotesModel::load()
{
    QFile file(storagePath());
    if (!file.open(QIODevice::ReadOnly))
        return;

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (!doc.isArray())
        return;

    const QJsonArray array = doc.array();
    m_notes.reserve(array.size());
    for (const QJsonValue &value : array) {
        const QJsonObject obj = value.toObject();
        Note note;
        note.text = obj.value(QStringLiteral("text")).toString();
        note.createdAt = QDateTime::fromString(
            obj.value(QStringLiteral("createdAt")).toString(), Qt::ISODate);
        m_notes.append(note);
    }
}

void NotesModel::save() const
{
    const QString path = storagePath();
    QDir().mkpath(QFileInfo(path).absolutePath());   // каталог может ещё не существовать

    QJsonArray array;
    for (const Note &note : m_notes) {
        QJsonObject obj;
        obj.insert(QStringLiteral("text"), note.text);
        obj.insert(QStringLiteral("createdAt"), note.createdAt.toString(Qt::ISODate));
        array.append(obj);
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return;
    file.write(QJsonDocument(array).toJson(QJsonDocument::Compact));
    file.close();
}
