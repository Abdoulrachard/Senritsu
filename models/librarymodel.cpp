#include "librarymodel.h"
#include <QTime>
#include <QColor>

LibraryModel::LibraryModel(Library* lib, QObject *parent)
    : QAbstractTableModel(parent)
    , library(lib)
{
    connect(library, &Library::libraryChanged, this, &LibraryModel::onLibraryChanged);
    refresh();
}

LibraryModel::~LibraryModel()
{
    qDeleteAll(displayedSongs);
}

int LibraryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return displayedSongs.size();
}

int LibraryModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 4; // Titre, Artiste, Album, Durée
}

QVariant LibraryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= displayedSongs.size())
        return QVariant();

    if (role == Qt::BackgroundRole) {
        if (index.row() == currentPlayingIndex) {
            return QColor("#1a75ff");
        }
    }
    else if (role == Qt::ForegroundRole) {
        if (index.row() == currentPlayingIndex) {
            return QColor(Qt::white);
        }
    }
    else if (role == Qt::DisplayRole) {
        const Song *song = displayedSongs.at(index.row());
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

QVariant LibraryModel::headerData(int section, Qt::Orientation orientation, int role) const
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

void LibraryModel::refresh()
{
    beginResetModel();
    qDeleteAll(displayedSongs);
    displayedSongs = library->getAllSongs();
    endResetModel();
}

void LibraryModel::onLibraryChanged()
{
    refresh();
}

Song* LibraryModel::getSongAt(int index) const
{
    if (index >= 0 && index < displayedSongs.size())
        return displayedSongs.at(index);
    return nullptr;
}

void LibraryModel::updateSearch(const QString& query)
{
    beginResetModel();
    qDeleteAll(displayedSongs);
    if (query.isEmpty()) {
        displayedSongs = library->getAllSongs();
    } else {
        displayedSongs = library->search(query);
    }
    endResetModel();
}

Qt::ItemFlags LibraryModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void LibraryModel::removeSong(int index)
{
    if (index >= 0 && index < displayedSongs.size()) {
        QString filePath = displayedSongs.at(index)->getFilePath();
        library->removeSong(filePath);
    }
}

void LibraryModel::setCurrentIndex(int index)
{
    // Réinitialiser l'ancien index
    if (currentPlayingIndex >= 0) {
        QModelIndex oldIndex = createIndex(currentPlayingIndex, 0);
        QModelIndex oldIndexEnd = createIndex(currentPlayingIndex, columnCount() - 1);
        currentPlayingIndex = -1;
        emit dataChanged(oldIndex, oldIndexEnd);
    }

    // Mettre à jour le nouvel index
    if (index >= 0 && index < displayedSongs.size()) {
        currentPlayingIndex = index;
        QModelIndex newIndex = createIndex(index, 0);
        QModelIndex newIndexEnd = createIndex(index, columnCount() - 1);
        emit dataChanged(newIndex, newIndexEnd);
    }
}

QString LibraryModel::getCurrentSongPath() const
{
    if (currentPlayingIndex >= 0 && currentPlayingIndex < displayedSongs.size()) {
        return displayedSongs.at(currentPlayingIndex)->getFilePath();
    }
    return QString();
}
