// --- Morse Code Modul (Erraten per Eingabe) ---
// LED: LED-BUILTIN; noch anzupassen an externe LED, Eingabe über Serial

int ledPin = LED_BUILTIN; 
int dotTime = 400; // <-- ggf anpassen
int currentPos = 0;
int strikesMorse = 0;

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
}

void loop() {
    
  

  // Eingabe prüfen
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
      dealay(letterPause);
    }
  }
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
  delay(wordPause);
}

void blinkMorseLetter(const char* code) {
  for (int i = 0; code[i] != '\0'; i++) {
    if(Serial.available()){break;} //Abbruch des Blinkmusters bei Eingabe
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