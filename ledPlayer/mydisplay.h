#ifndef MYDISPLAY_H
#define MYDISPLAY_H
//#include "mainwindow.h"
#include "QCheckBox"

class myDisplay: public QWidget
{
private:
    QCheckBox **x; //ponteiro para ponteiro para array que guarda matriz
    unsigned int m,n; //Dimensoes
public:
    myDisplay();
    myDisplay(unsigned int _m,unsigned int _n, QWidget *parent);//Construtor com argumentos
    void setLed(unsigned int _m,unsigned  int _n, bool ledIsOn);
    void switchLed(unsigned int _m,unsigned  int _n);
    void clean(void);
    unsigned int getM(void);
    unsigned int getN(void);
    bool getState(unsigned int _m,unsigned  int _n);

};

#endif // MYDISPLAY_H
