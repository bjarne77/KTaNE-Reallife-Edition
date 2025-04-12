const int Button_red = 2;
const int Button_green = 3;
const int Button_blue = 4;
const int Button_yellow = 5;

const int Led_red = 6;
const int Led_green = 7;
const int Led_blue = 8;
const int Led_yellow = 9;

int buttons[4] = {Button_red, Button_green, Button_blue, Button_yellow};
bool lastButtonState[4] = {LOW, LOW, LOW, LOW};

int sequenz[4] = {};

const int light_on = 500;
const int light_off = 750;
const int sequenz_break = 1000;

const int entprell_delay = 100;

int number_of_pressed_button = 0;
bool vowel_in_serial_number = 0;
int number_of_strikes = 0;

void setup() {
  randomSeed(analogRead(0));
  for (int i=0; i<4; i++) {
    sequenz[i] = random(1, 5);
  }
  Serial.begin(9600);

  pinMode(Button_red, INPUT);
  pinMode(Button_green, INPUT);
  pinMode(Button_blue, INPUT);
  pinMode(Button_yellow, INPUT);
  
  pinMode(Led_red, OUTPUT);
  pinMode(Led_green, OUTPUT);
  pinMode(Led_blue, OUTPUT);
  pinMode(Led_yellow, OUTPUT);

  digitalWrite(Led_red, LOW);
  digitalWrite(Led_green, LOW);
  digitalWrite(Led_blue, LOW);
  digitalWrite(Led_yellow, LOW);

  Serial.print(sequenz[0]);
  Serial.print(sequenz[1]);
  Serial.print(sequenz[2]);
  Serial.print(sequenz[3]);
}


void loop() {
  for (int i = 0; i <= 3; i++) {
      if (sequenz[i] == 1) {
        digitalWrite(Led_red, HIGH);
        long current_time = millis();
        while (millis() <= current_time + light_on) {
          test_button ();
          delay(entprell_delay);
        }
        digitalWrite(Led_red, LOW);
        current_time = millis();
        while (millis() <= current_time + light_off) {
          test_button ();
          delay(entprell_delay);
        }
      }
      else if (sequenz[i] == 2) {
        digitalWrite(Led_green, HIGH);
        long current_time = millis();
        while (millis() <= current_time + light_on) {
          test_button ();
          delay(entprell_delay);
        }
        digitalWrite(Led_green, LOW);
        current_time = millis();
        while (millis() <= current_time + light_off) {
          test_button ();
          delay(entprell_delay);
        }
      }
      else if (sequenz[i] == 3) {
        digitalWrite(Led_blue, HIGH);
        long current_time = millis();
        while (millis() <= current_time + light_on) {
          test_button ();
          delay(entprell_delay);
        }
        digitalWrite(Led_blue, LOW);
        current_time = millis();
        while (millis() <= current_time + light_off) {
          test_button ();
          delay(entprell_delay);
        }
      }
      else if (sequenz[i] == 4) {
        digitalWrite(Led_yellow, HIGH);
        long current_time = millis();
        while (millis() <= current_time + light_on) {
          test_button ();
          delay(entprell_delay);
        }
        digitalWrite(Led_yellow, LOW);
        current_time = millis();
        while (millis() <= current_time + light_off) {
          test_button ();
          delay(entprell_delay);
        }
      }
    }
    long current_time = millis();
      while (millis() <= current_time + sequenz_break) {
        test_button ();
        delay(entprell_delay);
      }
}


void test_button() {
  for (int i = 0; i < 4; i++) {
    int currentState = digitalRead(buttons[i]);
    
    // Prüfe auf steigende Flanke: LOW -> HIGH
    if (currentState == HIGH && lastButtonState[i] == LOW) {
      checking(i)
      Serial.print("button pressed: ");
      Serial.println(buttons[i]);
    }

    // Speichere den aktuellen Zustand für den nächsten Durchlauf
    lastButtonState[i] = currentState;
  }
}

void checking(pressed_button) {
  if (vowel_in_serial_number == 0) {
    if (number_of_strikes == 0) {
      for (int i = 0; i < 4; i++)
        if (number_of_pressed_button == 0) {
          
      }
    }
    else if (number_of_strikes == 1) {

    }
    else if (number_of_strikes == 2) {

    }
  }
}