const int YES_BUTTON_PIN = 2;
const int NO_BUTTON_PIN = 3;

unsigned long lastTime;
unsigned long gameStartTime;

int questionCountdown = 0;
int responseCountdown = 5;
unsigned long lastResponseTime;

bool questionActive = false;
bool awaitingResponse = false;
bool gameOver = false;

enum QuestionType { VENT_GAS, DETONATE };
QuestionType currentQuestion;

#include <Arduino.h>
#include <SPI.h>
#include "dogm_7036.h"

dogm_7036 DOG;

byte arrow_down[] = {0x04, 0x04, 0x04, 0x04, 0x15, 0x0E, 0x04, 0x00}; //pattern for own defined character



void setup() {
  serial_init(); // <-- Display initialisieren

  pinMode(YES_BUTTON_PIN, INPUT_PULLUP);
  pinMode(NO_BUTTON_PIN, INPUT_PULLUP);
  
  Serial.begin(9600);
  randomSeed(analogRead(A0));

  gameStartTime = millis();
  scheduleNextQuestion();
}

void displayMessage(const char* line1) {
  DOG.position(1, 1); // Erste Zeile
  DOG.string(line1);
}
void serial_init(){
  DOG.initialize(10,0,0,9,4,1,DOGM081);   //SS = 10, 0,0= use Hardware SPI, 9 = RS, 4= RESET, 1 = 5V, EA DOGM081-A (=1 line)
  DOG.displ_onoff(true);          //turn Display on
  DOG.cursor_onoff(true);         //turn Curosor blinking on

  DOG.define_char(0, arrow_down); //define own char on memory adress 0
}

void loop() {
  unsigned long currentTime = millis();


  // Frage kommt nach geheimer Countdown-Zeit
  if (!questionActive && currentTime - lastTime >= 1000) {
    questionCountdown--;
    lastTime = currentTime;

    if (questionCountdown <= 0) {
      askNewQuestion();
    }
  }

  // Während Antwortzeit läuft
  if (awaitingResponse && currentTime - lastResponseTime >= 1000) {
    responseCountdown--;
    lastResponseTime = currentTime;

    char buffer[16];
    sprintf(buffer, "Zeit: %d Sek.", responseCountdown);
    displayMessage(currentQuestion == VENT_GAS ? "VENT GAS?" : "DETONATE?");

    Serial.print("Antwort-Zeit: ");
    Serial.println(responseCountdown);

    if (responseCountdown <= 0) {
      handleNoResponse();
    }
  }

  // Eingabe
  if (awaitingResponse) {
    if (digitalRead(YES_BUTTON_PIN) == LOW) {
      handleAnswer(true);
    } else if (digitalRead(NO_BUTTON_PIN) == LOW) {
      handleAnswer(false);
    }
  }
}

// Zufällige Frage vorbereiten (aber nicht anzeigen)
void scheduleNextQuestion() {
  // Wenn max. Spielzeit bald vorbei, keine neue Frage mehr planen
  unsigned long remainingTime = 350 - ((millis() - gameStartTime) / 1000);
  if (remainingTime < 1) {
    gameOver = true;
    return;
  }

  questionCountdown = random(1, min(21, remainingTime + 1)); // max countdown = Restspielzeit
  questionActive = false;
  awaitingResponse = false;

  lastTime = millis();
}

// Neue Frage stellen
void askNewQuestion() {
  currentQuestion = (random(2) == 0) ? VENT_GAS : DETONATE;
  questionActive = true;
  awaitingResponse = true;
  responseCountdown = random(20,41);
  lastResponseTime = millis();

  if (currentQuestion == VENT_GAS) {
    displayMessage("VENT GAS?  Y / N");
  } else {
    displayMessage("DETONATE?  Y / N");
  }

  Serial.println("\n NEUE FRAGE:");
  Serial.println(currentQuestion == VENT_GAS ? "VENT GAS?" : "DETONATE?");
  Serial.println("Drücke YES oder NO!");
  Serial.print("Antwort-Zeit: ");
  Serial.println(responseCountdown);
}

// Reaktion des Spielers
void handleAnswer(bool userSaidYes) {
  awaitingResponse = false;

  if (currentQuestion == VENT_GAS) {
    if (userSaidYes) {
      displayMessage("CORRECT    ");
    } else {
      displayMessage("STRIKE   ");
  delay(3000); 
  scheduleNextQuestion();
    }
  } else if (currentQuestion == DETONATE) {
    if (userSaidYes) {
      displayMessage("BOOM!   ");
      delay(3000);
      DOG.displ_onoff(false);
    } else {
      displayMessage("CORRECT   ");
      delay(3000);
      scheduleNextQuestion();
    }
  }

}

// Wenn keine Taste gedrückt wurde
void handleNoResponse() {
  awaitingResponse = false;

  displayMessage("⏰ Zeit abgelaufen! ❌ Strike");
  if (currentQuestion == VENT_GAS) {
    displayMessage("STRIKE    ");
  }

  delay(3000);
  askNewQuestion();
}
