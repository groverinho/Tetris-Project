#include <cstdlib>
#include <ctime>
#include "miniwin.h"
#include <windows.h>
#include <sstream>
#include <string>
//developed by
//Grover Ariel Irusta Miranda

using namespace std;
using namespace miniwin;
const int TAM =25;
const int FILAS = 20;
const int COLUMNAS =10;
struct Coord {int x,y;};
struct Pieza
{
    Coord orig;
    Coord perif[3];
    int color;
    Coord posicion(int n)const; // pos 0 - bloqe central //  pos 1,2,3 bloq periferico
};
Coord Pieza::posicion(int n)const
{
    Coord ret = {orig.x , orig.y}; //bloqe orign
    if(n!=0)
    {
        ret.x += perif[n-1].x;      //bloque perifek
        ret.y += perif[n-1].y;
    }
    return ret;
}
typedef int Tablero[COLUMNAS][FILAS]; //pantalla
void cuadrado(int x, int y) //partes de la pieza
{
        rectangulo_lleno(20+ 1+x*TAM,
                         20+ 1+y*TAM,
                         20+ x*TAM+TAM,
                         20+ y*TAM+TAM);
}
void pinta_pieza(const Pieza& P)
{
    color(P.color);
    for(int i=0; i<4; i++)
    {
       Coord c = P.posicion(i);
       cuadrado(c.x,c.y);
    }
}
Coord rota_derecha(const Coord& c)
{
    Coord ret = {-c.y, c.x};
    return ret;
}
void rota_derecha(Pieza& P)
{
    for(int i=0; i<3; i++)
        P.perif[i]= rota_derecha(P.perif[i]);
}
void tablero_vacia(Tablero& T)
{
    for(int i=0; i<COLUMNAS;i++)
         for(int j=0; j<FILAS;j++)
            T[i][j] = NEGRO;
}
void tablero_pinta(const Tablero& T)
{
    for(int i=0; i<COLUMNAS;i++)
         for(int j=0; j<FILAS;j++)
         {
            color(T[i][j]);
            cuadrado(i,j);
         }
}
void tablero_incrusta_pieza(Tablero& T, const Pieza& P)
{
    for(int i=0; i<4; i++)
    {
        Coord c = P.posicion(i);
        T[c.x][c.y] =P.color;
    }
}
bool tablero_colision(const Tablero& T, const Pieza& P)
{
    for(int i=0; i<4;i++)
    {
        Coord c = P.posicion(i);
        if(c.x < 0 || c.x >= COLUMNAS)
            return true;
        if(c.y < 0 || c.y >= FILAS)
          return true;
        if(T[c.x][c.y] != NEGRO)
           return true;
    }
    return false;
}
const Coord perifs[7][3]=
{
  {{1,0},{0,1},{1,1}},  //cuadrado
  {{1,0},{-1,1},{0,1}},  //s
  {{0,1},{1,1},{-1,0}},  //s alreves
  {{0,1},{0,-1},{1,1}},  //L
  {{0,1},{0,-1},{-1,1}},  //L alreves
  {{-1,0},{1,0},{0,1}},  //T
  {{0,1},{0,-1},{0,2}},  // palo

  /*{{-1,1},{1,1},{0,-1}},  // nueva
  {{-1,1},{1,1},{0,-1}},  // nueva
  {{-1,1},{1,1},{0,-1}},  // nueva
  {{-1,1},{1,1},{0,-1}},  // nueva
  {{-1,1},{1,1},{0,-1}},  // nueva
    {{-1,1},{1,1},{0,-1}},  // nueva
  {{-1,1},{1,1},{0,-1}},  //nueva*/

};
void pieza_nueva(Pieza& P)
{
    P.orig.x=12;
    P.orig.y=2;
    P.color = 1 + rand() % 6;
    //pieza al azar
    int r = rand() % 7;
    for(int i=0; i <3; i++)
        P.perif[i] = perifs[r][i];
}
bool tablero_fila_llena(const Tablero& T, int fila)
{
    for(int i=0; i< COLUMNAS; i++)
    {
        if(T[i][fila]==NEGRO)
            return false;
    }
    return true;
}
void tablero_colapsa(Tablero& T, int fila)
{
    //copia de abajo asia arriba
    for(int j=fila; j>0; j--)
    {
        for (int i=0; i<COLUMNAS; i++)
        {
            T[i][j] = T[i][j-1];
        }
    }
    //vacia la d arriba
    for(int i =0; i<COLUMNAS; i++)
        T[i][0] = NEGRO;
}
int tablero_cuenta_lineas(Tablero& T)
{
    int fila = FILAS-1, cont =0;
    while(fila>=0)
    {
        if(tablero_fila_llena(T,fila))
        {
            tablero_colapsa(T,fila);
            cont++;
            PlaySound("D:\\notify.wav",NULL,SND_FILENAME);
        }
        else
            fila--;
    }
    return cont;
}
string convierteString(int puntos)
{
    stringstream sout;
    sout << puntos;
    return sout.str();
}
void repinta(const Tablero& T, const Pieza& p, const Pieza& sig, int puntos, int nivel)
{
    const int ancho = TAM * COLUMNAS;
    const int alto = TAM * FILAS;
    borra();
    tablero_pinta(T);
    color_rgb(128,128,128);
    linea(20,20,20,20+alto);
    linea(20,20+alto,20+ancho,20+alto);
    linea(20+ancho,20+alto, 20+ancho,20);
    texto(40+ ancho,20,"Pieza siguiente");
    texto(40+ ancho,150,"Nivel");
    texto(40+ ancho,250,"Puntos");
    color(BLANCO);
    texto(40+ ancho,270,convierteString(puntos));
    texto(40+ ancho,170, convierteString(nivel+1));
    pinta_pieza(p);
    pinta_pieza(sig);
    if(puntos >= 3)
    {
         texto(140,240,"Felicidades gano el juego ! ");
         refresca();
        PlaySound("D:\\tonotetris.wav",NULL,SND_FILENAME);
        vcierra();
    }
    refresca();
}
const int puntos_limite[10]={ 50, 100, 150, 200, 250, 300, 350, 400, 450, 500};
const int tics_nivel[10]={ 33, 25, 20, 18, 16, 14, 12, 10, 8, 2};
void game_over()
{
    //gameover
    texto(140,240,"Game Over ! ");
    refresca();
    PlaySound("D:\\gameover.wav",NULL,SND_FILENAME);
    espera(3000);
    vcierra();
}
int main()
{
    vredimensiona(TAM*COLUMNAS +220, TAM*FILAS+100);
    srand(time(0));
    int tic=0, puntos = 0, nivel=0;
    Tablero T;
    tablero_vacia(T);
    Pieza c, sig;
    pieza_nueva(c);
    pieza_nueva(sig);

    c.orig.x=5;
    repinta(T,c, sig, puntos, nivel);
    int t = tecla();
     while(t != ESCAPE)
    {
        Pieza copia = c;
        if(t==NINGUNA && tic >tics_nivel[nivel])
        {
            tic=0;
            t=ABAJO;
        }
        if(t==int('N'))
            nivel++;
        //1prueba el movimiento
        if(t == ABAJO)
           c.orig.y++;
        else if(t==ARRIBA)
            rota_derecha(c);
        else if(t==DERECHA)
            c.orig.x++;
        else if(t==IZQUIERDA)
            c.orig.x--;
            //2mira si ay colision
        if(tablero_colision(T,c))
           {
                c = copia;
                if(t==ABAJO)
                {
                    tablero_incrusta_pieza(T,c);
                    int cont = tablero_cuenta_lineas(T);
                    puntos += cont * cont;
                    if(puntos > puntos_limite[nivel])
                        nivel++;
                    c=sig;
                    pieza_nueva(sig);
                    c.orig.x=5;
                    if(tablero_colision(T,c))
                       game_over();
                }
           }
        if(t != NINGUNA)
            repinta(T,c,sig,puntos,nivel);

            espera(30);
            tic++;
            t=tecla();
        }
    vcierra();
    return 0;
}
