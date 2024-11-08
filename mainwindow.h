#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QStyle>
#include "models/library.h"
#include "models/playlist.h"
#include "models/playlistmodel.h"
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QEventLoop>
#include <QMediaMetaData>
#include <QStatusBar>
#include "models/librarymodel.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void playPause();
    void next();
    void previous();
    void stop();
    void volumeUp();
    void volumeDown();
    void openFile();
    void openFolder();
    void savePlaylist();
    void loadPlaylist();
    void toggleShuffle();
    void onSearchTextChanged(const QString& text);
    void onLibraryDoubleClicked(const QModelIndex& index);
    void onAddToLibrary();
    void onResetLibrary();
    void showLibraryContextMenu(const QPoint& pos);
    void onPlaylistDoubleClicked(const QModelIndex& index);
    void showPlaylistContextMenu(const QPoint& pos);
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void playNextInLibrary();
    void playNextInPlaylist();

private:
    void setupUi();
    void setupConnections();
    void setupPlayer();
    void createActions();
    void createToolbar();
    void updatePlaybackControls();
    void updateTimeLabel();
    void updateAlbumArt(const QString& filePath);

    Ui::MainWindow *ui;
    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    Library *library;
    PlaylistModel *playlistModel;
    LibraryModel *libraryModel;
    
    // Actions
    QAction *playAction;
    QAction *pauseAction;
    QAction *stopAction;
    QAction *nextAction;
    QAction *previousAction;
    QAction *openFileAction;
    QAction *openFolderAction;
    QAction *savePlaylistAction;
    QAction *loadPlaylistAction;
    QPushButton* shuffleButton = nullptr;
    bool isShuffleEnabled = false;
    QLabel *albumArtLabel;

    enum class SourceType {
        Library,
        Playlist
    };
    SourceType currentSource = SourceType::Playlist;
};
#endif // MAINWINDOW_H
