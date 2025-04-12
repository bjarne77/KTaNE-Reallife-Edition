#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>

#include "mp3.h"

SoftwareSerial softSerial(/*rx =*/7, /*tx =*/8);
#define FPSerial softSerial

DFRobotDFPlayerMini player;

#define VOLUME (20) // 0-30

void mp3_init() {
  FPSerial.begin(9600);

  player.begin(FPSerial, /*isACK = */true, /*doReset = */true);

  // set volume between 0 and 30
  player.volume(VOLUME);
}

void mp3_reset() {
  player.stop();
  return;
}

void mp3_start() {
  // play tick
  //player.play(1);
  player.loop(1);
}

void mp3_explode() {
  // play explosion sound
  player.play(2);
}


 