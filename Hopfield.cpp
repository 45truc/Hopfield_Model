#include "clases_encabezados.h"

using namespace std;


//Variables Globales
#define D_MAX 127
#define mu_MAX 8
double Temp,pasos=100, w[D_MAX*D_MAX][D_MAX*D_MAX], theta[D_MAX*D_MAX], a[mu_MAX];


//Funciones
double Probabilidad_T(Neuron S[], double T, unsigned int index, double w[][D_MAX*D_MAX], double theta[], unsigned int N);
double Delta_H(Neuron S[], unsigned int index, double w[][D_MAX*D_MAX], double theta[], unsigned int N);
void omega_theta(Neuron Patron[][D_MAX*D_MAX], unsigned int N, unsigned int P, double w[][D_MAX*D_MAX], double theta[], double a[]);
double Solapamiento(Neuron S[], Neuron Patron[][D_MAX*D_MAX], int mu, double a[], unsigned int P, unsigned int N);
void menu(Neuron S[], Neuron Patron[][D_MAX*D_MAX], unsigned int N);
void Deformador(Neuron S[], Neuron Patron[][D_MAX*D_MAX], int N, int mu, int def);

int main()
{   
    //Variables necesarias para el algoritmo
    unsigned short int uno, on;
    unsigned int cont,index, P ,N;
    Neuron S[D_MAX*D_MAX], Patron[mu_MAX][D_MAX*D_MAX];
    double media=0;
    
    //Lectura de datos desde fichero y salida tb
    gnuplot p,q,b;
    ifstream input;
    ofstream output, estado_final,solp, inicial;
    input.open("datos.dat");
    output.open("resultados.dat");
    solp.open("solapamiento.dat");
    input >> N;
    input >> Temp;
    input >> pasos;
    input >> P;
    input.close();
    input.open("patrones_de_entrada.dat");
    for(int mu=0;mu<P;mu++)
    {
        for(int j=0;j<N*N;j++)
        {
            input >> on;
            if(on==1) Patron[mu][j].Triggered=true;  
        }
    }
    input.close();

    
    
    

    //Deformamos un patron como condicion inicial y lo mostramos
    Deformador( S, Patron, N, 1, 30);
    gnuplot pic;
    inicial.open("estado_inicial.dat");
    for(int j=0; j<N*N;j++)
    {
        inicial << S[N*N-j-1].num() << "\t";
        if((j+1)%N==0) inicial << endl;
    }
    inicial.close();
    pic("set palette grey \n set xra[-0.5:121.5]\n set yra[-0.5:121.5] \n plot 'estado_inicial.dat' matrix with image"); 

    
     

/*
//Partimos del estado aleatorio
    for(int j=0;j<N*N;j++)
        S[j].Ruido();
    
    //Menu
    //menu( S, Patron, N);
*/



    //Aleatoriedad
    random_device rd;  //Will be used to obtain a seed for the random mumber engine
    mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    uniform_real_distribution<> phi(0,1);
    uniform_int_distribution<> pos(0,N*N-1);

    //Algoritmo en si
    omega_theta(Patron, N, P, w, theta, a);
    for(int i=0; i<pow(N,2)*pasos;i++)
    {
        //Posición aleatoria en el vector
        index=pos(gen);
        
        //Generar la probabilidad y Permitir transición
        if(phi(gen)<=Probabilidad_T(S, Temp, index, w, theta, N))
        {
            S[index].Triggered=!S[index].Triggered;
            cont++;

            //Salida de datos cada vez que ocurrre un número relevante de cambios    
            if(cont>(0.01*pow(N,2)))
            {
                for(int j=0; j<N*N;j++)
                {
                    output << S[N*N-j-1].num() << "\t";
                    if((j+1)%N==0) output << endl;
                }
                output << endl << endl;
                cont=0;
            }
        }

        if(i%(N*N)==0)
        {
            solp << i*1.0/(N*N);
            for(int k=0; k<P; k++)
                solp << "\t"<< setw(14) << setprecision(14) << Solapamiento(S,Patron, k, a, P, N);
            
            solp << endl;
            if(i*1.0/(N*N)>=15)
                media+=Solapamiento(S,Patron, 1, a, P, N);
        }
    }
    //La última iteración es le último paso pero para registrarlo correctamente hay que haerlo fuera
    solp << pasos;
    for(int k=0; k<P; k++)
        solp << "\t"<< setw(14) << setprecision(14) << Solapamiento(S,Patron, k, a, P, N);

    media+=Solapamiento(S,Patron, 0, a, P, N);
    cout << endl << "La media a partir del paso 15 para el patrón 0 es de: " << setw(14) << setprecision(14) << media/6 << endl <<endl ;
    solp.close();
    output.close();

    //Grafica del estado final
    estado_final.open("final.dat");
    for(int j=0; j<N*N;j++)
    {
       estado_final << S[N*N-j-1].num() << "\t";
        if((j+1)%N==0) estado_final << endl;
    }
    estado_final.close();

    string s; 
    s="set yrange[-1:1] \n set xlabel 'Número de pasos montecarlo' \n set ylabel 'solapamiento'\n plot 'solapamiento.dat' w l t 'solapamiento patrón 1'";
    for(int i=2; i<=P; i++)
        s+=", 'solapamiento.dat' u 1:"+to_string(i+1)+" w l t 'solapamiento patrón "+to_string(i)+"'";

    p("set palette grey \n set xra[-0.5:121.5]\n set yra[-0.5:121.5] \n plot 'final.dat' matrix with image");
    //b("set yrange[-1:1] \n set xlabel 'Número de pasos montecarlo' \n set ylabel 'solapamiento'\n plot 'solapamiento.dat' w l t 'solapamiento patrón 1', 'solapamiento.dat' u 1:3 w l t 'solapamiento patrón 2', 'solapamiento.dat' u 1:4 w l t 'solapamiento patrón 3'");
    b(s);
    
    //Pequeño menú de animación
    cout << endl <<"Simulación terminada, introduzca 1 para reprodurcirla: ";
    cin >> uno;
    if(uno==1)
    q("load 'elgif.plot'");
    
    return 0;    
} 


