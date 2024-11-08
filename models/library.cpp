#include "library.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

Library::Library(QObject *parent) : QObject(parent)
{
    initDatabase();
}

Library::~Library()
{
    qDeleteAll(songs);
    db.close();
}

void Library::initDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("senritsu.db");
    
    if (!db.open()) {
        qDebug() << "Erreur lors de l'ouverture de la base de données:" << db.lastError().text();
        return;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS songs ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "title TEXT,"
               "artist TEXT,"
               "album TEXT,"
               "duration INTEGER,"
               "filepath TEXT UNIQUE"
               ")");
}

void Library::addSong(const QString& filePath)
{
    Song* song = new Song(filePath);
    QSqlQuery query;
    
    // Vérifier si le fichier existe déjà
    query.prepare("SELECT COUNT(*) FROM songs WHERE filepath = :filepath");
    query.bindValue(":filepath", filePath);
    query.exec();
    
    if (query.next() && query.value(0).toInt() > 0) {
        delete song;
        return;
    }
    
    // Ajouter la nouvelle chanson
    query.prepare("INSERT INTO songs (title, artist, album, duration, filepath) "
                 "VALUES (:title, :artist, :album, :duration, :filepath)");
                 
    query.bindValue(":title", song->getTitle());
    query.bindValue(":artist", song->getArtist());
    query.bindValue(":album", song->getAlbum());
    query.bindValue(":duration", song->getDuration());
    query.bindValue(":filepath", song->getFilePath());
    
    if (query.exec()) {
        songs.append(song);
        emit libraryChanged();
    } else {
        delete song;
        qDebug() << "Erreur lors de l'ajout de la chanson:" << query.lastError().text();
    }
}

QList<Song*> Library::search(const QString& query)
{
    QList<Song*> results;
    QSqlQuery sqlQuery;
    sqlQuery.prepare("SELECT filepath FROM songs WHERE "
                    "title LIKE :query OR "
                    "artist LIKE :query OR "
                    "album LIKE :query");
    sqlQuery.bindValue(":query", "%" + query + "%");
    
    if (sqlQuery.exec()) {
        while (sqlQuery.next()) {
            QString filePath = sqlQuery.value(0).toString();
            results.append(new Song(filePath));
        }
    }
    
    return results;
}

void Library::reset()
{
    QSqlQuery query;
    query.exec("DELETE FROM songs");
    qDeleteAll(songs);
    songs.clear();
    emit libraryChanged();
}

QList<Song*> Library::getAllSongs() const
{
    QList<Song*> allSongs;
    QSqlQuery query;
    query.prepare("SELECT title, artist, album, duration, filepath FROM songs");
    
    if (query.exec()) {
        while (query.next()) {
            QString filePath = query.value(4).toString();
            Song* song = new Song(filePath);
            allSongs.append(song);
        }
    } else {
        qDebug() << "Erreur lors de la récupération des chansons:" << query.lastError().text();
    }
    
    return allSongs;
}

void Library::removeSong(const QString& filePath)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM songs WHERE filepath = :filepath");
    query.bindValue(":filepath", filePath);
    
    if (query.exec()) {
        // Supprimer de la liste en mémoire
        for (int i = 0; i < songs.size(); ++i) {
            if (songs[i]->getFilePath() == filePath) {
                delete songs.takeAt(i);
                break;
            }
        }
        emit libraryChanged();
    } else {
        qDebug() << "Erreur lors de la suppression de la chanson:" << query.lastError().text();
    }
}
