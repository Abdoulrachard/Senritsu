#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QSqlDatabase>
#include "song.h"

class Library : public QObject
{
    Q_OBJECT
public:
    explicit Library(QObject *parent = nullptr);
    ~Library();

    void addSong(const QString& filePath);
    void removeSong(const QString& filePath);
    QList<Song*> search(const QString& query);
    void reset();
    QList<Song*> getAllSongs() const;

signals:
    void libraryChanged();

private:
    void initDatabase();
    QSqlDatabase db;
    QList<Song*> songs;
};

#endif // LIBRARY_H
