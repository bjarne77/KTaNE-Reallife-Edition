
// --- Morse Code Modul (Erraten per Eingabe) ---
// LED: LED-BUILTIN; noch anzupassen an externe LED, Eingabe über Serial

#include <Arduino.h>
#include <SPI.h>
#include "dogm_7036.h"
#include <Wire.h>
#include "module.h"

const int ledPin = 5; 

Module module;

int cooldown = 500;
long DownLastTimePressed = millis();
long UpLastTimePressed = millis();
bool DownReleased = false;
bool UpReleased = false;

dogm_7036 DOG;

int currentPos = 0;
int selected = 0;
bool aktualisierung = true;

const int buttonPin1 = 16; // Pin-Nummern für die Taster
const int buttonPin2 = 17;
const int buttonPin3 = 18;
const int completedPin = 7;

// Werte sind fest
const int dotTime = 200; // <-- ggf anpassen
const int dashTime = dotTime * 3;
const int symbolPause = dotTime;
const int letterPause = dotTime * 3;
const int wordPause = dotTime * 7;

const int delay_entprell = 50;

// moegliche Woerter, max Leange ist "strobe" mit 22 dot-Einheiten
const char* words[] = {
   "shell", "halls", "slick", "trick", "boxes",
  "leaks", "strobe", "bistro", "flick", "bombs", "break", "brick",
  "steak", "sting", "vector", "beats"
};
const int wordCount = sizeof(words) / sizeof(words[0]);

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
  randomSeed(analogRead(A0));
  Serial.begin(9600);

  module.init(DEVICE::MORSE);
  module.handle_init_ptr = &init_module;
  module.handle_reset_ptr = &reset_module;
  module.handle_start_ptr = &start_module;

  pinMode(buttonPin1, INPUT_PULLUP); // Pull up intern
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);

  pinMode(ledPin, OUTPUT);
  pinMode(completedPin, OUTPUT);
  digitalWrite(completedPin, HIGH);

  serial_init();
  serial_write("Boot... ");
}

void loop() {
  // Morsecode regelmäßig blinken
  if (module.get_status() == Module::STATUS::RUNNING) {
    blinkMorseWord(currentWord.c_str());
  }
}

// Rest the module
void reset_module() {
}

void init_module() {
  currentPos = random(wordCount);
  currentWord = words[currentPos];
}

void start_module() {
  Refresh_Display();
  Serial.print("Neues Spiel! Rate das Wort in Morsecode.\nHinweis: ");
  Serial.println(currentWord + "---" + freq[currentPos]);  // <-- Für Debug, später ausblenden
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
    char c = word[i];
    int index = findLetterIndex(word);
    if (index != -1) {
      blinkMorseLetter(morse[index]);
      long current_time = millis();
      while (millis() <= current_time + letterPause) {
        testButton ();

      }
    }
  }
  long current_time = millis();
  while (millis() <= current_time + wordPause) {
    testButton();
  }
}

void blinkMorseLetter(const char* code) {
  Serial.print("in blinkMorseLetter");      //NICHT LÖSCHEN! --> muss halt so
  for (int i = 0; code[i] != '\0'; i++) {
    if (code[i] == '.') {
      blinkDot();
    } else if (code[i] == '-') {
      blinkDash();
    }
    long current_time = millis();
      while (millis() <= current_time + symbolPause) {
        testButton();
      }
  }
}

void blinkDot() {
  digitalWrite(ledPin, HIGH);

 // hier dotTime lang testInput testen
      long current_time = millis();
      while (millis() <= current_time + dotTime) {
        testButton();
      }
  digitalWrite(ledPin, LOW);
}

void blinkDash() {
  digitalWrite(ledPin, HIGH);
  
  // time_t start = time(nullptr);
      long current_time = millis();
      while (millis() <= current_time + dashTime) {
        testButton();
      }
  digitalWrite(ledPin, LOW);
}

int findLetterIndex(char c) {
  for (int i = 0; i < 26; i++) {
    if (letters[i] == c) return i;
  }
  return -1;
}

void Refresh_Display (){
  char str[9];
  strcpy(str, freq[selected]);
  strcat(str, "MHz");
  serial_write(str);
}

void up_pressed () {
  if (selected < 15){
    selected++;
    Refresh_Display();
  }
}

void down_pressed () {
  if (selected > 0){
    selected--;
    Refresh_Display();
  }
}

void enter_pressed(){
  if (selected==currentPos){
    serial_write("         ");
    module.update_status(Module::STATUS::SUCCESS);
    digitalWrite(completedPin, LOW);
  } else {
    module.update_status(Module::STATUS::NEW_STRIKE);
  }
}

//String testButton (){
void testButton (){
  // Lese den Zustand jedes Tasters
  int buttonState1 = digitalRead(buttonPin1);
  int buttonState2 = digitalRead(buttonPin2);
  int buttonState3 = digitalRead(buttonPin3);

  // Prüfe, welcher Taster gedrückt ist (Active LOW wegen INPUT_PULLUP)
  if (buttonState1 == LOW) {
    delay(delay_entprell); // entprellen und Wiederholungen vermeiden
    enter_pressed();
  }

  if (buttonState2 == HIGH) {
    DownReleased = true;
  }

  if (buttonState2 == LOW && (millis() > DownLastTimePressed + cooldown || DownReleased)) {
    DownReleased = false;
    DownLastTimePressed = millis();
    delay(delay_entprell);
    down_pressed();
  }

  if (buttonState3 == HIGH) {
    UpReleased = true;
  }

  if (buttonState3 == LOW && (millis() > UpLastTimePressed + cooldown || UpReleased)) {
    UpReleased = false;
    UpLastTimePressed = millis();
    delay(delay_entprell);
    up_pressed();
  }
}