#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QStyle>
#include <QToolBar>
#include <QMessageBox>
#include <QTime>
#include <QMediaPlayer>
#include <QEventLoop>
#include <QImage>
#include <QPixmap>
#include <QGraphicsDropShadowEffect>
#include <QMediaMetaData>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , player(new QMediaPlayer(this))
    , playlist(new QMediaPlaylist(this))
    , library(new Library(this))
    , playlistModel(new PlaylistModel(this))
    , libraryModel(new LibraryModel(library, this))
{
    ui->setupUi(this);
    
    // Configurez la vue de la bibliothèque
    ui->libraryView->setModel(libraryModel);
    ui->libraryView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    setStyleSheet(R"(
        QMainWindow {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        QTableView {
            background-color: #333333;
            color: #ffffff;
            border: none;
            gridline-color: #444444;
        }
        QTableView::item:selected {
            background-color: #0066cc;
        }
        QHeaderView::section {
            background-color: #404040;
            color: #ffffff;
            padding: 5px;
            border: none;
        }
        QPushButton {
            background-color: #404040;
            color: #ffffff;
            border: none;
            padding: 5px 10px;
            border-radius: 3px;
        }
        QPushButton:hover {
            background-color: #505050;
        }
        QSlider::groove:horizontal {
            background: #404040;
            height: 4px;
        }
        QSlider::handle:horizontal {
            background: #0066cc;
            width: 12px;
            margin: -4px 0;
            border-radius: 6px;
        }
        QToolBar {
            background-color: #333333;
            border: none;
            spacing: 3px;
        }
        QToolButton {
            background-color: transparent;
            border: none;
            padding: 5px;
        }
        QToolButton:hover {
            background-color: #404040;
            border-radius: 3px;
        }
    )");
    
    setupUi();
    setupPlayer();
    createActions();
    createToolbar();
    setupConnections();
    
    setWindowTitle(tr("Senritsu"));
}

void MainWindow::setupUi()
{
    // Configuration des icônes des boutons
    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->previousButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    
    // Configuration des icônes de volume
    ui->volumeLowLabel->setPixmap(style()->standardIcon(QStyle::SP_MediaVolumeMuted).pixmap(16, 16));
    ui->volumeHighLabel->setPixmap(style()->standardIcon(QStyle::SP_MediaVolume).pixmap(16, 16));
    
    // Configuration du bouton shuffle
    ui->shuffleButton->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    ui->shuffleButton->setCheckable(true);
    ui->shuffleButton->setToolTip(tr("Lecture aléatoire"));
    
    // Style du bouton shuffle
    ui->shuffleButton->setStyleSheet(R"(
        QPushButton {
            background-color: #404040;
            border: none;
            padding: 5px 10px;
            border-radius: 3px;
            margin: 0 2px;
        }
        QPushButton:hover {
            background-color: #505050;
        }
        QPushButton:checked {
            background-color: #0066cc;
        }
        QPushButton:checked:hover {
            background-color: #0077ee;
        }
    )");

    // Configuration du volume initial
    player->setVolume(ui->volumeSlider->value());
    
    // Configuration de la vue playlist
    ui->playlistView->setModel(playlistModel);
    ui->playlistView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Style pour le label de la pochette
    ui->albumArtLabel->setStyleSheet(R"(
        QLabel {
            background-color: #333333;
            border-radius: 10px;
            padding: 10px;
        }
    )");

    // Effet d'ombre pour la pochette
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(20);
    shadowEffect->setColor(QColor(0, 0, 0, 160));
    shadowEffect->setOffset(0, 0);
    ui->albumArtLabel->setGraphicsEffect(shadowEffect);
}

void MainWindow::setupPlayer()
{
    playlist->setPlaybackMode(QMediaPlaylist::Sequential);
    player->setPlaylist(playlist);
}

void MainWindow::createActions()
{
    // Création des actions pour la barre d'outils et le menu
    openFileAction = new QAction(tr("Ouvrir fichier"), this);
    openFileAction->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    connect(openFileAction, &QAction::triggered, this, &MainWindow::openFile);

    openFolderAction = new QAction(tr("Ouvrir dossier"), this);
    openFolderAction->setIcon(style()->standardIcon(QStyle::SP_DirIcon));
    connect(openFolderAction, &QAction::triggered, this, &MainWindow::openFolder);

    savePlaylistAction = new QAction(tr("Sauvegarder playlist"), this);
    savePlaylistAction->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    connect(savePlaylistAction, &QAction::triggered, this, &MainWindow::savePlaylist);

    loadPlaylistAction = new QAction(tr("Charger playlist"), this);
    loadPlaylistAction->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    connect(loadPlaylistAction, &QAction::triggered, this, &MainWindow::loadPlaylist);
}

void MainWindow::createToolbar()
{
    QToolBar *toolbar = addToolBar(tr("Outils"));
    toolbar->addAction(openFileAction);
    toolbar->addAction(openFolderAction);
    toolbar->addSeparator();
    toolbar->addAction(savePlaylistAction);
    toolbar->addAction(loadPlaylistAction);
}

void MainWindow::setupConnections()
{
    // Connexions des boutons de contrôle
    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::playPause);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::stop);
    connect(ui->previousButton, &QPushButton::clicked, this, &MainWindow::previous);
    connect(ui->nextButton, &QPushButton::clicked, this, &MainWindow::next);
    
    // Connexions du lecteur
    connect(player, &QMediaPlayer::stateChanged, this, &MainWindow::updatePlaybackControls);
    connect(player, &QMediaPlayer::positionChanged, this, [this](qint64 position) {
        ui->progressSlider->setValue(position);
        updateTimeLabel();
    });
    connect(player, &QMediaPlayer::durationChanged, this, [this](qint64 duration) {
        ui->progressSlider->setMaximum(duration);
        updateTimeLabel();
    });
    
    // Connexion du slider de volume
    connect(ui->volumeSlider, &QSlider::valueChanged, player, &QMediaPlayer::setVolume);
    
    // Connexion du slider de progression
    connect(ui->progressSlider, &QSlider::sliderMoved, player, &QMediaPlayer::setPosition);
    
    // Connexion pour suivre la chanson en cours
    connect(player, &QMediaPlayer::currentMediaChanged, this, [this](const QMediaContent &content) {
        if (content.isNull()) return;
        
        // Trouver l'index de la chanson en cours dans la playlist
        QString currentPath = content.canonicalUrl().toLocalFile();
        for (int i = 0; i < playlistModel->rowCount(); ++i) {
            Song* song = playlistModel->getSongAt(i);
            if (song && song->getFilePath() == currentPath) {
                playlistModel->setCurrentIndex(i);
                // Optionnel : faire défiler la vue jusqu'à la chanson en cours
                ui->playlistView->scrollTo(playlistModel->index(i, 0));
                break;
            }
        }
    });

    // Connexion pour le mode aléatoire
    connect(ui->shuffleButton, &QPushButton::clicked, this, &MainWindow::toggleShuffle);

    // Mise à jour de la pochette quand la chanson change
    connect(player, &QMediaPlayer::currentMediaChanged, this, [this](const QMediaContent &content) {
        if (!content.isNull()) {
            QString filePath = content.canonicalUrl().toLocalFile();
            updateAlbumArt(filePath);
        }
    });

    // Ajoutez ces nouvelles connexions
    connect(ui->searchLineEdit, &QLineEdit::textChanged,
            this, &MainWindow::onSearchTextChanged);
            
    connect(ui->libraryView, &QTableView::doubleClicked,
            this, &MainWindow::onLibraryDoubleClicked);

    connect(ui->addToLibraryButton, &QPushButton::clicked,
            this, &MainWindow::onAddToLibrary);
            
    connect(ui->resetLibraryButton, &QPushButton::clicked,
            this, &MainWindow::onResetLibrary);

    // Double-clic pour ajouter à la playlist
    connect(ui->libraryView, &QTableView::doubleClicked,
            this, &MainWindow::onLibraryDoubleClicked);
            
    // Menu contextuel pour la suppression
    ui->libraryView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->libraryView, &QTableView::customContextMenuRequested,
            this, &MainWindow::showLibraryContextMenu);

    connect(ui->playlistView, &QTableView::doubleClicked, this, &MainWindow::onPlaylistDoubleClicked);
    connect(ui->playlistView, &QTableView::customContextMenuRequested, this, &MainWindow::showPlaylistContextMenu);

    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onMediaStatusChanged);
}

