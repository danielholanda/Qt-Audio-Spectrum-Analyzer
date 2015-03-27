#ifndef FFT_H
#define FFT_H

#include <complex>
#include <iostream>
#include <valarray>

const double PI = 3.141592653589793238460;

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

// Cooley–Tukey FFT (in-place)

void fft(CArray& x);

void ifft(CArray& x);

#endif
