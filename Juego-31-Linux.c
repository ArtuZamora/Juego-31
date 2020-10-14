/*                     Kodigo Academy 2020
                    Developed by Rafael Zamora
                     31-Game (BlackJack-ish)
             Based on the structured programming paradigm
*/
//Beginning
#pragma region Library Imports And #Defines Constants
#include <stdio.h>
#include <stdlib.h>     
#include <stdbool.h>    //Libreria que define a true-false en 1-0 respectivamente, entre otros
#include <string.h>     /*Libreria de formateo para cadenas*/
#include <time.h>       //Libreria para utilizacion de servicios hora-fecha del sistema y su reloj
#include <unistd.h>     //Libreria para la utilizacion de funcion usleep en LINUX (detiene el hilo de ejecucion), entre otros
#include <termios.h>      //Libreria para simulacion de getch()
#define TAM 12          //Cantidad de cartas
#define COL 4           //Cantidad de mazos
#define CTRL1 -1         //Bandera para verificar utilizacion de cartas
#pragma endregion

#pragma region Global Variables Declarations
    #pragma region Vectors
        //Vectores de tipo entero para contenedores de los 4 distintos mazos
        int mazoCorazones[TAM];
        int mazoPicas[TAM];
        int mazoTreboles[TAM];
        int mazoDiamantes[TAM];
        //Vectores punteros tipo char para contenedor del significado de las cartas existentes
        char *tipos[] = {"Corazones","Picas","Trebol","Diamantes"};
        char *rangos[] = {"AS","Dos","Tres","Cuatro","Cinco","Seis","Siete","Ocho","Nueve","J","Q","K"};
        /*Vectores multidemensionales de tipo entero (matrices, en este caso 12x4)
        contenedores de la mano tanto del jugador como de la computadora,
        declarada como 12x4 por la posibilidad de contener cualquier carta del mazo.
        Inicializados previamente a 0*/
        int jugador[TAM][COL]={0};
        int computadora[TAM][COL]={0};
    #pragma endregion
    //Variables contenedoras de los numeros aleatorios, rand1 aleatoriza los tipos de mazo, rand2 los rangos
    int rand1,rand2;
    int mode;       //Variable contenedora del estado actual del jugador (1 Usuario, 0 PC)
    int puntajeJugador , puntajeComputadora;
    char nombreJugador[100]; //Variable tipo char contenedora del nombre del usuario que se pide al principio
    /*Variables tipo booleanas, flag determina si la carta aleatorizada esta tomada en ese momento
    inicioJuego determina el estado actual del juego*/
    bool flag , inicioJuego;
#pragma endregion

#pragma region Procedures Declarations
    void imprimirMano();
    void LlenadoMazos();
    void repartirCartas();
    void turnoCompleto();
    void setRandom();
    void actualizarPuntaje();
    void validarPuntaje();
    void flushVariables();
    bool verificarEx();
    bool turno();
    bool terminarJuego();
    bool Reglas();
#pragma endregion

