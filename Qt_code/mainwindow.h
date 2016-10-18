#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include <climits>
#include <QAudioBuffer>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioProbe>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QHeaderView>
#include <QImage>
#include <QIODevice>
#include <QMainWindow>
#include <QMediaMetaData>
#include <QMediaPlaylist>
#include <QMessageBox>
#include <QMimeDatabase>
#include <QSettings>
#include <QStandardItemModel>
#include <QString>
#include <QtMultimedia>
#include <QtMultimedia/QMediaPlayer>
#include <QUrl>
#include <QVector>


#include "fftcalc.h"
#include "playlistmodel.h"
#include "mydisplay.h"

namespace Ui {
class MainWindow;
}

// the MainWindow class is used to manage with all stuff
// fft, audio probing, widget properties

class MainWindow : public QMainWindow{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
  //
  // see description on mainwindow.h
  //
    void goToItem(const QModelIndex &index);
    void loadMedia();
    void ledConnect();
    void ledSendData();
    void loadPlaylist();
    void onAddMediaToPlayList(QString media);
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void metaDataChanged();
    void next();
    void playPause();
    void slotPositionChanged(qint64 e);
    void prev();
    void processBuffer(QAudioBuffer buffer);
    void setMediaAt(qint32 percent);
    void setVolume(int volume);
    void spectrumAvailable(QVector<double> spectrum);
    void metaDataAvailableChanged(bool);
private:
    // LED Display
    myDisplay *d;

    // User interface widget
    Ui::MainWindow *ui;

    // pointer to the media player object
    QMediaPlayer *player;

    // stores the playlist
    QMediaPlaylist *playlist;

    // audio info... we do not use it
    QAudioDeviceInfo audioInfo;

    // a buffer to copy audio into it
    QByteArray buffer;

    // the audio prober
    QAudioProbe *probe;

    // a fft calculator object
    FFTCalc *calculator;

    // item model to design the playlist into mainwindow
    QStandardItemModel *model;

    // each item to be displayed in playlist
    QStandardItem *item;

    // input sample to fft calc
    QVector<double> sample;

    // output vector with spectrum
    QVector<double> spectrum;

    PlaylistModel *playlistModel;

    // left and right mean levels
    double levelLeft, levelRight;
signals:
    // telle a new buffer from audio prober
    int spectrumChanged(QVector<double> &sample);

    // music position changed by user. Tell
    // new position to the player
    int positionChanged(qint64 position);

    // tells the new left and right mean audio levels
    int levels(double left, double right);

    // tells the duration of media
    // when a new media is played
    int elapsedTimeChanged(qint64 elapsed);

    // tells there are new directories to be added to the music library
    int addFolderToLibrary(QString folder);

protected slots:
private slots:
    void timeisOver();
};


#endif // MAINWINDOW_H
