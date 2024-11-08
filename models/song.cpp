#include "song.h"
#include <QFileInfo>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QEventLoop>

Song::Song(const QString& path) 
    : filePath(path)
    , title("")
    , artist("Unknown Artist")
    , album("Unknown Album")
    , duration(0)
{
    QFileInfo fileInfo(path);
    title = fileInfo.baseName();
    extractMetadata();
}

void Song::extractMetadata()
{
    QMediaPlayer player;
    player.setMedia(QUrl::fromLocalFile(filePath));
    
    // Attendre que les métadonnées soient chargées
    QEventLoop loop;
    QObject::connect(&player, &QMediaPlayer::mediaStatusChanged,
                    [&](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::LoadedMedia)
            loop.quit();
    });
    loop.exec();

    // Extraire les métadonnées
    if (!player.metaData(QMediaMetaData::Title).toString().isEmpty())
        title = player.metaData(QMediaMetaData::Title).toString();
    if (!player.metaData(QMediaMetaData::AlbumArtist).toString().isEmpty())
        artist = player.metaData(QMediaMetaData::AlbumArtist).toString();
    if (!player.metaData(QMediaMetaData::AlbumTitle).toString().isEmpty())
        album = player.metaData(QMediaMetaData::AlbumTitle).toString();
    
    duration = player.duration();
}
