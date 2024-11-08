#ifndef LIBRARYMODEL_H
#define LIBRARYMODEL_H

#include <QAbstractTableModel>
#include "song.h"
#include "library.h"

class LibraryModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit LibraryModel(Library* library, QObject *parent = nullptr);
    ~LibraryModel();
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    
    void refresh();
    Song* getSongAt(int index) const;
    void updateSearch(const QString& query);
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void removeSong(int index);
    void setCurrentIndex(int index);
    QString getCurrentSongPath() const;
    int getCurrentIndex() const { return currentPlayingIndex; }

private slots:
    void onLibraryChanged();

private:
    Library* library;
    QList<Song*> displayedSongs;
    int currentPlayingIndex = -1;
};

#endif // LIBRARYMODEL_H
