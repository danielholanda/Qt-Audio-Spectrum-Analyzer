#include "controls.h"
#include "ui_controls.h"
#include <QMouseEvent>
#include <QDebug>

// controls constructor
// starts up all stuff
Controls::Controls(QWidget *parent):
  AbstractControl(parent), ui(new Ui::Controls){
  ui->setupUi(this);
  // user did not selected music position yet
  sliderPressed = false;
  setParent(parent);
  // when the user releases the slider, tells qt a
  // new position was chosen
  connect(ui->horizontalSliderPosition,SIGNAL(sliderReleased()),
          this,SLOT(onSliderReleased()));

  // duration records total music time to be played
  duration=1;
  ui->horizontalSliderPosition->setDisabled(true);
}

// put the trash on the trash
Controls::~Controls(){
  delete ui;
}

// deals with slider release events
void Controls::onSliderReleased(){
  // when the user releases the slider, the component tells qt
  // the new position in percentual values
  emit elapsedSelected(ui->horizontalSliderPosition->value()*duration/100);
}

// the folowing method is self-explained
void Controls::onPlayPauseClicked(){
  emit playPause();
}

// the folowing method is self-explained
void Controls::onPrevClicked(){
  emit prev();
}

// the folowing method is self-explained
void Controls::onNextClicked(){
  emit next();
}

// tells current volume
void Controls::onVolumeChanged(int value){
  // if slider is not down it means someone else
  // is selecting the current volume
  if(!ui->dialVolume->isSliderDown()){
    ui->dialVolume->setValue(value);
  }
  else{
    // if slider is down the user is
    // selecting the volume in this component
    emit volumeSelected(value);
  }
}

// music duration has changed (current media has changed)
void Controls::onDurationChanged(qint64 value){
  ui->horizontalSliderPosition->setEnabled(true);
  duration = value;
  // display the new duration in lcdnumber
  ui->lcdNumberDuration->display(QTime(0,0).addMSecs(value).toString(QString("hh:mm:ss")));
}

// elapsed time has changed
void Controls::onElapsedChanged(qint64 value){
  // adjust the elapsed time on lcdnumber
  ui->lcdNumberElapsed->display(QTime(0,0).addMSecs(value).toString(QString("hh:mm:ss")));
  // if slider is not down, it means someone else is changing
  // elapsed time
  if(!ui->horizontalSliderPosition->isSliderDown()){
    // position slider in the new elapsed time
    ui->horizontalSliderPosition->setValue(100*value/duration);
  }
}