void MainWindow::playPause()
{
    if (player->state() == QMediaPlayer::PlayingState) {
        player->pause();
    } else {
        if (player->state() == QMediaPlayer::StoppedState) {
            if (currentSource == SourceType::Library) {
                if (libraryModel->getCurrentIndex() == -1 && libraryModel->rowCount() > 0) {
                    libraryModel->setCurrentIndex(0);
                }
                QString currentSong = libraryModel->getCurrentSongPath();
                if (!currentSong.isEmpty()) {
                    player->setMedia(QUrl::fromLocalFile(currentSong));
                }
            } else {
                if (playlistModel->getCurrentIndex() == -1 && playlistModel->rowCount() > 0) {
                    playlistModel->setCurrentIndex(0);
                }
                QString currentSong = playlistModel->getCurrentSongPath();
                if (!currentSong.isEmpty()) {
                    player->setMedia(QUrl::fromLocalFile(currentSong));
                }
            }
        }
        player->play();
    }
}

void MainWindow::stop()
{
    player->stop();
}

void MainWindow::next()
{
    if (currentSource == SourceType::Library) {
        int nextIndex = libraryModel->getCurrentIndex() + 1;
        if (nextIndex < libraryModel->rowCount()) {
            libraryModel->setCurrentIndex(nextIndex);
            player->setMedia(QUrl::fromLocalFile(libraryModel->getCurrentSongPath()));
            player->play();
        }
    } else {
        int nextIndex = playlistModel->getCurrentIndex() + 1;
        if (nextIndex < playlistModel->rowCount()) {
            playlistModel->setCurrentIndex(nextIndex);
            player->setMedia(QUrl::fromLocalFile(playlistModel->getCurrentSongPath()));
            player->play();
        }
    }
}

