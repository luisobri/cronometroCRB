#include <TimerOne.h>
#include <Wire.h>
#include <MultiFuncShield.h>

int LDR;
volatile unsigned int clockMilliSeconds = 0;
volatile byte clockCentiSeconds = 0;
volatile byte clockSeconds = 0;
bool coche = false;
int pasada = 0;

static byte LDR_MIN = 180;
static byte numeroVueltas = 1; // Numero de vueltas que contaremos
int vuelta[4];
int total = 0;
int masRapida = 99999;

boolean isr = false;
boolean isrC = false;

void setup()
{
  Timer1.initialize(1000); // Inicializamos Timer1 a mil microsegundos -> 1 milisegundo
  MFS.userInterrupt = clockISR;
  MFS.initialize(&Timer1);
  MFS.blinkDisplay(DIGIT_ALL);
  MFS.beep(10);
  Serial.begin(115200); // Mas velocidad -> menos tiempo para escribir por el puerto serie
}

void loop()
{
  if (isr) {
    ejecutar_isr();
  }
  if (isrC) {
    displayTime(clockSeconds, clockCentiSeconds);
    isrC=false;
  }
}

void ejecutar_isr() {
  LDR = analogRead(A5);
  if (LDR < LDR_MIN) { // Haz cortado
    if (coche) { // Ya ha arrancado la carrera
      vuelta[pasada] = clockSeconds * 100 + clockCentiSeconds;
      clockMilliSeconds = 0;
      clockCentiSeconds = 0;
      clockSeconds = 0;
      MFS.beep(10);
      char time[5];
      sprintf(time, "%04d", pasada);
      MFS.write(time, 1);
      delay(500);
      if (pasada == numeroVueltas) { // SE ACABO
        total = 0;
        for (int i = 0; i < numeroVueltas; i++) {
          total += vuelta[i];
          if (masRapida>vuelta[i]) masRapida=vuelta[i];
        }
        while (1) {
          MFS.write("Fin");
          delay(1000);
          MFS.write("Tot");
          delay(500);
          sprintf(time, "%04d", total);
          MFS.write(time, 1);
          delay(1000);
          MFS.write("Rap");
          delay(500);
          sprintf(time, "%04d", masRapida);
          MFS.write(time, 1);
          delay(1000);
        }
      }
      pasada++;
    } else {
      MFS.blinkDisplay(DIGIT_ALL, 0);
      coche = true; // ¡Arrancamos la carrera!
      // Ponemos a cero el reloj
      clockMilliSeconds = 0;
      clockCentiSeconds = 0;
      clockSeconds = 0;
      pasada = 0;
    }
  } else {
    if (coche) { // Ya ha arrancado la carrera
      //MFS.beep(20, 2, 8);
    } else {
      clockMilliSeconds = 0;
      clockCentiSeconds = 0;
      clockSeconds = 0;
    }
  }

  isr = false;
}

//---------------------------------------------------------
void displayTime (byte seconds, byte centiseconds)
{
  char time[5];
  sprintf(time, "%04d", (seconds * 100) + centiseconds);
  MFS.write(time, 1);
}

//--------------------------------------------------------------------------------------------------
void clockISR ()
{
  isr = true;
  clockMilliSeconds++;
  if (clockMilliSeconds >= 10) {
    clockMilliSeconds = 0;
    clockCentiSeconds++;
    isrC = true;
    if (clockCentiSeconds >= 100) {
      clockCentiSeconds = 0;
      clockSeconds++;
      if (clockSeconds >= 100) {
        clockCentiSeconds = 0;
        clockCentiSeconds = 0;
        clockSeconds = 0;
      }
    }
  }
}
