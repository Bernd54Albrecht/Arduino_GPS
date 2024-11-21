#include <Keypad.h>
const byte SPALTEN = 4;
const byte ZEILEN = 4;
char TASTEN[ZEILEN][SPALTEN]={
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'} };
byte SpaltenPIN[SPALTEN] = { 5, 4, 3, 2};
byte ZeilenPIN[ZEILEN] = { 9, 8, 7, 6 };
char pressedKey;
char nextKey;
char LatLon[] = {};
Keypad myKeypad = Keypad(makeKeymap(TASTEN), ZeilenPIN, SpaltenPIN, ZEILEN, SPALTEN);

void setup() {
Serial.begin(9600);

}


void loop() {
pressedKey = myKeypad.getKey();
if (pressedKey) {
Serial.print(pressedKey);
Serial.println();
}
}