void MainWindow::previous()
{
    if (currentSource == SourceType::Library) {
        int prevIndex = libraryModel->getCurrentIndex() - 1;
        if (prevIndex >= 0) {
            libraryModel->setCurrentIndex(prevIndex);
            player->setMedia(QUrl::fromLocalFile(libraryModel->getCurrentSongPath()));
            player->play();
        }
    } else {
        int prevIndex = playlistModel->getCurrentIndex() - 1;
        if (prevIndex >= 0) {
            playlistModel->setCurrentIndex(prevIndex);
            player->setMedia(QUrl::fromLocalFile(playlistModel->getCurrentSongPath()));
            player->play();
        }
    }
}

void MainWindow::volumeUp()
{
    int newVolume = qMin(player->volume() + 10, 100);
    player->setVolume(newVolume);
    ui->volumeSlider->setValue(newVolume);
}

void MainWindow::volumeDown()
{
    int newVolume = qMax(player->volume() - 10, 0);
    player->setVolume(newVolume);
    ui->volumeSlider->setValue(newVolume);
}

void MainWindow::openFile()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        tr("Ouvrir des fichiers audio"),
        QDir::homePath(),
        tr("Fichiers audio (*.mp3 *.wav *.ogg *.m4a);;Tous les fichiers (*.*)")
    );

    for (const QString& file : files) {
        playlistModel->addSong(file);
        playlist->addMedia(QUrl::fromLocalFile(file));
    }
}

void MainWindow::openFolder()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Ouvrir un dossier"),
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (!dir.isEmpty()) {
        QDir directory(dir);
        QStringList filters;
        filters << "*.mp3" << "*.wav" << "*.ogg" << "*.m4a";
        QFileInfoList files = directory.entryInfoList(filters, QDir::Files);

        for (const QFileInfo& file : files) {
            playlistModel->addSong(file.filePath());
            playlist->addMedia(QUrl::fromLocalFile(file.filePath()));
        }
    }
}

