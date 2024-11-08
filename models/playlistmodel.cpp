#include "playlistmodel.h"
#include <QTime>
#include <QColor>

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return songs.size();
}

int PlaylistModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 4; // Titre, Artiste, Album, Durée
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= songs.size())
        return QVariant();

    if (role == Qt::BackgroundRole) {
        if (index.row() == currentPlayingIndex) {
            return QColor("#1a75ff");  // Couleur de surbrillance
        }
    }
    else if (role == Qt::ForegroundRole) {
        if (index.row() == currentPlayingIndex) {
            return QColor(Qt::white);  // Texte blanc pour la chanson en cours
        }
    }
    else if (role == Qt::DisplayRole) {
        const Song *song = songs.at(index.row());
        switch (index.column()) {
            case 0:
                return song->getTitle();
            case 1:
                return song->getArtist();
            case 2:
                return song->getAlbum();
            case 3: {
                QTime time(0, 0);
                time = time.addMSecs(song->getDuration());
                return time.toString("mm:ss");
            }
        }
    }
    return QVariant();
}

QVariant PlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("Titre");
            case 1:
                return tr("Artiste");
            case 2:
                return tr("Album");
            case 3:
                return tr("Durée");
        }
    }
    return QVariant();
}

void PlaylistModel::addSong(const QString &filePath)
{
    beginInsertRows(QModelIndex(), songs.size(), songs.size());
    songs.append(new Song(filePath));
    endInsertRows();
}

void PlaylistModel::clear()
{
    beginResetModel();
    qDeleteAll(songs);
    songs.clear();
    endResetModel();
}

void PlaylistModel::setCurrentIndex(int index)
{
    // Réinitialiser l'ancien index
    if (currentPlayingIndex >= 0) {
        QModelIndex oldIndex = createIndex(currentPlayingIndex, 0);
        QModelIndex oldIndexEnd = createIndex(currentPlayingIndex, columnCount() - 1);
        currentPlayingIndex = -1;
        emit dataChanged(oldIndex, oldIndexEnd);
    }

    // Mettre à jour le nouvel index
    if (index >= 0 && index < songs.size()) {
        currentPlayingIndex = index;
        QModelIndex newIndex = createIndex(index, 0);
        QModelIndex newIndexEnd = createIndex(index, columnCount() - 1);
        emit dataChanged(newIndex, newIndexEnd);
    }
}

QString PlaylistModel::getCurrentSongPath() const
{
    if (currentPlayingIndex >= 0 && currentPlayingIndex < songs.size()) {
        return songs.at(currentPlayingIndex)->getFilePath();
    }
    return QString();
}

int PlaylistModel::getCurrentIndex() const
{
    return currentPlayingIndex;
}

void PlaylistModel::removeSong(int index)
{
    if (index >= 0 && index < songs.size()) {
        beginRemoveRows(QModelIndex(), index, index);
        delete songs.takeAt(index);
        endRemoveRows();
    }
}
