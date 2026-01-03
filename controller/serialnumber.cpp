#include <Arduino.h>
#include <SPI.h>
#include "dogm_7036.h"

dogm_7036 DOG;

byte arrow_down[] = {0x04, 0x04, 0x04, 0x04, 0x15, 0x0E, 0x04, 0x00}; //pattern for own defined character

void serialnumber_init() {
  DOG.initialize(10,0,0,9,4,1,DOGM162);   //SS = 10, 0,0= use Hardware SPI, 9 = RS, 4= RESET, 1 = 5V, EA DOGM081-A (=1 line)
  DOG.displ_onoff(true);          //turn Display on
  DOG.cursor_onoff(true);         //turn Curosor blinking on

  DOG.define_char(0, arrow_down); //define own char on memory adress 0
}

void serialnumber_write(char* str) {
  DOG.position(1,1);              //set Position: first line, first character
  DOG.string(str);          //show String
}

void serialnumber_write2(char* str) {
  DOG.position(1,2);              //set Position: first line, first character
  DOG.string(str);          //show String
}

void generate_serial_number(char *output, bool hasVowels, bool hasOdd) {
    const static char *vowels = "AEIOU";
    const static char *consonants = "BCDFGHJKLMNPQRSTVWXYZ";
    const static char *digits = "0123456789";
    const static char *oddNumbers = "13579";
    const static char *evenNumbers = "2468";

    static char letters[27]; // max 26 letters + null terminator
    letters[0] = '\0';

    // Build allowed letters
    if (hasVowels) {
        strcpy(letters, consonants);
        strcat(letters + strlen(consonants), vowels);
    } else {
        strcpy(letters, consonants);
    }

    const size_t numLetters = strlen(letters);
    const size_t numDigits = strlen(digits);
    const size_t numVowels = strlen(vowels);

    char temp[9]; // 8 chars + null terminator

    if(hasVowels) {
        temp[0] = vowels[random(0, numVowels)];
    }

    // Fill the rest randomly
    for (int i = 1; i < 7; i++) {
        if (random(0, 5) == 0) { // 75% chance
            temp[i] = letters[random(0, numLetters)];
        } else {
            temp[i] = digits[random(0, numDigits)];
        }
    }

    temp[8] = '\0';

    // Shuffle result to make positions random
    for (int i = 6; i > 0; i--) {
        int j = random(0, i+1);
        char tmp = temp[i];
        temp[i] = temp[j];
        temp[j] = tmp;
    }

    // make sure the last digit is a number
    if(hasOdd) {
        temp[7] = oddNumbers[random(0, strlen(oddNumbers))];
    } else {
        temp[7] = evenNumbers[random(0, strlen(evenNumbers))];
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

