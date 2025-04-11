#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>

SoftwareSerial softSerial(/*rx =*/10, /*tx =*/11);
#define FPSerial softSerial

DFRobotDFPlayerMini player;

void setup()
{
  FPSerial.begin(9600);

  player.begin(FPSerial, /*isACK = */true, /*doReset = */true);

  player.volume(20);
  player.play(1);
}

void loop()
{
}