//Definición de T, donde primero va el puntero, temperatura, fila, columna y dimensión
double Probabilidad_T(Neuron S[], double T, unsigned int index, double w[][D_MAX*D_MAX], double theta[], unsigned int N)
{
    double Exponencial;

    Exponencial=exp(-Delta_H( S, index, w, theta, N)/T);
    return (Exponencial<1) ? Exponencial:1.0;
}

//Calculo del peso, función de los patrones 
void omega_theta(Neuron Patron[][D_MAX*D_MAX], unsigned int N, unsigned int P, double w[][D_MAX*D_MAX], double theta[], double a[])
{
    double sum=0;
    //Cálculo de a
    for(int mu=0;mu<P; mu++)
    {
        for(int i=0; i<N*N; i++)
        {
            sum+=Patron[mu][i].num();
        }
        a[mu]=sum/(N*N);
    }

    //Cálculo de las w, optimizado porque hay simetría
    for(int i=0; i<N*N-1; i++)
    {
        for(int j=i+1; j<N*N; j++) //i nunca es igual a j, y por el constructor de la variable double los que son iguales se anulan por defecto
        {
            for(int mu=0; mu<P; mu++)
            {
                w[i][j]+=(Patron[mu][i].num()-a[mu])*(Patron[mu][j].num()-a[mu]);
            }
            w[i][j]=w[i][j]/(N*N);
            w[j][i]=w[i][j]; //Simetría
        }
    }

    //Cálculo de las theta
    for(int i=0; i<N*N; i++)
    {
        sum=0;
        for(int j=0;j<N*N; j++)
        {
            sum+=w[i][j];
        }
        theta[i]=sum/2;
    }

    return;
}

//Delta de H calculado de forma óptima
double Delta_H(Neuron S[], unsigned int index, double w[][D_MAX*D_MAX], double theta[], unsigned int N)
{
    double DH=theta[index];

    for(int i=0;i<N*N;i++)
    {
        DH+=-w[i][index]*S[i].num();
    }

    return S[index].Triggered ? -DH:DH;
}

//Solapamiento del patrón mu
double Solapamiento(Neuron S[], Neuron Patron[][D_MAX*D_MAX], int mu, double a[], unsigned int P, unsigned int N)
{
    double m=0;
    for(int i=0; i<N*N; i++)
    {
        m+=(Patron[mu][i].num()-a[mu])*(S[i].num()-a[mu]);
    }

    m=m/(N*N*a[mu]*(1-a[mu]));
    return m;
}

void menu(Neuron S[], Neuron Patron[][D_MAX*D_MAX], unsigned int N)
{
    unsigned int o;
    cout << "Introduzca el número de patrón base para la evolución: ";
    cin >> o;
    
    for(int i=0; i<N*N; i++)
    {
        S[i].Triggered=Patron[o][i].Triggered;
    }

    return;
}

//Para partir de una patron deformado se seleciona el ptron y se deforma un porcentaje
void Deformador(Neuron S[], Neuron Patron[][D_MAX*D_MAX], int N, int mu, int def)
{
    //Aleatoriedad
    random_device rd;  //Will be used to obtain a seed for the random mumber engine
    mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    uniform_int_distribution<> pos(0,N*N-1);

    for(int j=0;j<N*N;j++)
    {
        S[j].Triggered=Patron[mu][j].Triggered;  
    }

    int x;
    for(int i=1; i<N*N*def*0.01; i++)
    {
        //Posición aleatoria en el vector
        x=pos(gen);
        S[x].Triggered=!S[x].Triggered;
    }

    return;
}