// --- Morse Code Modul (Erraten per Eingabe) ---
// LED: LED-BUILTIN; noch anzupassen an externe LED, Eingabe über Serial

int ledPin = 5; 
int dotTime = 200; // <-- ggf anpassen

#include <Arduino.h>
#include <SPI.h>
#include "dogm_7036.h"

dogm_7036 DOG;

byte arrow_down[] = {0x04, 0x04, 0x04, 0x04, 0x15, 0x0E, 0x04, 0x00}; //pattern for own defined character

void serial_init() {
  DOG.initialize(10,0,0,9,4,1,DOGM081);   //SS = 10, 0,0= use Hardware SPI, 9 = RS, 4= RESET, 1 = 5V, EA DOGM081-A (=1 line)
  DOG.displ_onoff(true);          //turn Display on
  DOG.cursor_onoff(true);         //turn Curosor blinking on

  DOG.define_char(0, arrow_down); //define own char on memory adress 0
}

void serial_write(char* str) {
  DOG.position(1,1);              //set Position: first line, first character
  DOG.string(str);          //show String
}


int currentPos = 0;
int strikesMorse = 0;
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
bool isBlinking = false;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  randomSeed(analogRead(A0));
  currentPos = random(wordCount);
  currentWord = words[currentPos];
  Serial.print("Neues Spiel! Rate das Wort in Morsecode.\nHinweis: ");
  Serial.println(currentWord + "---" + freq[currentPos]);  // <-- Für Debug, später ausblenden
  isBlinking = true;
  serial_init();

  pinMode(buttonPin1, INPUT_PULLUP); // Pull up intern
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
}

void loop() {
    
  

  // Eingabe prüfen
  /*
  if (Serial.available()) {
    String guess = Serial.readStringUntil('\n');
    guess.trim();
    guess.toLowerCase();

    //Aktuelle Ausgabe bei RICHTIEGER Eingabe, ggf durch Uebermittlung ersetzten
    if (guess == freq [currentPos]) {
      Serial.println("✅ Richtig ");
      isBlinking = false;
      for (int fb=0; fb<10; fb++){
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
        }
    }
   

    // Aktuelle Ausgabe bei FALSCHER Eingabe, ggf durch Uebermittlung ersetzten 
    else {
      Serial.println("❌ Falsch! Versuche es erneut.");
      strikesMorse++;
      delay(letterPause);
    }
    
  }
  */
  // Morsecode regelmäßig blinken
  if (isBlinking) {
    blinkMorseWord(currentWord.c_str());
  }
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
      isBlinking = false;
      digitalWrite(completedPin, HIGH);
      return;
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
