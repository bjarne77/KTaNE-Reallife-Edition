
#include <Wire.h>

#include "module.h"

#define DEVICE (2)

Module module;

const int Button_red = 3;
const int Button_blue = 2;
const int Button_green = 5;
const int Button_yellow = 4;

const int Led_red = 7;
const int Led_blue = 6;
const int Led_green = 9;
const int Led_yellow = 8;

const int Led_finisch = 13;

int buttons[4] = {Button_red, Button_blue, Button_green, Button_yellow};
bool lastButtonState[4] = {LOW, LOW, LOW, LOW};

enum Color {
  Red = 0,
  Blue = 1,
  Green = 2,
  Yellow = 3
};

Color sequenz[4] = {};

const int light_on = 500;
const int light_off = 750;
const int sequenz_break = 1000;

const int entprell_delay = 100;

// [strikes] [color]
const static Color matrix_vowel[3][4] = {
  {Blue, Red, Yellow, Green},
  {Yellow, Green, Blue, Red},
  {Green, Red, Yellow, Blue}
};
const static Color matrix_novowel[3][4] = {
  {Blue, Yellow, Green, Red},
  {Red, Blue, Yellow, Green},
  {Yellow, Green, Blue, Red}
};

static int solved_state = 0;

void setup() {
  randomSeed(analogRead(0));
  Serial.begin(9600);

  module.init(DEVICE);
  module.handle_reset_ptr = &reset_module;
  module.handle_init_ptr = &init_module;
  module.handle_start_ptr = &start_module;
  module.handle_finished_ptr = &handle_finish;
  module.handle_exploded_ptr = &handle_exploded;

  pinMode(Button_red, INPUT_PULLUP);
  pinMode(Button_green, INPUT_PULLUP);
  pinMode(Button_blue, INPUT_PULLUP);
  pinMode(Button_yellow, INPUT_PULLUP);
  
  pinMode(Led_red, OUTPUT);
  pinMode(Led_green, OUTPUT);
  pinMode(Led_blue, OUTPUT);
  pinMode(Led_yellow, OUTPUT);
  pinMode(Led_finisch, OUTPUT);

  digitalWrite(Led_red, LOW);
  digitalWrite(Led_green, LOW);
  digitalWrite(Led_blue, LOW);
  digitalWrite(Led_yellow, LOW);
  digitalWrite(Led_finisch, LOW);
}


void loop() {
  if(module.get_status() == Module::STATUS::RUNNING)
  for (int i = 0; i <= 3; i++) {
    switch(sequenz[i]) {
      case Color::Red:
        blink_and_test(Led_red);
        break;
      case Color::Blue:
        blink_and_test(Led_blue);
        break;
      case Color::Green:
        blink_and_test(Led_green);
        break;
      case Color::Yellow:
        blink_and_test(Led_yellow);
        break;
    }
  }
  long current_time = millis();
  while (millis() <= current_time + sequenz_break) {
    test_button ();
    delay(entprell_delay);
  }
}


// Rest the module
void reset_module() {
  digitalWrite(Led_red, LOW);
  digitalWrite(Led_green, LOW);
  digitalWrite(Led_blue, LOW);
  digitalWrite(Led_yellow, LOW);
  digitalWrite(Led_finisch, LOW);
}

void init_module() {
  for (int i=0; i<4; i++) {
    sequenz[i] = random(0, 4);
  }
  Serial.print("New Sequenz: ");
  Serial.print(color2name(sequenz[0]));
  Serial.print(" ");
  Serial.print(color2name(sequenz[1]));
  Serial.print(" ");
  Serial.print(color2name(sequenz[2]));
  Serial.print(" ");
  Serial.print(color2name(sequenz[3]));
  Serial.println("");
}

void start_module() {
  // TODO
}

void handle_finish() {
  Serial.println("Finished !");
  digitalWrite(Led_red, LOW);
  digitalWrite(Led_green, LOW);
  digitalWrite(Led_blue, LOW);
  digitalWrite(Led_yellow, LOW);
  digitalWrite(Led_finisch, HIGH);
}

void handle_exploded() {
  Serial.println("Exploded !");
  digitalWrite(Led_red, LOW);
  digitalWrite(Led_green, LOW);
  digitalWrite(Led_blue, LOW);
  digitalWrite(Led_yellow, LOW);
  digitalWrite(Led_finisch, LOW);
}


void test_button() {
  for (int i = 0; i < 4; i++) {
    int currentState = digitalRead(buttons[i]);
    
    // Prüfe auf fallende Flanke: LOW -> HIGH
    if (currentState == HIGH && lastButtonState[i] == LOW) {
      checking(buttons[i]);
    }

    // Speichere den aktuellen Zustand für den nächsten Durchlauf
    lastButtonState[i] = currentState;
  }
}

void blink_and_test(const int led) {
  digitalWrite(led, HIGH);
  long current_time = millis();
  while (millis() <= current_time + light_on) {
    test_button ();
    delay(entprell_delay);
  }
  digitalWrite(led, LOW);
  current_time = millis();
  while (millis() <= current_time + light_off) {
    test_button ();
    delay(entprell_delay);
  }
}

const char* color2name(const int color) {
  switch(color) {
    case Color::Red:
      return "red";
    case Color::Blue:
      return "blue";
    case Color::Green:
      return "green";
    case Color::Yellow:
      return "yellow";
    default:
      return "undef";
  }
}

Color button2color(const int button) {
  switch(button) {
    case Button_red:
      return Color::Red;
    case Button_green:
      return Color::Green;
    case Button_yellow:
      return Color::Yellow;
    case Button_blue:
      return Color::Blue;
    default:
      Serial.println("Error");
      exit(0);
  }
}

int checking(int pressed_button, Color matrix[3][4]) {
  const auto exprected = matrix[module.get_strike_count()][sequenz[solved_state]];
  Serial.print("Expected: ");
  Serial.print(color2name(exprected));
  Serial.print(" got: ");
  Serial.println(color2name(button2color(pressed_button)));

  if(exprected == button2color(pressed_button)) {
    solved_state++;
    if(solved_state == 4) {
      Serial.println("Successful");
      //module.update_status(Module::STATUS::SUCCESS)
    }
  } else {
    solved_state = 0;
    //module.update_status(Module::STATUS::NEW_STRIKE)
  }

  Serial.print("State: ");
  Serial.println(solved_state);
}

int checking(int pressed_button) {
  if(module.get_status() == Module::STATUS::RUNNING) {
    if (module.has_vowels() == WireBool::WIRE_TRUE) {
      checking(pressed_button, matrix_vowel);
    } else {
      checking(pressed_button, matrix_novowel);
    }
  }
}