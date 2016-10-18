#include "fftcalc.h"

#undef CLAMP
#define CLAMP(a,min,max) ((a) < (min) ? (min) : (a) > (max) ? (max) : (a))

// fftcalc class is designed to treat with fft calculations
FFTCalc::FFTCalc(QObject *parent)
  :QObject(parent){

  // fftcalc is done in other thread
  // so it cannot overload the main thread
  processor.moveToThread(&processorThread);

  // qRegisterMetaType is used to register QVector<double> as the typename for QVector<double>
  // it is necessary for signal/slots events treatment.
  qRegisterMetaType< QVector<double> >("QVector<double>");

  // when the spectrum is calculated, setSpectrum function will comunicate
  // such spectrum to Qt as an emitted signal
  connect(&processor, SIGNAL(calculatedSpectrum(QVector<double>)), SLOT(setSpectrum(QVector<double>)));

  // when the processor finishes the calculation, the busy condition is changed.
  connect(&processor, SIGNAL(allDone()),SLOT(freeCalc()));

  // start the processor thread with low priority
  processorThread.start(QThread::LowestPriority);

  // initially, the processor is not occupied
  isBusy = false;
}

FFTCalc::~FFTCalc(){
  // tells the processor thread to quit
  processorThread.quit();
  // wait a bit until it finishes. I guess 10ms is enough!
  processorThread.wait(10000);
}

void FFTCalc::calc(QVector<double> &_array, int duration){
  if(isBusy)
    return;

  // processor is busy performing fft calculations...
  isBusy = true;

  // start the calcs...
  QMetaObject::invokeMethod(&processor, "processBuffer",
                            Qt::QueuedConnection, Q_ARG(QVector<double>, _array),
                            Q_ARG(int, duration));
}

void FFTCalc::setSpectrum(QVector<double> spectrum){
  // tells Qt about that a new spectrum has arrived
  emit calculatedSpectrum(spectrum);
}

void FFTCalc::freeCalc()
{
  // fftcalc is ready for new spectrum calculations
  isBusy = false;
}
/*
 * processes the buffer for fft calculation
 */

BufferProcessor::BufferProcessor(QObject *parent){
  // the pointer is not used here
  Q_UNUSED(parent);

  // this timer is used to send the pieces of spectrum
  // as they are calculated
  timer = new QTimer(this);

  // call run() to send such pieces
  connect(timer,SIGNAL(timeout()),this,SLOT(run()));

  // window functions are used to filter some undesired
  // information for fft calculation.
  window.resize(SPECSIZE);

  // the complex frame that is sent to fft function
  complexFrame.resize(SPECSIZE);

  // only half spectrum is used because of the simetry property
  spectrum.resize(SPECSIZE/2);

  // logscale is used for audio spectrum display
  logscale.resize(SPECSIZE/2+1);

  // by default, spectrum is log scaled (compressed)
  compressed = false;

  // window function (HANN)
  for(int i=0; i<SPECSIZE;i++){
    window[i] = 0.5 * (1 - cos((2*PI*i)/(SPECSIZE)));
  }

  // the log scale
  for(int i=0; i<=SPECSIZE/2; i++){
    logscale[i] = powf (SPECSIZE/2, (float) 2*i / SPECSIZE) - 0.5f;
  }

  // nothing is running yet
  running = false;

  // process buffer each 100ms (initially, of course)
  timer->start(100);
}

BufferProcessor::~BufferProcessor(){
  timer->stop();

}

void BufferProcessor::processBuffer(QVector<double> _array, int duration){
  // if the music is new, array size may change
  if(array.size() != _array.size()){
    //array is splitted into a set of small chuncks
    chunks = _array.size()/SPECSIZE;

    // resize the array to the new array size
    array.resize(_array.size());
  }
  // interval of notification depends on the duration of the sample
  interval = duration/chunks;

  // copy the array
  array = _array;

  // count the number of fft calculations until the chunk size
  // is reached
  pass = 0;

  // it cannot be so small
  if(interval < 1)
    interval = 1;

  // redefines the timer interval
  timer->start(interval);
}

void BufferProcessor::run(){
  // spectrum amplitude
  qreal amplitude;
  qreal SpectrumAnalyserMultiplier = 0.15e-3;

  // tells when all chunks has been processed
  if(pass == chunks){
    emit allDone();
    return;
  }

  // we does not calc spectra when array is too small
  if(array.size() < SPECSIZE){
    return;
  }

  // prepare complex frame for fft calculations
  for(uint i=0; i<SPECSIZE; i++){
    complexFrame[i] = Complex(window[i]*array[i+pass*SPECSIZE],0);
  }

  // do the magic
  fft(complexFrame);

  // some scaling/windowing is needed for displaying the fourier spectrum somewhere
  for(uint i=0; i<SPECSIZE/2;i++){
    amplitude = SpectrumAnalyserMultiplier*std::abs(complexFrame[i]);
    amplitude = qMax(qreal(0.0), amplitude);
    amplitude = qMin(qreal(1.0), amplitude);
    complexFrame[i] = amplitude;
  }

  // audio spectrum is usually compressed for better displaying
  if(compressed){
    for (int i = 0; i <SPECSIZE/2; i ++){
      /* sum up values in freq array between logscale[i] and logscale[i + 1],
         including fractional parts */
      int a = ceilf (logscale[i]);
      int b = floorf (logscale[i+1]);
      float sum = 0;

      if (b < a)
        sum += complexFrame[b].real()*(logscale[i+1]-logscale[i]);
      else{
        if (a > 0)
          sum += complexFrame[a-1].real()*(a-logscale[i]);
        for (; a < b; a++)
          sum += complexFrame[a].real();
        if (b < SPECSIZE/2)
          sum += complexFrame[b].real()*(logscale[i+1] - b);
      }

      /* fudge factor to make the graph have the same overall height as a
         12-band one no matter how many bands there are */
      sum *= (float) SPECSIZE/24;

      /* convert to dB */
      float val = 20*log10f (sum);

      /* scale (-DB_RANGE, 0.0) to (0.0, 1.0) */
      val = 1 + val / 40;
      spectrum[i] = CLAMP (val, 0, 1);
    }
  }
  else{
    // if not compressed, just copy the real part clamped between 0 and 1
    for(int i=0; i<SPECSIZE/2; i++){
      spectrum[i] = CLAMP(complexFrame[i].real()*100,0,1);
    }
  }
  // emit the spectrum
  emit calculatedSpectrum(spectrum);

  // count the pass
  pass++;
}


