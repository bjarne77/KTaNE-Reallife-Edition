// Created 29 March 2006
// This example code is in the public domain.

#include <Wire.h>

#include "mp3.h"
#include "serialnumber.h"

enum DEVICE_t {
  DEVICE_1 = 1,
  SIMONSAY,
  MORSE,
  DEVICE_N
};

enum CMD_t {
  RESET,
  INIT,
  START,
  STATUS,
  STRIKE_0,
  STRIKE_1,
  STRIKE_2,
  HAS_VOWELS,
  HAS_ODD,
  EXPLODED,
  FINISHED
};

#define NOT_INIT (100)
#define READY (101)
#define RUNNING (102)
#define NEW_STRIKE (110)
#define FAILED (200)
#define SUCCESS (201)

volatile uint8_t status = NOT_INIT;

uint8_t strikes = 0;

char serial_number[9] = {0};

bool found[DEVICE_N] = {0};
int num_found = 0;

void setup() {
  randomSeed(analogRead(A0));
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  pinMode(LED_BUILTIN, OUTPUT);

  serial_init();
  serial_write("Boot....");

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
      num_found++;
    }
  }
  mp3_init();
  mp3_reset();

  // generate serial number
  const bool has_vowels = random(0,2);
  generate_serial_number(serial_number, has_vowels);
  for(int i = 0; i < DEVICE_N && has_vowels; i++) {
    if(found[i]) {
      sendCommand(i, HAS_VOWELS);
      readResponse(i);
    }
  }
  if(containsOddDigit(serial_number)) {
    for(int i = 0; i < DEVICE_N && has_vowels; i++) {
      if(found[i]) {
        sendCommand(i, HAS_ODD);
        readResponse(i);
      }
    }
  }


  strikes = 0;

  status = READY;

  for(int i = 0; i < DEVICE_N; i++) {
    if(found[i]) {
      sendCommand(i, START);
      readResponse(i);
    }
  }
  mp3_start();
  

  serial_write(serial_number);
  Serial.print("Serial Nr:");
  Serial.println(serial_number);
  status = RUNNING;
}


void loop() {
  static uint8_t new_status;
  new_status = status;

  static uint8_t num_success;
  num_success = 0;

  for(int i = 0; i < DEVICE_N; i++) {
    if(found[i]) {
      sendCommand(i, STATUS);
      const uint8_t module_status = readResponse(i);
      // Serial.print("Status from Node ");
      // Serial.print(i);
      // Serial.print(":  ");
      // Serial.println(module_status);

      if(status == RUNNING && module_status == FAILED) {
        new_status = FAILED;
      } else if(status == RUNNING && module_status == NEW_STRIKE) {
        strikes++;
        if(strikes >= 3) {
          new_status = FAILED;
        } else {
          new_status = NEW_STRIKE;
          Serial.print("Got new Strike from Device ");
          Serial.println(i);
        }
      } else if(status == RUNNING && module_status == SUCCESS) {
        Serial.print("Got Success from Device ");
        Serial.println(i);
        num_success++;
      }
    }
  }

  if(new_status == FAILED) {
    if(status == RUNNING) {
      Serial.println("Exploded");
      mp3_explode();
      serial_write("Exploded");

      for(int i = 0; i < DEVICE_N; i++) {
        if(found[i]) {
          sendCommand(i, EXPLODED);
          readResponse(i);
        }
      }
    }
    status = FAILED;
  } else if (new_status == NEW_STRIKE) {
    for(int i = 0; i < DEVICE_N; i++) {
      if(found[i]) {
        switch(strikes) {
          case 0:
            sendCommand(i, STRIKE_0);
            break;
          case 1:
            sendCommand(i, STRIKE_1);
            break;
          case 2:
            sendCommand(i, STRIKE_2);
            break;
          default:
            break;
        }
        readResponse(i);
      }
    }
    status = RUNNING;
  }

  if(status == RUNNING && num_success == num_found) {
    for(int i = 0; i < DEVICE_N; i++) {
      if(found[i]) {
        sendCommand(i, FINISHED);
        readResponse(i);
      }
    }
    status = FINISHED;
    // TODO inform the player
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
  // Serial.print("Gesendet: ");
  // Serial.println(cmd);
}

uint8_t readResponse(int address) {
  delay(50); // Kleine Pause für den Slave
  Wire.requestFrom(address, 1); // max 20 Bytes

  uint8_t response = 0;
  while (Wire.available()) {
    response = Wire.read();
  }

  // Serial.print("Antwort: ");
  // Serial.println(response);

  return response;
}
