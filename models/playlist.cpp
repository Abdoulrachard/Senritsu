#include "playlist.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>

Playlist::Playlist(QObject *parent) : QObject(parent)
{
}

Playlist::~Playlist()
{
    clear();
}

void Playlist::addSong(Song* song)
{
    songs.append(song);
    emit playlistChanged();
}

void Playlist::removeSong(int index)
{
    if (index >= 0 && index < songs.size()) {
        delete songs.takeAt(index);
        emit playlistChanged();
    }
}

void Playlist::clear()
{
    qDeleteAll(songs);
    songs.clear();
    emit playlistChanged();
}

void Playlist::loadFromM3u(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    clear();
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty() && !line.startsWith("#")) {
            QFileInfo fileInfo(line);
            if (fileInfo.exists())
                addSong(new Song(line));
        }
    }
}

void Playlist::saveToM3u(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << "#EXTM3U\n";
    for (Song* song : songs) {
        out << song->getFilePath() << "\n";
    }
}
