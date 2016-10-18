#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mydisplay.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <stdio.h>
#include <string.h>

QSerialPort serial;

bool lowDetected=true;

// constructor: warm up all stuff
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  audioInfo(QAudioDeviceInfo::defaultInputDevice())
{
  // draws the ui
  ui->setupUi(this);

  // test for saving settings
  QCoreApplication::setOrganizationName("PlayerFlat");
  QSettings settings;
  settings.setValue("alo","maria");

  // defines sample size equals to spectrum size
  sample.resize(SPECSIZE);

  // threads are as separate processes running within the same
  // program. for fft calculation, it is better to move it
  // to another thread to make the calcs faster.
  // moreover, it will not slow down the ui
//  fftThread = new QThread(this);

  calculator = new FFTCalc();
 // calculator->moveToThread(fftThread);

  // launches the new media player
  player = new QMediaPlayer();

  // starts a new playlist
  playlist = new QMediaPlaylist();

  // starts the playlist model
  playlistModel = new PlaylistModel(this);

  // tell playlistmodel where is the playlist
  playlistModel->setPlaylist(playlist);

  // attach the listView to the playlistModel
  ui->listViewPlaylist->setModel(playlistModel);

  // set current index to the first element
  ui->listViewPlaylist->setCurrentIndex(playlistModel->index(playlist->currentIndex(), 0));

  loadPlaylist();

  // attachs the playlist to the player
  player->setPlaylist(playlist);

  // playlist plays in loop mode. It restarts after last song has finished playing.
  playlist->setPlaybackMode(QMediaPlaylist::Loop);

  // this allow the user to select the media it wants to play
  connect(ui->listViewPlaylist, SIGNAL(doubleClicked(QModelIndex)),
          this, SLOT(goToItem(QModelIndex)));

  // when play position changes, start callback function
  connect(player, SIGNAL(positionChanged(qint64)),
          this, SLOT(slotPositionChanged(qint64)));

  // if some metadata changed for media, display it somewhere
  // it seems not work on windows
  // but works for linux :)
  connect(player,SIGNAL(metaDataChanged()),
          this, SLOT(metaDataChanged()));

  // if metadata is available, take care of it
  // it seems not work for linux
  // but works for windows :p
  connect(player, SIGNAL(metaDataAvailableChanged(bool)),
          this, SLOT(metaDataAvailableChanged(bool)));

  // the media status changed (new stream has arrived)
  connect(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
          this, SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));

  // the user selected a new position on music to play
  // perharps using some scrollbar
  connect(this,SIGNAL(positionChanged(qint64)),
          player,SLOT(setPosition(qint64)));

  connect(player,SIGNAL(volumeChanged(int)),
                        ui->control,SLOT(onVolumeChanged(int)));

  // that is the audio probe object that "listen to"
  // the music. It will help with fft stuff
  probe = new QAudioProbe();

  // fft is delivered using a QVector<double> but
  // signal/slot scheme does not recognizes this type by default
  // therefore, we have to register it
  qRegisterMetaType< QVector<double> >("QVector<double>");

  // here goes the control unit event handlers
  connect(ui->control, SIGNAL(playPause()), this, SLOT(playPause()));
  connect(ui->control, SIGNAL(prev()), this, SLOT(prev()));
  connect(ui->control, SIGNAL(next()), this, SLOT(next()));

  // when the music position changes on player, it has to be
  // informed to the control unit to redraw it ui
  connect(player,SIGNAL(positionChanged(qint64)),
          ui->control,SLOT(onElapsedChanged(qint64)));

  // fft goes here...
  // if a new audio buffer is ok, we have to make some
  // calcs (fft) to display the spectrum
  connect(probe, SIGNAL(audioBufferProbed(QAudioBuffer)),
          this, SLOT(processBuffer(QAudioBuffer)));

  // if the user selected a new position on stream to play
  // we have to tell it to the player
  connect(ui->control, SIGNAL(elapsedSelected(qint64)),
          player, SLOT(setPosition(qint64)));

  // changing audio volume
  connect(ui->control, SIGNAL(volumeSelected(int)),
          player, SLOT(setVolume(int)));

  // calculator is the thead that calcs the ffts we need to display
  // every time a new spectrum is available, the calculator
  // emits a calculatedSpectrum signal
  connect(calculator, SIGNAL(calculatedSpectrum(QVector<double>)),
          this, SLOT(spectrumAvailable(QVector<double>)));

  // tells the probe what to probe
  probe->setSource(player);

  d = new myDisplay(8,32,this);

  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(timeisOver()));
  timer->start(50);

}

