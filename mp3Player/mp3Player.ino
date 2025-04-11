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

  // set volume between 0 and 30
  player.volume(20);
}

void loop()
{
  static unsigned long timerTick = millis();
  static unsigned long timerExplodeStart = millis();
  static unsigned long timerExplodeStop = millis();
  static bool explode = false;

  if (millis() - timerExplodeStart > 12000) {
    explode = true;
    timerExplodeStart = millis();
    timerExplodeStop = millis();
    // play explosion sound
    player.play(2);
  }

  if (millis() - timerExplodeStop > 5000) {
    explode = false;
  }

  if (!explode && millis() - timerTick > 1000) {
    timerTick = millis();
    // play tick sound
    player.play(1);
  }
}
