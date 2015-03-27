#ifndef ABSTRACTCONTROL_H
#define ABSTRACTCONTROL_H

#include <QWidget>

// this is the abstract class to implement audio controls
class AbstractControl : public QWidget{
private:
  Q_OBJECT
public:
  // constructor does nothing
  explicit AbstractControl(QWidget *parent = 0){Q_UNUSED(parent)}

public slots:
  // listen when mainwindow tells user pressed play/pause button
  virtual void onPlayPauseClicked(void)=0;

  // listen when mainwindow tells user pressed prev button
  virtual void onPrevClicked(void)=0;

  // listen when mainwindow tells user pressed next button
  virtual void onNextClicked(void)=0;

  // someone changed the volume somewhere
  virtual void onVolumeChanged(int)=0;

  // elapsed time has changed somewhere
  virtual void onElapsedChanged(qint64)=0;

 // void myElapsedChanged(int)=0;
  virtual void onDurationChanged(qint64)=0;

signals:
  // tells when user pressed play/pause button
  void playPause();

  // tells when user pressed next button
  void next();

  // tells when user pressed prevp button
  void prev();

  // tells when user pressed stop button
  void stop();

  // tells when user changed volume
  void volumeSelected(int);

  // tells when user change music position
  void elapsedSelected(qint64);
};

#endif // ABSTRACTCONTROL_H
