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

void generate_serial_number(char *output, bool allowVowels) {
    const static char *vowels = "AEIOU";
    const static char *consonants = "BCDFGHJKLMNPQRSTVWXYZ";
    const static char *digits = "0123456789";

    static char letters[27]; // max 26 letters + null terminator
    letters[0] = '\0';

    // Build allowed letters
    if (allowVowels) {
        strcpy(letters, consonants);
        strcat(letters, vowels);
    } else {
        strcpy(letters, consonants);
    }

    size_t numLetters = strlen(letters);
    size_t numDigits = strlen(digits);

    char temp[9]; // 8 chars + null terminator
    bool hasLetter = false;
    bool hasDigit = false;

    // Ensure at least one letter and one digit
    temp[0] = letters[random(0, numLetters)];
    temp[1] = digits[random(0, numDigits)];
    hasLetter = true;
    hasDigit = true;

    // Fill the rest randomly
    for (int i = 2; i < 8; i++) {
        if (random(0, 2) == 0) { // 50% chance
            temp[i] = letters[random(0, numLetters)];
            hasLetter = true;
        } else {
            temp[i] = digits[random(0, numDigits)];
            hasDigit = true;
        }
    }

    temp[8] = '\0';

    // Shuffle result to make positions random
    for (int i = 7; i > 0; i--) {
        int j = random(0, i+1);
        char tmp = temp[i];
        temp[i] = temp[j];
        temp[j] = tmp;
    }

    strcpy(output, temp);
}

bool containsOddDigit(const char *str) {
    while (*str) {
        if (isdigit(*str)) {
            char digit = *str;
            if ((digit - '0') % 2 == 1) {
                return true;
            }
        }
        str++;
    }
    return false;
}