// what to do when user select a new song to play
void MainWindow::goToItem(const QModelIndex &index){
  if (index.isValid()) {
    playlist->setCurrentIndex(index.row());
    player->play();
  }
}

// prepares the playlist to display the media to be played
// it should be stored into a file with settings.
// I will let this to a far future
void MainWindow::loadPlaylist(void){
}

void MainWindow::onAddMediaToPlayList(QString media){
  playlist->addMedia(QUrl::fromLocalFile(media));
}

// process audio buffer for fft calculations
void MainWindow::processBuffer(QAudioBuffer buffer){
  qreal peakValue;
  int duration;

  if(buffer.frameCount() < SPECSIZE)
    return;

  // return left and right audio mean levels
  levelLeft = levelRight = 0;
  // It only knows how to process stereo audio frames
  // mono frames = :P
  if(buffer.format().channelCount() != 2)
    return;

  sample.resize(buffer.frameCount());

//  qDebug() << " alo = " << buffer.format().sampleSize() << endl;

  // audio is signed int
  if(buffer.format().sampleType() == QAudioFormat::SignedInt){
    QAudioBuffer::S16S *data = buffer.data<QAudioBuffer::S16S>();
    // peak value changes according to sample size.
    if (buffer.format().sampleSize() == 32)
      peakValue=INT_MAX;
    else if (buffer.format().sampleSize() == 16)
      peakValue=SHRT_MAX;
    else
      peakValue=CHAR_MAX;


    //peakValue = 32767;
    //qDebug() << " Pico Signed: ------    " << peakValue << endl;
    // scale everything to [0,1]
    for(int i=0; i<buffer.frameCount(); i++){
      // for visualization purposes, we only need one of the
      // left/right channels
      sample[i] = data[i].left/peakValue;
      levelLeft+= abs(data[i].left)/peakValue;
      levelRight+= abs(data[i].right)/peakValue;
    }
  }

  // audio is unsigned int
  else if(buffer.format().sampleType() == QAudioFormat::UnSignedInt){
    QAudioBuffer::S16U *data = buffer.data<QAudioBuffer::S16U>();
    if (buffer.format().sampleSize() == 32)
      peakValue=UINT_MAX;
    else if (buffer.format().sampleSize() == 16)
      peakValue=USHRT_MAX;
    else
      peakValue=UCHAR_MAX;

    //peakValue = 32767;
    //qDebug() << " Pico Unsigned: ------    " << peakValue << endl;

    for(int i=0; i<buffer.frameCount(); i++){
      sample[i] = data[i].left/peakValue;
      levelLeft+= abs(data[i].left)/peakValue;
      levelRight+= abs(data[i].right)/peakValue;
    }
  }

  // audio is float type
  else if(buffer.format().sampleType() == QAudioFormat::Float){
    QAudioBuffer::S32F *data = buffer.data<QAudioBuffer::S32F>();
    peakValue = 1.00003;
    for(int i=0; i<buffer.frameCount(); i++){
      sample[i] = data[i].left/peakValue;
      // test if sample[i] is infinity (it works)
      // some tests produced infinity values :p
      if(sample[i] != sample[i]){
        sample[i] = 0;
      }
      else{
        levelLeft+= abs(data[i].left)/peakValue;
        levelRight+= abs(data[i].right)/peakValue;
      }
    }
  }
  //qDebug() << peakValue<<endl;
  // if the probe is listening to the audio
  // do fft calculations
  // when it is done, calculator will tell us
  if(probe->isActive()){
      //qDebug() << "buffer" << sample[4] << endl;

    duration = buffer.format().durationForBytes(buffer.frameCount())/1000;
    calculator->calc(sample, duration);
  }
  // tells anyone interested about left and right mean levels
  emit levels(levelLeft/buffer.frameCount(),levelRight/buffer.frameCount());
}

