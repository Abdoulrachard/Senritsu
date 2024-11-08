#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractTableModel>
#include <QMediaPlaylist>
#include "song.h"

class PlaylistModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit PlaylistModel(QObject *parent = nullptr);
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    
    void addSong(const QString &filePath);
    void clear();
    void setCurrentIndex(int index);
    Song* getSongAt(int index) const {
        if (index >= 0 && index < songs.size())
            return songs.at(index);
        return nullptr;
    }
    QString getCurrentSongPath() const;
    int getCurrentIndex() const;
    void removeSong(int index);

private:
    QList<Song*> songs;
    int currentPlayingIndex = -1;
};

#endif // PLAYLISTMODEL_H
