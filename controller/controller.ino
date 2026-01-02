// Created 29 March 2006
// This example code is in the public domain.

#include <Wire.h>

#include "module.h"

#include "mp3.h"
#include "serialnumber.h"


volatile Module::STATUS status = Module::STATUS::NOT_INIT;

uint8_t strikes = 0;

char serial_number[9] = {0};

bool found[DEVICE::DEVICE_N] = {0};
int num_found = 0;

void setup() {
  randomSeed(analogRead(A0));
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  pinMode(LED_BUILTIN, OUTPUT);

  serialnumber_init();
  serialnumber_write("Boot....");

  // Detect devices
  for(int i = 0; i < DEVICE::DEVICE_N; i++) {
    found[i] = foundDevice(i);

    if(found[i]) {
      sendCommand(i, Module::CMD::RESET);
      readResponse(i);
      sendCommand(i, Module::CMD::INIT);
      readResponse(i);
      delay(1000);
      sendCommand(i, Module::CMD::STATUS_UPDATE);
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
  for(int i = 0; i < DEVICE::DEVICE_N && has_vowels; i++) {
    if(found[i]) {
      sendCommand(i, Module::CMD::HAS_VOWELS);
      readResponse(i);
    }
  }
  if(containsOddDigit(serial_number)) {
    for(int i = 0; i < DEVICE::DEVICE_N && has_vowels; i++) {
      if(found[i]) {
        sendCommand(i, Module::CMD::HAS_ODD);
        readResponse(i);
      }
    }
  }


  strikes = 0;

  status = Module::STATUS::READY;

  for(int i = 0; i < DEVICE::DEVICE_N; i++) {
    if(found[i]) {
      sendCommand(i, Module::CMD::START);
      readResponse(i);
    }
  }
  mp3_start();
  

  serialnumber_write(serial_number);
  Serial.print("Serial Nr:");
  Serial.println(serial_number);
  status = Module::STATUS::RUNNING;
}


void loop() {
  static Module::STATUS new_status;
  new_status = status;

  static uint8_t num_success;
  num_success = 0;

  for(int i = 0; i < DEVICE::DEVICE_N; i++) {
    if(found[i]) {
      sendCommand(i, Module::CMD::STATUS_UPDATE);
      const Module::STATUS module_status = (Module::STATUS)readResponse(i);
      // Serial.print("Status from Node ");
      // Serial.print(i);
      // Serial.print(":  ");
      // Serial.println(module_status);

      if(status == Module::STATUS::RUNNING && module_status == Module::STATUS::FAILED) {
        new_status = Module::STATUS::FAILED;
      } else if(status == Module::STATUS::RUNNING && module_status == Module::STATUS::NEW_STRIKE) {
        strikes++;
        if(strikes >= 3) {
          new_status = Module::STATUS::FAILED;
        } else {
          new_status = Module::STATUS::NEW_STRIKE;
          Serial.print("Got new Strike from Device ");
          Serial.println(i);
        }
      } else if(status == Module::STATUS::RUNNING && module_status == Module::STATUS::SUCCESS) {
        Serial.print("Got Success from Device "); 
        Serial.println(i);
        num_success++;
      }
    }
  }

  if(new_status == Module::STATUS::FAILED) {
    if(status == Module::STATUS::RUNNING) {
      Serial.println("Exploded");
      mp3_explode();
      serialnumber_write("Exploded");

      for(int i = 0; i < DEVICE::DEVICE_N; i++) {
        if(found[i]) {
          sendCommand(i, Module::CMD::EXPLODED);
          readResponse(i);
        }
      }
    }
    status = Module::STATUS::FAILED;
  } else if (new_status == Module::STATUS::NEW_STRIKE) {
    for(int i = 0; i < DEVICE::DEVICE_N; i++) {
      if(found[i]) {
        switch(strikes) {
          case 0:
            sendCommand(i, Module::CMD::STRIKE_0);
            break;
          case 1:
            sendCommand(i, Module::CMD::STRIKE_1);
            break;
          case 2:
            sendCommand(i, Module::CMD::STRIKE_2);
            break;
          default:
            break;
        }
        readResponse(i);
      }
    }
    status = Module::STATUS::RUNNING;
  }

  if(status == Module::STATUS::RUNNING && num_success == num_found) {
    for(int i = 0; i < DEVICE::DEVICE_N; i++) {
      if(found[i]) {
        sendCommand(i, Module::CMD::FINISHED);
        readResponse(i);
      }
    }
    status = Module::STATUS::FINISHED;
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

void sendCommand(const int address, const Module::CMD cmd) {
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