// what to do when fft spectrum is available
void MainWindow::spectrumAvailable(QVector<double> spectrum){
  // just tell the spectrum
  // the visualization widget will catch the signal...

    //Mode 1
    if(ui->radioButton_1->isChecked()){
        for(unsigned int i=0; i<(d->getM()); i=i+1){
            if(spectrum[i]==1){
                    d->setLed(i,d->getN()-1,!ui->checkBox_inverted->checkState());
            }
        }
    }

    //Mode 2
    else if(ui->radioButton_2->isChecked()){
        unsigned int count=0;
        for(unsigned int i=0; i<(d->getM()); i=i+1){
            if(spectrum[i]==1){
                count++;
            }
        }
        if(ui->checkBox_Mirror->checkState()){
            for(unsigned int i=d->getM()-1; i>=d->getM()-count; i--){
                d->setLed(i,d->getN()-1,!ui->checkBox_inverted->checkState());
            }
        }
        else{
            for(unsigned int i=0; i<count; i=i+1){
                d->setLed(i,d->getN()-1,!ui->checkBox_inverted->checkState());
            }
        }
    }

    //Mode 3
    else if(ui->radioButton_3->isChecked()){
        unsigned int count=0;
        float sum=0;
        for(unsigned int i=0; i<(d->getM()); i=i+1){
            sum=sum+spectrum[i];
        }
        count=sum;
        if(ui->checkBox_Mirror->checkState()){
            for(unsigned int i=d->getM()-1; i>=d->getM()-count; i--){
                d->setLed(i,d->getN()-1,!ui->checkBox_inverted->checkState());
            }
        }
        else{
            for(unsigned int i=0; i<count; i=i+1){
                d->setLed(i,d->getN()-1,!ui->checkBox_inverted->checkState());
            }
        }
    }


    //Mode 4
    else if(ui->radioButton_4->isChecked()){
        bool beatFound=false;
        float specLow=spectrum[0]+spectrum[1]+spectrum[2];
        if(specLow<1){
            lowDetected=true;
        }
        else if(specLow>2){
            beatFound=true;
        }

        if(lowDetected && beatFound){
                for(unsigned int i=0; i<d->getM(); i=i+1){
                    d->setLed(i,d->getN()-1,!ui->checkBox_inverted->checkState());
                }
                lowDetected=false;
        }
    }

}

// destructor... clear all mess
MainWindow::~MainWindow(){
  //stops the player
  player->stop();

  // wait for the calculator to stop
  delete calculator;

  // finish the ui
  delete ui;

  //Close serial communication
  serial.close();
}

// load a new media selected by the user
void MainWindow::loadMedia(){
  // mime database to detect file type
  QMimeDatabase db;

  // the mime type (to test if it is an audio file
  QMimeType type;

  // file list to be inserted into playlist
  QStringList filelist;

  // audio file to be opened
  QFileDialog d;
  filelist = d.getOpenFileNames(this,tr("Open File"),
                                "/home",
                                tr("Audio (*.wav *.mp3 *.ogg *.flac)"));

  // retrieve mime type
  for(QList<QString>::const_iterator it=filelist.begin(); it!= filelist.end(); it++){
    type = db.mimeTypeForFile(*it);
    // test if the file is an audio file
    // if yes, send it to the playlist
    if(type.name().startsWith("audio")){
      playlist->addMedia(QUrl::fromLocalFile(*it));
    }
  }
}

//Connect to USB port
void MainWindow::ledConnect()
{

    qDebug() << "Number of serial ports:" << QSerialPortInfo::availablePorts().count();

        foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {

            qDebug() << "\nPort:" << serialPortInfo.portName();
            qDebug() << "Location:" << serialPortInfo.systemLocation();
            qDebug() << "Description:" << serialPortInfo.description();
            qDebug() << "Manufacturer:" << serialPortInfo.manufacturer();
            qDebug() << "Vendor Identifier:" << (serialPortInfo.hasVendorIdentifier() ? QByteArray::number(serialPortInfo.vendorIdentifier(), 16) : QByteArray());
            qDebug() << "Product Identifier:" << (serialPortInfo.hasProductIdentifier() ? QByteArray::number(serialPortInfo.productIdentifier(), 16) : QByteArray());
            qDebug() << "Busy:" << (serialPortInfo.isBusy() ? QObject::tr("Yes") : QObject::tr("No"));

            QSerialPort *port = new QSerialPort(serialPortInfo);
            if (port->open(QIODevice::ReadWrite)) {
                qDebug() << "Baud rate:" << port->baudRate();
                qDebug() << "Data bits:" << port->dataBits();
                qDebug() << "Stop bits:" << port->stopBits();
                qDebug() << "Parity:" << port->parity();
                qDebug() << "Flow control:" << port->flowControl();
                qDebug() << "Read buffer size:" << port->readBufferSize();
                port->close();
            } else {
                qDebug() << "Unable to open port, error code" << port->error();
            }
            delete port;
        }

        /*Port: "ttyUSB0"
        Location: "/dev/ttyUSB0"
        Description: "USB-Serial Controller"
        Manufacturer: "Prolific Technology Inc."
        Vendor Identifier: "67b"
        Product Identifier: "2303"
        Busy: "No"
        Baud rate: 9600
        Data bits: 8
        Stop bits: 1
        Parity: 0
        Flow control: 0
        Read buffer size: 0
  */
    //serial.setPortName("/dev/ttyUSB0");
    serial.setPortName("/dev/ttyACM0");
    serial.setBaudRate(QSerialPort::Baud9600);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    serial.open(QIODevice::ReadWrite);
    if(!serial.isOpen()){
        QMessageBox messageBox;
        messageBox.information(0,"Sorry!","No Led display is connected.");
        messageBox.setFixedSize(500,200);
    }
}

