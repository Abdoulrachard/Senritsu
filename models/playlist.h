#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QObject>
#include <QList>
#include "song.h"

class Playlist : public QObject {
    Q_OBJECT

public:
    explicit Playlist(QObject *parent = nullptr);
    ~Playlist();

    void addSong(Song* song);
    void removeSong(int index);
    void clear();
    void loadFromM3u(const QString& filePath);
    void saveToM3u(const QString& filePath);
    QList<Song*> getSongs() const { return songs; }

signals:
    void playlistChanged();

private:
    QList<Song*> songs;
};

#endif // PLAYLIST_H
