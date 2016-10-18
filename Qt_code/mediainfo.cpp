#include "mediainfo.h"
#include "ui_mediainfo.h"
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>

MediaInfo::MediaInfo(QWidget *parent) :
  AbstractMediaInfo(parent),
  ui(new Ui::MediaInfo){
  ui->setupUi(this);

  setParent(parent);
}

MediaInfo::~MediaInfo(){
  delete ui;
}

// this function just assemble a large string with all information
// that is passed. you should modify it for your needs
void MediaInfo::setAtribute(QString property, QString value){
  QString text;
  if(property == "AlbumArtist")
    artist = value;
  if(property == "Title")
    title = value;
  if(property == "AlbumTitle")
    album = value;
  if(property == "TrackNumber")
    track = value;
  if(property == "AudioBitRate")
    bitrate = value;
  if(property == "AudioCodec")
    codec = value;
  text = artist+"/"+title+"/"+
      album+"/"+track+"/"+
      bitrate+"/"+codec;

  // just display the text into the label :P
  ui->label->setText(artist);
  ui->label_2->setText(album);
}

void MediaInfo::on_pushButton_clicked()
{
    QString link = "https://play.google.com/store/search?q="+artist+"%20"+title;
    //https://play.google.com/store/search?q=uz%20trapshit
    QDesktopServices::openUrl(QUrl(link));
}