#pragma region Procedures Code
//Procedimiento que simula getch() en linux
char getch(void)
{
    char buf = 0;
    struct termios old = {0};
    fflush(stdout);
    if(tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if(tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if(read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if(tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    printf("%c\n", buf);
    return buf;
 }
//Procedimiento que imprime la mano actual del jugador que se especifique como parametro
void imprimirMano(int manoJugador[TAM][COL])
{
    for(int i=0;i<COL;i++)
    {
        for(int j=0;j<TAM;j++)
        {
            if(manoJugador[j][i] != 0)
            {
                printf("\n --> %s de %s",rangos[j],tipos[i]);
            }
        }
    }
}
/*Procedimiento que ejecuta el turno completo necesario para un jugador que se especifique como parametro
el segundo parametro actualiza la variable global mode, para saber que turno se ejecuta*/
void turnoCompleto(int array[TAM][COL], int player)
{
            mode=player;

            int times;                  //Cartas a repartir
            if (inicioJuego) times=3;   //Repartir 3 si es el inicio del juego
            else times=1;               //Repartir 1 si no es el inicio del juego

            for(int i=1;i<=times;i++)
            {
                do{
                    flag=turno();
                } while (flag); /*Mientras la carta aleatorizada ya este utilizada, repetir hasta que 
                                  se aleatorice una carta no utilizada*/
                repartirCartas(array);
            }
}
/*Procedimiento de reinicio de los datos de los mazos, es decir regresar las cartas utilizadas a sus mazos
    Se lleanan con los valores de 1 hasta 13*/
void LlenadoMazos()
{
        for(int i=0;i<TAM;i++)
        {
        mazoCorazones[i]=i+1;
        mazoPicas[i]=i+1;
        mazoTreboles[i]=i+1;
        mazoDiamantes[i]=i+1;
        }
}
//Aleatorizacion de los numeros
void setRandom()
{
    /*Sintaxis para aleatorizar entre min-max:
        int num_aleatorio = (rand() % (max_value - min_value + 1)) + min_value;
    */
    rand1 = (rand() %  (COL - 1 + 1)) + 1;
    rand2 = (rand() %  (TAM - 1 + 1)) + 1;
}
//Procedmiento para la reparticion de las cartas recibiendo como parametro el vector correspondiende al jugador
void repartirCartas(int array[TAM][COL])
{
    if(rand2==1) //Si la carta repartida es un AS
    {
        if(mode==1) //Es jugador
        {
            //Preguntar al jugador que valor le quiere dar a su AS
            int valor;
            printf("\nHas obtenido un AS, qu%c valor le quieres dar?",130);
            printf("\n-->Esta es tu mano actual antes de darle un valor al AS<--\n");
            imprimirMano(jugador);
            printf("\n\nSu puntaje actual: %d",puntajeJugador);
            printf("\n\nIngrese 1 u 11: ");
            //Validacion de tipo de dato e informacion de dato
            while (!scanf("%i",&valor)) //Scanf devuelve 0 si la conversion es incorrecta
            {
                printf("\n\nTiene que ingresar 1 u 11, no otra cosa, si no marca error esto:v");
                printf("\nIngrese de nuevo, pero BIEN, %s: ",nombreJugador);
                while((valor = getchar()) != '\n' && valor != EOF); //Limpiar Buffer
            }
            while (!(valor ==1 || valor == 11))
            {
                printf("\n\nTiene que ingresar 1 u 11, no otra cosa, si no marca error esto:v");
                printf("\nIngrese de nuevo, pero BIEN, %s: ",nombreJugador);

                while (!scanf("%i",&valor))
                {
                    printf("\n\nTiene que ingresar 1 u 11, no otra cosa, si no marca error esto:v");
                    printf("\nIngrese de nuevo, pero BIEN, %s: ",nombreJugador);
                    while((valor = getchar()) != '\n' && valor != EOF);
                }
            }
            //Otorgar valor elegido
                if(valor==11)
                {
                    array[rand2-1][rand1-1]=valor;  
                }
                else
                {
                    array[rand2-1][rand1-1]=rand2;  
                }
        }
        else    //Es Computadora
        {
            array[rand2-1][rand1-1]=11;  //otorgar un valor de 11
        }
    }
    //Si la carta es una J, una Q o una K, otorgar valor de 10
    else if(rand2==10 || rand2==11 || rand2==12)
    {
        array[rand2-1][rand1-1]=10; 
    }
    //Si es de otro tipo, otrogar el valor contendor del numero aleatorizado
    else
    {
        array[rand2-1][rand1-1]=rand2;   
    }

    actualizarPuntaje(array); //Actualizar puntaje para jugador o para computadora

    switch (rand1) //Cartas utilizadas en la posicion dada dar valor de -1
    {
        case 1:
            mazoCorazones[(rand2-1)] = -1;        
            break;
        case 2:
            mazoPicas[(rand2-1)] = -1;      
            break;
        case 3:
            mazoTreboles[(rand2-1)] = -1;      
            break;
        default:
            mazoDiamantes[(rand2-1)] = -1;      
            break;
    }

}
//Procedimiento que reinicia el valor de las variables
void flushVariables()
{
        puntajeJugador = 0;
        puntajeComputadora=0;
        inicioJuego=true;
        LlenadoMazos();
        for(int i=0;i<TAM;i++)
        {
            for(int j=0;j<COL;j++)
            {
                jugador[i][j] = 0;
                computadora[i][j] = 0;
            }
        }
}
void actualizarPuntaje(int arrayJugador[TAM][COL])
{
    if(mode==1)
    {
        puntajeJugador+=arrayJugador[rand2-1][rand1-1];
    }
    else
    {
        puntajeComputadora+=arrayJugador[rand2-1][rand1-1]; 
    }
}
//Funcion tipo booleana que muestra al jugador las reglas y pide si quiere ingresar al juego
bool Reglas()
{
    char resp[1];
    system("clear"); //Procedimiento para limpiar la consola
    printf("\t++++++++++++Bienvenido al Juego 31++++++++++++\n"
           "\t++++++++Reglas e indicaciones del juego+++++++\n\n"
               "- El numero total de cartas es de 48 (sin comodines), 4 mazos de 12 cartas. \n\n"
               "- Las cartas existentes son "
        "8 cartas con sus respectivos valores del 2 al 9; \n    seguido de las cartas especiales: J, Q, K y AS.\n\n"
"- Las cartas J, Q y K tienen un valor de 10.\n\n"
"- El AS es una carta especial que puede tener 2 valores: 1 u 11.\n  Esto dependera del valor que le"
"  convenga al jugador.\n\n"
"- Para la computadora, el AS siempre tendrá un valor de 11.\n\n"
"- El juego se hara por turnos. Primero sera el jugador en sumar puntos por sus cartas.\n\n"
"- El jugador decide si quiere o no una carta mas de las actuales disponibles. \n  Por lo tanto, el juego"
"  le preguntara si decide quedarse con las cartas actuales luego de entregar una nueva.\n"
"  Mientras no alcance un valor mayor o igual a 31, podra seguir hasta indicar que pase el turno.\n\n"
"- El jugador que tengan el mayor puntaje de sus cartas gana la partida. \n\n- El jugador que exceda el valor"
"  31 pierde directamente el juego.\n\n"
"- Cuando el jugador se ha quedado sin turnos, es decir, se ha quedado con las cartas actuales,\n"
"   el juego le indicara quien fue el ganador.\n\n"
               "Ingrese Y para jugar o cualquier otra cosa para salir: ");
               scanf("%1s",resp);
               //strcmp devuelve 0 si encuentra coincidencias en la comparacion
               if(strcmp(resp, "y") == 0 || strcmp(resp, "Y") == 0) 
               {
                   printf("\nIngrese su nombre: ");
                   scanf("%s",nombreJugador);
                   return true;
               }
               else
               {
                   return false;
               }
               
}
/*Funcion tipo booleana si la carta a verificar en el mazo especificado, tiene el valor de -1, es decir
    que ya esta tomada*/
bool verificarEx(int mazoVerf[])
{
        if(mazoVerf[(rand2-1)] == CTRL1)
        {
            return true;
        }
        else
        {
            return false;
        }
}
//Funcion tipo booleana que invoca la aleatorizacion de numeros y verifica la existencia de cartas
bool turno()
{
    setRandom();
    switch (rand1)
    {
        case 1:        
            return verificarEx(mazoCorazones);
        case 2:
            return verificarEx(mazoPicas);
        case 3:
            return verificarEx(mazoTreboles);
        default:
            return verificarEx(mazoDiamantes);
    }
}
//Funcion tipo booleana que pregunta al usuario si desea continuar o quedarse
bool terminarJuego()
{
    char resp[1];
    printf("\n\nIngrese C para pedir mas cartas");
    printf("\nIngrese Q para quedarse con las cartas que tiene\n");
    scanf("%1s",resp); //Sin validar
        if(strcmp(resp, "Q") == 0 || strcmp(resp, "q") == 0)
        {
            return true;
        }
        else if(strcmp(resp, "C") == 0 || strcmp(resp, "c") == 0)
        {
            return false;
        }
}
#pragma endregion

#pragma region Main Code
    int main (void)
    {
            char seguir[1];
            srand(time(0)); //No repetir el mismo numero al aleatorizar
            if(Reglas())
            {
                do
                {
                    system("clear");
                    printf("\n\t++++++++++++ Bienvenido, %s ++++++++++++",nombreJugador);
                    usleep(1000000);//detiene el hilo de ejecucion en nanosegundos
                    flushVariables();
                    printf("\n\t...Repartiendo sus cartas, %s...",nombreJugador);
                    usleep(2000000);
                    turnoCompleto(jugador,1);
                    //Condicionales  por si en el primer turno el jugador llega a 31
                    if(puntajeJugador == 31)
                    {
                        system("clear");
                        printf("\n +++++++++++++ Enhorabuena, %s! Has ganado en el primer turno +++++++++++++\n",nombreJugador);
                        usleep(2000000);
                        printf("\n\n--->Con estas cartas has ganado<---");
                        imprimirMano(jugador);
                        usleep(2000000);
                        printf("\nPuntaje final: %d",puntajeJugador);
                        printf("\n\nEres lo mejor, no has ni dejado que se ejecute el turno de la computadora");
                        usleep(2000000);
                    }
                    else
                    {
                        printf("\n----->Mano Actual<-----\n");
                        imprimirMano(jugador);
                        printf("\nSu puntaje actual: %d",puntajeJugador);
                        usleep(2500000);
                        printf("\n\n...Ahora es el turno de la Computadora...");
                        usleep(2500000);
                        turnoCompleto(computadora,0);

                        //Condicional por si la computadora llega a 31 en el primer turno
                        if(puntajeComputadora == 31)
                        {
                            printf("\n++++++++++++++Sad time , %s:c ! Has perdido en el primer turno++++++++++++++\n",nombreJugador);  
                            usleep(2000000);
                            printf("\n-->Con esta mano ha perdido<--\n");
                            imprimirMano(jugador);
                            usleep(2000000);
                            printf("\nCon este puntaje ha perdido: %d",puntajeJugador);
                            usleep(2000000);
                            printf("\n\n-->Con esta mano la computadora le ha ganado<--\n");
                            usleep(2000000);
                            imprimirMano(computadora);
                            usleep(2000000);
                            printf("\nCon este puntaje la computadora ha ganado: %d",puntajeComputadora);
                            usleep(2000000);
                            printf("\nANIMATE CAMPEON, LA PROXIMA TU PUEDES GANAR!\n");
                        }
                        else
                        {
                            inicioJuego=false;
                            
                            //Ejecutar mientras el jugador quiera mas cartas
                            while(!terminarJuego())
                            {
                                printf("\n\t...Repartiendo una carta mas, %s...",nombreJugador);
                                usleep(2000000);
                                turnoCompleto(jugador,1);
                                //Si el jugador se pasa de 31 en el puntaje, romper el subproceso
                                if(puntajeJugador >= 31) break;
                                imprimirMano(jugador);
                                printf("\nSu puntaje: %d",puntajeJugador);
                                usleep(2500000);
                                printf("\n\n...Ahora es el turno de la Computadora...");
                                usleep(2500000);
                                turnoCompleto(computadora,0);
                                //Si la computadora se pasa de 31 en el puntaje, romper el subproceso
                                if(puntajeComputadora >= 31) break;
                            } 
                            printf("\n\nJuego terminado... Imprimiendo Resultados");
                            usleep(3000000);
                            system("clear");
                            //Mensajes personalizados en cada uno de los casos
                            if(puntajeJugador == 31) printf("\nUsted ha ganado, Ehnorabuena, %s!!!!!!!",nombreJugador);
                            else if(puntajeComputadora == 31) printf("\nHa ganado la computadora, sad time, %s :c!",nombreJugador);
                            else if(puntajeJugador > 31) printf ("\n Ha perdido, se ha pasado de 31, sad time, %s :c!",nombreJugador);
                            else if(puntajeComputadora > 31) printf("\n Ha ganado, la computadora se ha pasado de 31, enhorabuena, %s!!!!!!!!",nombreJugador);
                            else if(puntajeJugador > puntajeComputadora) printf("\nHa ganado, su puntaje es mayor al de la computadora, enhorabuena, %s!!!!!!!!",nombreJugador);
                            else if(puntajeJugador < puntajeComputadora) printf("\nHa perdido, su puntaje es menor al de la computadora, sad time, %s :c!",nombreJugador);
                            else printf("Has empatado con la computdora, enhorasadtime, %s :c:!", nombreJugador);

                            printf("\n\n-----Resultados del Juego-----\n");
                            usleep(2000000);
                            printf("\nSu puntaje final, %s: %d",nombreJugador,puntajeJugador);
                            usleep(2000000);
                            printf("\n\nSu mano final (de cartas");
                            usleep(1500000);
                            printf(", jeje)");
                            imprimirMano(jugador);
                            usleep(2000000);
                            printf("\n\nPuntaje final de la Computadora (si, estaba jugando tambien): %d",puntajeComputadora);
                            imprimirMano(computadora);
                        }
                    }
                    usleep(3000000);
                    printf("\nDesea volver a jugar?");
                    printf("\nIngrese Y para volver a jugar u otra cosa para salir\n");
                    scanf("%1s",seguir);

                }while(strcmp(seguir, "y") == 0 || strcmp(seguir, "Y") == 0); //Repetir hasta que el jugador quiera salir
            }
        
        printf("\n\n---------------->Juego Finalizado<------------------");
        printf("\n------->Presione alguna tecla para finalizar<-------\n");
        getch();
        return 0;
    }
#pragma endregion
//End