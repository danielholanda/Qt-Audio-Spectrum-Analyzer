#ifndef CONTROLS_H
#define CONTROLS_H
#include "abstractcontrol.h"

#include <QTime>

namespace Ui {
class Controls;
}

class Controls : public AbstractControl{
    Q_OBJECT

public:
    explicit Controls(QWidget *parent = 0);
    ~Controls();
    
private:
    Ui::Controls *ui;
    qint64 duration;
    int sliderPressed;
public slots:
    void onPlayPauseClicked(void);
    void onPrevClicked(void);
    void onNextClicked(void);
    void onVolumeChanged(int value);
    void onElapsedChanged(qint64 value);
    void onDurationChanged(qint64 value);
protected slots:
    void onSliderReleased();
signals:
    void playPause();
    void next();
    void prev();
    void stop();
    void volumeSelected(int);
    void elapsedSelected(qint64);
};

#endif // CONTROLS_H
