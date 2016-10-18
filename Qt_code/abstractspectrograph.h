#ifndef ABSTRACTSPECTROGRAPH_H
#define ABSTRACTSPECTROGRAPH_H

#include <QWidget>
#include <QVector>

// the spectrum visualization widget
class AbstractSpectrograph : public QWidget{
  Q_OBJECT
public:
  explicit AbstractSpectrograph(QWidget *parent):QWidget(parent){}

signals:

public slots:
  // load the spectrum to be displayed
    virtual void loadSamples(QVector<double>&)=0;

  // load left and right mean audio levels
    virtual void loadLevels(double, double)=0;

};

#endif // ABSTRACTSPECTROGRAPH_H
