#ifndef FFTCALC_H
#define FFTCALC_H

#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QVector>
#include <QDebug>
#include <QTimer>
#include <QObject>
#include "fft.h"

// the size of fft array that is dispatched to
// mainwindow
#define SPECSIZE 1024

class BufferProcessor: public QObject
{
    Q_OBJECT
  QVector<double> array;
  QVector<double> window;
  QVector<double> spectrum;
  QVector<double> logscale;
  QTimer *timer;
  bool compressed, running, iscalc;
  int chunks, interval, pass;
  CArray complexFrame;
public slots:
    void processBuffer(QVector<double> _array, int duration);
signals:
    void calculatedSpectrum(QVector<double> spectrum);
    void allDone(void);
protected slots:
    void run();
public:
    explicit BufferProcessor(QObject *parent=0);
    ~BufferProcessor();
    void calc(QVector<double> &_array, int duration);
};

// fftcalc runs in a separate thread
class FFTCalc : public QObject{
    Q_OBJECT
private:
  bool isBusy;
  BufferProcessor processor;
  QThread processorThread;

public:
  explicit FFTCalc(QObject *parent = 0);
  ~FFTCalc();
  void calc(QVector<double> &_array, int duration);
public slots:
  void setSpectrum(QVector<double> spectrum);
  void freeCalc();
signals:
  void calculatedSpectrum(QVector<double> spectrum);
};

#endif // FFTCALC_H
