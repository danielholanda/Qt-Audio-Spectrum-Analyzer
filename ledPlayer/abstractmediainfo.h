#ifndef ABSTRACTMEDIAINFO_H
#define ABSTRACTMEDIAINFO_H

#include <QWidget>
#include <QString>
// class to display media info
class AbstractMediaInfo : public QWidget{
  Q_OBJECT
public:
  explicit AbstractMediaInfo(QWidget *parent = 0){Q_UNUSED(parent);}

public slots:
  // set attributes to be displayed
    virtual void setAtribute(QString property, QString value)=0;
};

#endif // ABSTRACTMEDIAINFO_H
