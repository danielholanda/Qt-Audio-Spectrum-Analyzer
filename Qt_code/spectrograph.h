#ifndef SPECTROGRAPH_H
#define SPECTROGRAPH_H
#include "abstractspectrograph.h"

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QAudioBuffer>
#include <complex>
#include <QVector>
#include <QDebug>
#include <QTimer>
#include <QGradient>

// the maximum number of bands
#define BAND_MAX 128

// width of the bar to be displayed
#define MIN_BARWIDTH 5

// spectrograph class is used to display fourier spectrum
// bars
class Spectrograph : public AbstractSpectrograph{
  Q_OBJECT
public:
  explicit Spectrograph(QWidget *parent = 0);

signals:

public slots:
  /*!brief Paints the screen for spectrum displaing
   */
  void paintEvent(QPaintEvent *e);
  /*!brief Load samples to be displayed
  */
  void loadSamples(QVector<double> &_spectrum);
  /*!brief Used to modify spectrum while a new sample
   * does not arrive
   */
  void timerEvent(QTimerEvent *e);
  /*!brief Redefines some variables when the widget is
   * resized
   */
  void resizeEvent(QResizeEvent *e);
  /*!brief Loads left and right mean audio values
   */
  void loadLevels(double left, double right);
private:
  /*! \var QVector<int> spectrum
   * \brief used to store spectrum to be displayed
   *
   * \var QVector<int> delay
   * \brief
   */
  QVector<int> spectrum, delay;

  int leftLevel, rightLevel;
  int decayIncr;
  int NUM_BANDS;
  QLinearGradient gradient;
  QBrush gradientBrush, backgroundBrush,
  transparentBrush, decayBrush;
  QPen pen;
  float barSpacing, barWidth, widgetHeight;
  double max;
  double time;
};

#endif // SPECTROGRAM_H
