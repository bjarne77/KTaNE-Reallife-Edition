const int Button_red = 2;
const int Button_blue = 3;
const int Button_green = 4;
const int Button_yellow = 5;

const int Led_red = 6;
const int Led_blue = 7;
const int Led_green = 8;
const int Led_yellow = 9;

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

int number_of_pressed_button = 0;
bool vowel_in_serial_number = true;
int number_of_strikes = 0;

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
  for (int i=0; i<4; i++) {
    sequenz[i] = random(0, 4);
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


void loop() {
  for (int i = 0; i <= 3; i++) {
      if (sequenz[i] == 1) {
        foo(Led_red);
      }
      else if (sequenz[i] == 2) {
        foo(Led_blue);
      }
      else if (sequenz[i] == 3) {
        foo(Led_green);
      }
      else if (sequenz[i] == 4) {
        foo(Led_yellow);
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
      checking(buttons[i]);
      Serial.print("button pressed: ");
      Serial.println(buttons[i]);
    }

    // Speichere den aktuellen Zustand für den nächsten Durchlauf
    lastButtonState[i] = currentState;
  }
}

void foo(const int led) {
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
    case 0:
      return "red";
    case 1:
      return "blue";
    case 2:
      return "green";
    case 3:
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

int checking(int pressed_button) {
  if (vowel_in_serial_number) {
    const auto exprected = matrix_vowel[number_of_strikes][sequenz[solved_state]];
    Serial.print("Expected: ");
    Serial.print(color2name(exprected));
    Serial.print(" got: ");
    Serial.println(color2name(button2color(pressed_button)));

    if(exprected == button2color(pressed_button)) {
      solved_state++;
    } else {
      solved_state = 0;
      // TODO inform master about a new strike...
    }

    Serial.print("State: ");
    Serial.println(solved_state);
  } else {
    const auto exprected = matrix_novowel[number_of_strikes][sequenz[solved_state]];
    Serial.print("Expected: ");
    Serial.print(color2name(exprected));
    Serial.print(" got: ");
    Serial.println(color2name(button2color(pressed_button)));

    if(exprected == button2color(pressed_button)) {
      solved_state++;
    } else {
      solved_state = 0;
      // TODO inform master about a new strike...
    }

    Serial.print("State: ");
    Serial.println(solved_state);
  }
}
