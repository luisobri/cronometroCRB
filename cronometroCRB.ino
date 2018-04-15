#include <TimerOne.h>
#include <Wire.h>
#include <MultiFuncShield.h>

int LDR;
bool haEmpezado = false;
int pasada = 0;

static byte LDR_MIN = 180;
static byte numeroVueltas = 1; // Numero de vueltas que contaremos
unsigned long vuelta[4];
unsigned long total = 0;
unsigned long masRapida = 99999;

boolean isr = false;
boolean isrC = false;

void setup()
{
  Timer1.initialize(1000); // Inicializamos Timer1 a mil microsegundos -> 1 milisegundo
  //MFS.userInterrupt = clockISR; // No lo usamos para nada
  MFS.initialize(&Timer1);
  MFS.blinkDisplay(DIGIT_ALL);
  MFS.beep(10);
  Serial.begin(115200); // Mas velocidad -> menos tiempo para escribir por el puerto serie

  // Ponemos el marcador a cero
  char time[5];
  sprintf(time, "%04d", 0);
  MFS.write(time, 1);
}

void loop()
{
  char time[5];
  unsigned long m = millis();
  comprobarPaso(m);
  if (haEmpezado) {
    // Imprimimos el tiempo
    sprintf(time, "%04d", m - vuelta[0]);
    MFS.write(time, 1);
  }
}


// Pasamos como parametro los millis para ahorrar la llamada al funcion
void comprobarPaso(unsigned long m) {
  unsigned int tmp = 0;
  char time[5];

  LDR = analogRead(A5);
  if (LDR < LDR_MIN) { // Haz cortado
    if (haEmpezado) { // Ya ha arrancado la carrera
      vuelta[pasada] = m;
      MFS.beep(10);
      // Imprimimos el número de vuelta
      sprintf(time, "%04d", pasada);
      MFS.write(time, 1);
      delay(500);
      if (pasada == numeroVueltas) { // SE ACABO
        total = 0;
        // Calculamos el total y la vuelta mas rapida
        for (unsigned int i = 1; i <= numeroVueltas; i++) {
          tmp = vuelta[i] - vuelta[i - 1]; // Calculamos el tiempo de vuelta
          total += tmp;
          if (masRapida > tmp) masRapida = tmp;
        }
        // Ponemos los datos en 
        int A = 0;    //flag para q solo muestre fin una vez
        while (1) {
          if(A == 0){   //Solo muestra fin si A = 0; despues le asigna 1 para mostrarlo solo 1 vez
            MFS.write("Fin");
            delay(1000);
            A = 1;
          }
          MFS.write("Tot");
          delay(500);
          sprintf(time, "%04d", total);
          MFS.write(time, 1);
          delay(1000);
          if(numeroVueltas > 1){  //solo muestra la vuelta mas rapida si el numero de vueltas es mayro que uno
            MFS.write("Rap");
            delay(500);
            sprintf(time, "%04d", masRapida);
            MFS.write(time, 1);
            delay(1000);
          }
        }
      }
    } else {
      MFS.blinkDisplay(DIGIT_ALL, 0);
      haEmpezado = true; // ¡Arrancamos la carrera!
      vuelta[0] = m; // Inicio de la carrera
      pasada++;
    }
  }
}
