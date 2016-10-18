#include "mydisplay.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QCheckBox"

myDisplay::myDisplay()
{
}



myDisplay::myDisplay(unsigned int _m, unsigned int _n, QWidget *parent)
{
    m=_m;n=_n;

    //ponteiro das linhas para o array com os elementos
    x=new QCheckBox*[m];


    //Aloca uma unica linha para guardar toda a matriz e grava o endereco que foi alocado na posicao x[0] do array
    x[0]=new QCheckBox[m*n];


    //grava posicoes intermediarias no bloco alocado para a matriz, visando chegar em cada uma das linhas
    for(unsigned int i=0; i<m;i++){
        x[i]=x[0]+i*n;
    }

    //Joga o ponto na tela no local correto
    for(unsigned int i=0; i<m;i++){
        for(unsigned int j=0; j<n;j++){
            (x[i]+j)->setParent(parent);
            (x[i]+j)->setGeometry(20+20*j,50+20*i,21,22);
        }
    }

}

//Turn the led on and off
void myDisplay::setLed(unsigned int _m,unsigned int _n, bool ledIsOn)
{
    if(_n<n and _m<m){
        (x[_m]+_n)->setChecked(ledIsOn);
    }
}

//Switch between on and off depending on the current status
void myDisplay::switchLed(unsigned int _m, unsigned int _n)
{
    if(_n<n and _m<m){
        bool ledStatus = (x[_m]+_n)->checkState();
        (x[_m]+_n)->setChecked(!ledStatus);
    }
}

void myDisplay::clean()
{
    for (uint i=0; i<m;i++){
        for(uint j=0;j<n;j++){
            (x[i]+j)->setChecked(false);
        }
    }
}

unsigned int myDisplay::getM()
{
    return m;
}

unsigned int myDisplay::getN()
{
    return n;
}

bool myDisplay::getState(unsigned int _m, unsigned int _n)
{
    return (x[_m]+_n)->checkState();
}


