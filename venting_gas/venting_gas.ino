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

void setup() {
  pinMode(YES_BUTTON_PIN, INPUT_PULLUP);
  pinMode(NO_BUTTON_PIN, INPUT_PULLUP);
  
  Serial.begin(9600);
  randomSeed(analogRead(A0));

  gameStartTime = millis();
  scheduleNextQuestion();
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

  questionCountdown = random(1, min(121, remainingTime + 1)); // max countdown = Restspielzeit
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

  Serial.println("\n⚠️  NEUE FRAGE:");
  if (currentQuestion == VENT_GAS) {
    Serial.println("VENT GAS?");
  } else {
    Serial.println("DETONATE?");
  }

  Serial.println("Drücke YES oder NO!");
  Serial.print("Antwort-Zeit: ");
  Serial.println(responseCountdown);
}

// Reaktion des Spielers
void handleAnswer(bool userSaidYes) {
  awaitingResponse = false;

  if (currentQuestion == VENT_GAS) {
    if (userSaidYes) {
      Serial.println("✅ Richtig (GAS entlüftet).");
    } else {
      Serial.println("❌ Strike! (VENTING PREVENTS EXPLOSIONS)");
    }
  } else if (currentQuestion == DETONATE) {
    if (userSaidYes) {
      Serial.println("💥 BOOM! Die Bombe ist explodiert.");
    } else {
      Serial.println("✅ Richtig (Nicht detoniert).");
    }
  }

  scheduleNextQuestion();
}

// Wenn keine Taste gedrückt wurde
void handleNoResponse() {
  awaitingResponse = false;

  Serial.println("⏰ Zeit abgelaufen!");
  Serial.println("❌ Strike");
  if (currentQuestion == VENT_GAS) {
    Serial.println("VENTING PREVENTS EXPLOSIONS");
  }

  askNewQuestion();
}
