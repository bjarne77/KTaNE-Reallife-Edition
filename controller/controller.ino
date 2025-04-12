// Created 29 March 2006
// This example code is in the public domain.

#include <Wire.h>

#include "mp3.h"

enum DEVICE_t {
  DEVICE_1 = 1,
  DEVICE_2,
  DEVICE_N
};

enum CMD_t {
  RESET,
  INIT,
  START,
  STATUS
};

#define NOT_INIT (100)
#define READY (101)
#define RUNNING (102)
#define FAILED (200)

volatile uint8_t status = NOT_INIT;

bool found[DEVICE_N] = {0};

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  pinMode(LED_BUILTIN, OUTPUT);

  // INIT Blink Master
  for(int i = DEVICE_1; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(400);                      // wait for a second
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
    delay(400);
  }

  // Detect devices
  for(int i = 0; i < DEVICE_N; i++) {
    found[i] = foundDevice(i);

    if(found[i]) {
      sendCommand(i, RESET);
      readResponse(i);
      sendCommand(i, INIT);
      readResponse(i);
      delay(1000);
      sendCommand(i, STATUS);
      readResponse(i);
      // Init device
    }
  }
  mp3_init();
  mp3_reset();

  status = READY;

  for(int i = 0; i < DEVICE_N; i++) {
    if(found[i]) {
      sendCommand(i, START);
      readResponse(i);
    }
  }
  mp3_start();
  status = RUNNING;
}


void loop() {
  for(int i = 0; i < DEVICE_N; i++) {
    if(found[i]) {
      sendCommand(i, STATUS);
      const uint8_t module_status = readResponse(i);

      if(status == RUNNING && module_status == FAILED) {
        Serial.println("Exploded");
        Serial.print("Abc");
        Serial.print(status);
        mp3_explode();
        status = FAILED;
        Serial.print("   ABV");
        Serial.println(status);
      }
    }
  }
  delay(100);
}

bool foundDevice(int address) {
  Wire.beginTransmission(address);
  byte error = Wire.endTransmission();

  switch(error) {
    case 0: // success
      Serial.print("I2C-Gerät gefunden bei Adresse 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
      return true;
      break;
    case 1: // to much data
      Serial.print("I2C-Gerät to much data bei Adresse 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
      break;
    case 2: // received NACK on transmit of address.
      Serial.print("I2C-Gerät NAC transmition bei Adresse 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
      break;
    case 3: // received NACK on transmit of data.
      Serial.print("I2C-Gerät NACK data bei Adresse 0x");
        if (address < 16)
          Serial.print("0");
        Serial.println(address, HEX);
      break;
    case 4: // other error
      Serial.print("I2C-Gerät other error bei Adresse 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
      break;
    case 5: // timeout
      Serial.print("I2C-Gerät timeout bei Adresse 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
      break;
    default:
      Serial.print("I2C-Gerät default case bei Adresse 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
      break;
  }
  return false;
}

void sendCommand(const int address, const CMD_t cmd) {
  Wire.beginTransmission(address);
  Wire.write((uint8_t)(cmd)); // Sende als C-String
  Wire.endTransmission();
  Serial.print("Gesendet: ");
  Serial.println(cmd);
}

uint8_t readResponse(int address) {
  delay(50); // Kleine Pause für den Slave
  Wire.requestFrom(address, 1); // max 20 Bytes

  uint8_t response = 0;
  while (Wire.available()) {
    response = Wire.read();
  }

  Serial.print("Antwort: ");
  Serial.println(response);

  return response;
}
