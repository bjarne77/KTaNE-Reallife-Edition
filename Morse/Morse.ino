// --- Morse Code Modul (Erraten per Eingabe) ---
// LED: LED-BUILTIN; noch anzupassen an externe LED, Eingabe über Serial

int ledPin = 5; 
int dotTime = 200; // <-- ggf anpassen

#include <Arduino.h>
#include <SPI.h>
#include "dogm_7036.h"
#include <Wire.h>

#define DEVICE (3)

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
  EXPLODED,
  FINISHED
};

volatile uint8_t status = NOT_INIT;

uint8_t strike = 0;

// Serial Number
bool has_vowels = false;
bool has_odd = false;

dogm_7036 DOG;

int currentPos = 0;
int selected = 0;
bool aktualisierung = true;

const int buttonPin1 = 2; // Pin-Nummern für die Taster
const int buttonPin2 = 3;
const int buttonPin3 = 6;
const int completedPin = 7;

// Werte sind fest
int dashTime = dotTime * 3;
int symbolPause = dotTime;
int letterPause = dotTime * 3;
int wordPause = dotTime * 7;

// moegliche Woerter, max Leange ist "strobe" mit 22 dot-Einheiten
const char* words[] = {
   "shell", "halls", "slick", "trick", "boxes",
  "leaks", "strobe", "bistro", "flick", "bombs", "break", "brick",
  "steak", "sting", "vector", "beats"
};
int wordCount = sizeof(words) / sizeof(words[0]);

const char* letters = "abcdefghijklmnopqrstuvwxyz";
const char* morse[] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",
  ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.",
  "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."
};

const char* freq[] = {"3.505", "3.515", "3.522", "3.532", "3.535", "3.542",
"3.545", "3.552", "3.555", "3.565", "3.572", "3.575", "3.582", "3.592", "3.595", "3.600"};

String currentWord = "";

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  Wire.begin(DEVICE);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  randomSeed(analogRead(A0));
  serial_init();
  serial_write("Boot... ");

  pinMode(buttonPin1, INPUT_PULLUP); // Pull up intern
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
}

void loop() {
  // Morsecode regelmäßig blinken
  if (status == RUNNING) {
    blinkMorseWord(currentWord.c_str());
  }
}

void receiveEvent(int howMany) {
  lastCommand = 0; // Befehl zurücksetzen

  while (Wire.available()) {
    lastCommand = Wire.read();
  }

  // Serial.print("Empfangener Befehl: ");
  // Serial.println(lastCommand);
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
      // Serial.print("Handle Status ");
      // Serial.println(status);
      Wire.write(status);
      if(status == NEW_STRIKE) {
        status = RUNNING;
      }
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
    case EXPLODED:
      Serial.println("Handle EXPLODED");
      break;
    case FINISHED:
      Serial.println("Handle FINISHED");
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

  currentPos = random(wordCount);
  currentWord = words[currentPos];
  status = READY;
}

void start_module() {
  Serial.print("Neues Spiel! Rate das Wort in Morsecode.\nHinweis: ");
  Serial.println(currentWord + "---" + freq[currentPos]);  // <-- Für Debug, später ausblenden

  Serial.println("Module started");
  status = RUNNING;
}

void serial_init() {
  DOG.initialize(10,0,0,9,4,1,DOGM081);   //SS = 10, 0,0= use Hardware SPI, 9 = RS, 4= RESET, 1 = 5V, EA DOGM081-A (=1 line)
  DOG.displ_onoff(true);          //turn Display on
  DOG.cursor_onoff(true);         //turn Curosor blinking on
}

void serial_write(char* str) {
  DOG.position(1,1);              //set Position: first line, first character
  DOG.string(str);          //show String
}

void blinkMorseWord(const char* word) {
  for (int i = 0; word[i] != '\0'; i++) {
    char c = toLowerCase(word[i]);
    int index = findLetterIndex(c);
    if (index != -1) {
      blinkMorseLetter(morse[index]);
      delay(letterPause);
    }
  }
  long current_time = millis();
  while (millis() <= current_time + wordPause) {
    testInput();
    delay(40);
  }
}

void blinkMorseLetter(const char* code) {
  for (int i = 0; code[i] != '\0'; i++) {
    // if(Serial.available()){break;} //Abbruch des Blinkmusters bei Eingabe
    testInput();
    if (code[i] == '.') {
      blinkDot();
    } else if (code[i] == '-') {
      blinkDash();
    }
    delay(symbolPause);
  }
}

void blinkDot() {
  digitalWrite(ledPin, HIGH);
  delay(dotTime);
  digitalWrite(ledPin, LOW);
}

void blinkDash() {
  digitalWrite(ledPin, HIGH);
  delay(dashTime);
  digitalWrite(ledPin, LOW);
}

int findLetterIndex(char c) {
  for (int i = 0; i < 26; i++) {
    if (letters[i] == c) return i;
  }
  return -1;
}

void testInput () {
  
  if (aktualisierung){
    //char* output = freq[selected] + "MHz";
    char str[9];
    strcpy(str, freq[selected]);
    strcat(str, "MHz");
    Serial.println(str);
    serial_write(str);
    aktualisierung = false;
  }
  
  String input = testButton();
  //Serial.print("Debugging: ");
  //Serial.println(input);
  input.trim();
  input.toLowerCase();

  if (input == "u" && selected < 15){
    selected++;
    aktualisierung = true;
  } else if ( input == "d" && selected > 0){
    selected--;
    aktualisierung = true;
  } else if (input == "e"){
    if (selected==currentPos){
      serial_write("         ");
      Serial.println("Success");
      status = SUCCESS;
      digitalWrite(completedPin, HIGH);
      return;
    } else {
      Serial.println("Strike");
      status = NEW_STRIKE;
    }
  }
}

String testButton (){
  // Lese den Zustand jedes Tasters
  int buttonState1 = digitalRead(buttonPin1);
  int buttonState2 = digitalRead(buttonPin2);
  int buttonState3 = digitalRead(buttonPin3);

  // Prüfe, welcher Taster gedrückt ist (Active LOW wegen INPUT_PULLUP)
  if (buttonState1 == LOW) {
    Serial.println("Button 1 wurde gedrückt.");
    delay(100); // entprellen und Wiederholungen vermeiden
    return ("e");
  }

  if (buttonState2 == LOW) {
    Serial.println("Button 2 wurde gedrückt.");
    delay(100);
    return("d");
  }

  if (buttonState3 == LOW) {
    Serial.println("Button 3 wurde gedrückt.");
    delay(100);
    return ("u");
  }
  return("/n");
}