#ifndef MEDIAINFO_H
#define MEDIAINFO_H
#include "abstractmediainfo.h"

#include <QWidget>
#include <QString>
#include <QStandardItemModel>
#include <QStandardItem>

namespace Ui {
class MediaInfo;
}

class MediaInfo : public AbstractMediaInfo
{
  Q_OBJECT

public:
  explicit MediaInfo(QWidget *parent = 0);
  ~MediaInfo();

public slots:
  void setAtribute(QString property, QString value);
private slots:
  void on_pushButton_clicked();

private:
  Ui::MediaInfo *ui;
  QString artist, title, album, bitrate, codec, track;
};

#endif // MEDIAINFO_H
