#ifndef SONG_H
#define SONG_H

#include <QString>
#include <QUrl>

class Song {
public:
    Song(const QString& filePath);
    
    QString getTitle() const { return title; }
    QString getArtist() const { return artist; }
    QString getAlbum() const { return album; }
    int getDuration() const { return duration; }
    QString getFilePath() const { return filePath; }
    QUrl getUrl() const { return QUrl::fromLocalFile(filePath); }

private:
    QString filePath = "";
    QString title = "";
    QString artist = "";
    QString album = "";
    int duration = 0;
    
    void extractMetadata();
};

#endif // SONG_H