void MainWindow::savePlaylist()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Sauvegarder la playlist"),
        QDir::homePath() + "/ma_playlist.m3u",
        tr("Playlist M3U (*.m3u)")
    );

    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(".m3u", Qt::CaseInsensitive))
            fileName += ".m3u";

        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << "#EXTM3U\n";
            
            for (int i = 0; i < playlist->mediaCount(); ++i) {
                Song* song = playlistModel->getSongAt(i);
                if (song) {
                    // Écriture des métadonnées
                    stream << "#EXTINF:" << song->getDuration()/1000 << ","
                          << song->getArtist() << " - " << song->getTitle() << "\n";
                    // Écriture du chemin du fichier
                    stream << song->getFilePath() << "\n";
                }
            }
            
            file.close();
            QMessageBox::information(this, tr("Sauvegarde réussie"),
                tr("La playlist a été sauvegardée dans :\n%1").arg(fileName));
        } else {
            QMessageBox::warning(this, tr("Erreur"),
                tr("Impossible de sauvegarder la playlist."));
        }
    }
}

void MainWindow::loadPlaylist()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Charger une playlist"),
        QDir::homePath(),
        tr("Playlist M3U (*.m3u)")
    );

    if (!fileName.isEmpty()) {
        playlist->clear();
        QFile file(fileName);
        
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            while (!stream.atEnd()) {
                QString line = stream.readLine().trimmed();
                if (!line.isEmpty() && !line.startsWith("#")) {
                    QUrl url = QUrl::fromLocalFile(line);
                    playlist->addMedia(url);
                    library->addSong(line);
                }
            }
        }
    }
}

void MainWindow::updatePlaybackControls()
{
    if (player->state() == QMediaPlayer::PlayingState) {
        ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    } else {
        ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
}

void MainWindow::updateTimeLabel()
{
    QTime currentTime(0, 0);
    currentTime = currentTime.addMSecs(player->position());
    
    QTime totalTime(0, 0);
    totalTime = totalTime.addMSecs(player->duration());
    
    QString format = "mm:ss";
    ui->currentTimeLabel->setText(currentTime.toString(format));
    ui->totalTimeLabel->setText(totalTime.toString(format));
}

void MainWindow::toggleShuffle()
{
    isShuffleEnabled = !isShuffleEnabled;
    if (isShuffleEnabled) {
        ui->shuffleButton->setStyleSheet("background-color: #1db954;"); 
    } else {
        ui->shuffleButton->setStyleSheet("");
    }
    qDebug() << "Mode aléatoire:" << (isShuffleEnabled ? "activé" : "désactivé");
}

void MainWindow::updateAlbumArt(const QString& filePath)
{
    QMediaPlayer tempPlayer;
    tempPlayer.setMedia(QUrl::fromLocalFile(filePath));
    
    // Attendre que les métadonnées soient chargées
    QEventLoop loop;
    connect(&tempPlayer, &QMediaPlayer::mediaStatusChanged,
            [&](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::LoadedMedia)
            loop.quit();
    });
    loop.exec();

    QImage albumArt;
    bool hasAlbumArt = false;

    // Essayer d'extraire la pochette du fichier
    if (tempPlayer.isMetaDataAvailable()) {
        QVariant art = tempPlayer.metaData(QMediaMetaData::CoverArtImage);
        if (art.isValid()) {
            albumArt = art.value<QImage>();
            hasAlbumArt = true;
        }
    }

    // Si aucune pochette n'est trouvée, utiliser l'image par défaut
    if (!hasAlbumArt) {
        albumArt = QImage(":/images/images/default_album.jpg");
    }

    // Redimensionner et afficher la pochette
    QPixmap pixmap = QPixmap::fromImage(albumArt)
        .scaled(ui->albumArtLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->albumArtLabel->setPixmap(pixmap);
}

void MainWindow::onSearchTextChanged(const QString& text)
{
    if (libraryModel) {
        libraryModel->updateSearch(text);
    }
}

void MainWindow::onLibraryDoubleClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    currentSource = SourceType::Library;  // Changer la source active
    libraryModel->setCurrentIndex(index.row());
    QString filePath = libraryModel->getCurrentSongPath();
    if (!filePath.isEmpty()) {
        player->setMedia(QUrl::fromLocalFile(filePath));
        player->play();
    }
}

void MainWindow::onAddToLibrary()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        tr("Ajouter des fichiers à la bibliothèque"),
        QString(),
        tr("Fichiers audio (*.mp3 *.wav *.ogg *.m4a)")
    );
    
    for (const QString& file : files) {
        library->addSong(file);
    }
}

void MainWindow::onResetLibrary()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Réinitialiser la bibliothèque"),
        tr("Êtes-vous sûr de vouloir supprimer toutes les chansons de la bibliothèque ?"),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        library->reset();
    }
}