//Prepare and send data to LED Display
void MainWindow::ledSendData()
{
    QByteArray package;

    //Clean package
    for(int i=0; i<35;i++){
        package[i]=0x00;
    }

    //First characters are here to align
    package[0]=0xAB;
    package[1]=0xCD;
    package[2]=0xEF;


    //Prepare package
    for(int j=0;j<32;j++){
        for(int i=0; i<8;i++){
            if(d->getState(i,j)){
                package[j+3]=package[j+3]|(1<<i);
            }
        }
    }

    //Send package
    serial.write(package);

}

// play the previous song
void MainWindow::prev(){
    playlist->previous();
    // adjust the current music playing on listview
    ui->listViewPlaylist->setCurrentIndex(playlistModel->index(playlist->currentIndex(), 0));

}

// play next song
void MainWindow::next(){
    playlist->next();
    // adjust the current music playing on listview
    ui->listViewPlaylist->setCurrentIndex(playlistModel->index(playlist->currentIndex(), 0));
}

// forward/rewind the song within mainwindow
// user may have multiple controls
// this will help to keep controls coherent
void MainWindow::setMediaAt(qint32 percent){
  if(percent < 0){
    percent = 0;
  }
  if(percent > 100){
    percent = 100;
  }
  emit positionChanged(percent*player->duration()/100);
}

// user selected a new position in the song
void MainWindow::slotPositionChanged(qint64 e){
  Q_UNUSED(e);
  ui->control->onElapsedChanged(100*e/player->duration());
}

// new song arriving
void MainWindow::mediaStatusChanged(QMediaPlayer::MediaStatus status){
  Q_UNUSED(status);
  ui->control->onDurationChanged(player->duration());
}

// this is for windows compilations
// display the song info
void MainWindow::metaDataAvailableChanged(bool flag){
  Q_UNUSED(flag);
  /*
  qDebug() << "metadata";
  ui->widgetInfo->setAtribute("AlbumArtist",player->metaData("AlbumArtist").toString());
  ui->widgetInfo->setAtribute("Title",player->metaData("Title").toString());
  ui->widgetInfo->setAtribute("AlbumTitle",player->metaData("AlbumTitle").toString());
  ui->widgetInfo->setAtribute("TrackNumber",player->metaData("TrackNumber").toString());
  ui->widgetInfo->setAtribute("AudioBitRate",player->metaData("AudioBitRate").toString());
  ui->widgetInfo->setAtribute("AudioCodec",player->metaData("AudioCodec").toString());
  */
}

// and now the linux one
// display the song info
void MainWindow::metaDataChanged(){
  if(player->isMetaDataAvailable()){
    ui->widgetInfo->setAtribute("AlbumArtist",player->metaData("AlbumArtist").toString());
    ui->widgetInfo->setAtribute("Title",player->metaData("Title").toString());
    ui->widgetInfo->setAtribute("AlbumTitle",player->metaData("AlbumTitle").toString());
    ui->widgetInfo->setAtribute("TrackNumber",player->metaData("TrackNumber").toString());
    ui->widgetInfo->setAtribute("AudioBitRate",player->metaData("AudioBitRate").toString());
    ui->widgetInfo->setAtribute("AudioCodec",player->metaData("AudioCodec").toString());
  }
  // pehraps lots of metadata may be available, only the above are passed ahead
  //  ui->widgetInfo->setAtribute(key,variant.toString());*/
}

// sets up the volume
void MainWindow::setVolume(int volume){
  player->setVolume(volume);
}

// deal with play/pause button
// no explanation needed here
void MainWindow::playPause(){
  if(player->state() == (QMediaPlayer::PausedState|QMediaPlayer::StoppedState))
    player->play();
  else
    player->pause();
}


void MainWindow::timeisOver()
{


    //Shift left
    for (unsigned int j=0;j<(d->getN()-1);j++){
        for(unsigned int i=0; i<(d->getM()); i++){
            d->setLed(i,j,d->getState(i, j+1));
        }
    }

    //Clean last column
    if(ui->checkBox_inverted->checkState()){
        for(unsigned int i=0; i<(d->getM()); i++){
            d->setLed(i,d->getN()-1,true);
        }
    }
    else{
        for(unsigned int i=0; i<(d->getM()); i++){
            d->setLed(i,d->getN()-1,false);
        }
    }

    //Output to screen
    if(serial.isOpen()){
        MainWindow::ledSendData();
    }


}
