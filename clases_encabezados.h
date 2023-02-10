#ifndef GNUPLOT_H
#define  GNUPLOT_H
#include <iostream>
#include <string>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <ctime> 
#include <cstdlib>
#include <random>

using namespace std;

class gnuplot {
    public:
    gnuplot();
    ~gnuplot();
    void operator () (const string & command);
    protected:
    FILE *gnuplotpipe;
};

gnuplot::gnuplot()
{
    gnuplotpipe =popen("gnuplot -persist", "w");
    if (!gnuplotpipe)
    cerr<<("Gnnuplot not found !");
}
gnuplot::~gnuplot()
{
    fprintf(gnuplotpipe, "exit\n");
    pclose(gnuplotpipe);
}
void gnuplot::operator() (const string & command )
{
    fprintf(gnuplotpipe, "%s\n" ,command.c_str());
    fflush(gnuplotpipe);
}
#endif




//////////////////////////////////////////////////////////////////
//      FUNCTION:       GenerarDoubleAleatorio                  //
//      DESCRIPTION:    Generamos un nmero aleatorio entre los  //
//                      limites dados.                          //
//                                                              //
//////////////////////////////////////////////////////////////////
random_device rd;  //Will be used to obtain a seed for the random number engine
mt19937 ge(rd()); //Standard mersenne_twister_engine seeded with rd()
double GenerarDoubleAleatorio(double inf, double sup)
{
    uniform_real_distribution<> dis(inf, sup);
    return dis(ge);
}





//////////////////////////////////////////////////////////////////
//      CLASS:       Neuron                                     //
//      DESCRIPTION:    Variable tipo bool adaptada a los       //
//                 valores posibles de una neurona para ahorrar //
//////////////////////////////////////////////////////////////////
class Neuron {
    public:
    bool Triggered;

    //Funciones internas
    void Ruido(void);
    int num(void); 
};

//Inicia la neurona a un verdadero o falso equiprobablemente
void Neuron::Ruido(void)
{
    this->Triggered=(GenerarDoubleAleatorio(0.0,1.0)<0.5);
}

//Permite la traducción a un valor numérico del spin
int Neuron::num(void)
{
    return Triggered ? 1:0;
}