void MainWindow::showLibraryContextMenu(const QPoint& pos)
{
    QModelIndex index = ui->libraryView->indexAt(pos);
    if (!index.isValid())
        return;
        
    QMenu contextMenu(this);
    QAction* removeAction = contextMenu.addAction(tr("Supprimer"));
    QAction* playAction = contextMenu.addAction(tr("Lire"));
    QAction* addToPlaylistAction = contextMenu.addAction(tr("Ajouter à la playlist"));
    
    QAction* selectedAction = contextMenu.exec(ui->libraryView->viewport()->mapToGlobal(pos));
    if (selectedAction == removeAction) {
        libraryModel->removeSong(index.row());
    }
    else if (selectedAction == playAction) {
        Song* song = libraryModel->getSongAt(index.row());
        if (song) {
            player->setMedia(QUrl::fromLocalFile(song->getFilePath()));
            player->play();
        }
    }
    else if (selectedAction == addToPlaylistAction) {
        Song* song = libraryModel->getSongAt(index.row());
        if (song) {
            playlistModel->addSong(song->getFilePath());
        }
    }
}

void MainWindow::onPlaylistDoubleClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    currentSource = SourceType::Playlist;
    playlistModel->setCurrentIndex(index.row());
    QString filePath = playlistModel->getCurrentSongPath();
    if (!filePath.isEmpty()) {
        player->setMedia(QUrl::fromLocalFile(filePath));
        player->play();
    }
}

void MainWindow::showPlaylistContextMenu(const QPoint& pos)
{
    QModelIndex index = ui->playlistView->indexAt(pos);
    if (!index.isValid())
        return;
        
    QMenu contextMenu(this);
    QAction* removeAction = contextMenu.addAction(tr("Supprimer"));
    QAction* playAction = contextMenu.addAction(tr("Lire"));
    
    QAction* selectedAction = contextMenu.exec(ui->playlistView->viewport()->mapToGlobal(pos));
    if (selectedAction == removeAction) {
        playlistModel->removeSong(index.row());
    }
    else if (selectedAction == playAction) {
        Song* song = playlistModel->getSongAt(index.row());
        if (song) {
            player->setMedia(QUrl::fromLocalFile(song->getFilePath()));
            player->play();
        }
    }
}

void MainWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        if (currentSource == SourceType::Library) {
            playNextInLibrary();
        } else {
            playNextInPlaylist();
        }
    }
}

void MainWindow::playNextInLibrary()
{
    int totalSongs = libraryModel->rowCount();
    if (totalSongs == 0) return;

    int nextIndex;
    if (isShuffleEnabled) {
        QVector<int> availableIndices;
        for (int i = 0; i < totalSongs; i++) {
            if (i != libraryModel->getCurrentIndex()) {
                availableIndices.append(i);
            }
        }
        if (!availableIndices.isEmpty()) {
            int randomPos = qrand() % availableIndices.size();
            nextIndex = availableIndices[randomPos];
        } else {
            nextIndex = 0;
        }
    } else {
        nextIndex = (libraryModel->getCurrentIndex() + 1) % totalSongs;
    }

    libraryModel->setCurrentIndex(nextIndex);
    player->setMedia(QUrl::fromLocalFile(libraryModel->getCurrentSongPath()));
    player->play();
}

void MainWindow::playNextInPlaylist()
{
    int totalSongs = playlistModel->rowCount();
    if (totalSongs == 0) return;

    int nextIndex;
    if (isShuffleEnabled) {
        QVector<int> availableIndices;
        for (int i = 0; i < totalSongs; i++) {
            if (i != playlistModel->getCurrentIndex()) {
                availableIndices.append(i);
            }
        }
        if (!availableIndices.isEmpty()) {
            int randomPos = qrand() % availableIndices.size();
            nextIndex = availableIndices[randomPos];
        } else {
            nextIndex = 0;
        }
    } else {
        nextIndex = (playlistModel->getCurrentIndex() + 1) % totalSongs;
    }

    playlistModel->setCurrentIndex(nextIndex);
    player->setMedia(QUrl::fromLocalFile(playlistModel->getCurrentSongPath()));
    player->play();
}

MainWindow::~MainWindow()
{
    delete ui;
}

