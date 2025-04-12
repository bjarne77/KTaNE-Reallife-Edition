// Wire Peripheral Sender
// by Nicholas Zambetti [http://www.zambetti.com](http://www.zambetti.com)


// Demonstrates use of the Wire library
// Sends data as an I2C/TWI peripheral device
// Refer to the "Wire Master Reader" example for use with this


// Created 29 March 2006
// This example code is in the public domain.



#include <Wire.h>

#define DEVICE (1)

uint8_t lastCommand = "";

#define NOT_INIT (100)
#define READY (101)
#define RUNNING (102)
#define NEW_STRIKE (110)
#define FAILED (200)
#define SUCCESS (201)

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
};

volatile uint8_t status = NOT_INIT;

unsigned long waitTime = 0;
unsigned long startTime = 0;

uint8_t strike = 0;

// Serial Number
bool has_vowels = false;
bool has_odd = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Slave bereit.");

  Wire.begin(DEVICE);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  
  pinMode(LED_BUILTIN, OUTPUT);
}


void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(700);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(700);
  if(status == RUNNING) {
    if(millis() - startTime >= waitTime) {
      status = FAILED;
      Serial.println("Failed");
    }
  }
}


void receiveEvent(int howMany) {
  lastCommand = 0; // Befehl zurücksetzen

  while (Wire.available()) {
    lastCommand = Wire.read();
  }

  Serial.print("Empfangener Befehl: ");
  Serial.println(lastCommand);
}

void requestEvent() {
  switch(lastCommand) {
    case RESET:
      Serial.println("Handle Reset");
      reset_module();
      break;
    case INIT:
      Serial.println("Handle Init");
      init_module();
      break;
    case START:
      Serial.println("Handle Start");
      start_module();
      break;
    case STATUS:
      Serial.println("Handle Status");
      Wire.write(status);
      break;
    case STRIKE_0:
      Serial.println("Handle Strike 0");
      strike = 0;
      break;
    case STRIKE_1:
      Serial.println("Handle Strike 1");
      strike = 1;
      break;
    case STRIKE_2:
      Serial.println("Handle Strike 2");
      strike = 2;
      break;
    case HAS_VOWELS:
      Serial.println("Handle HAS_VOWELS");
      has_vowels = true;
      break;
    case HAS_ODD:
      Serial.println("Handle HAS_ODD");
      has_odd = true;
      break;
    default:
      Serial.println("Handle undef cmd");
      break;
  }
}

// Rest the module
void reset_module() {
  strike = 0;
  has_vowels = false;
  has_odd = false;
  status = NOT_INIT;
}

void init_module() {
  if(status != NOT_INIT) {
    Serial.println("Can't init without reset");
    return;
  }

  waitTime = random(1000, 10001);
  Serial.print("Wartezeit (ms): ");
  Serial.println(waitTime);

  status = READY;
}

void start_module() {
  Serial.println("Module started");
  startTime = millis();
  status = RUNNING;
}

