

void serialnumber_init();
void serialnumber_write(char* str);
void serialnumber_write2(char* str); // Write to second line

void generate_serial_number(char *output, bool hasVowels, bool hasOdd);
bool containsOddDigit(const char *str);