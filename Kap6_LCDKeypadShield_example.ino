#include <LiquidCrystal.h>
// initialize the LCD library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
 
// Erstellen einiger Variablen
int Taster = 0;
int Analogwert = 0;
#define Tasterrechts 0
#define Tasteroben 1
#define Tasterunten 2
#define Tasterlinks 3
#define Tasterselect 4
#define KeinTaster 5

// Ab hier wird ein neuer Programmblock mit dem Namen "Tasterstatus" erstellt. Hier wird ausschließlich geprüft, welcher Taster gedrückt ist.
int Tasterstatus()   {
  Analogwert = analogRead(A0); // Auslesen der Taster am Analogen Pin A0.
  if (Analogwert > 1000) return KeinTaster;
  if (Analogwert < 50) return Tasterrechts;
  if (Analogwert < 195) return Tasteroben;
  if (Analogwert < 380) return Tasterunten;
  if (Analogwert < 555) return Tasterlinks;
  if (Analogwert < 790) return Tasterselect;
  return KeinTaster; // Ausgabe wenn kein Taster gedrückt wurde.
}
// Hier wird der Programmblock "Tasterstatus" abeschlossen.

void setup()   {
  Serial.begin(9600);
  lcd.begin(16, 2); // Starten der Programmbibliothek.
  lcd.setCursor(0,0); // Angabe des Cursorstartpunktes oben links.
  lcd.print("LCD KeypadShield"); // Ausgabe des Textes "Nachricht".
  }
 
void loop()   {
  lcd.setCursor(12,1); // Bewegt den Cursor in Zeile 2 (Line0=Zeile1 und Line1=Zeile2) und die Stelle "12".
  lcd.print(millis()/1000); // Zeigt die Sekunden seit Start des Programms in Sekunden an.
  lcd.setCursor(0,1); // Bewegt den Cursor and die Stelle "0" in Zeile 2.
  
  Taster = Tasterstatus(); //Hier springt der Loop in den oben angegebenen Programmabschnitt "Tasterstatus" und liest dort den gedrückten Taster aus.
  
  switch (Taster) // Abhängig von der gedrückten Taste wird in dem folgenden switch-case Befehl entschieden, was auf dem LCD angezeigt wird.
  {
  case Tasterrechts: // Wenn die rechte Taste gedrückt wurde...
    {
    lcd.print("Rechts      "); //Erscheint diese Zeile. Die Leerzeichen hinter dem Text sorgen dafür, dass der vorherige Text in der Zeile gelöscht wird.
    Serial.println("Rechts      ");
    break;
    }
  case Tasterlinks:  // Wenn die linke Taste gedrückt wurde...
    {
    lcd.print("Links       ");  //Erscheint diese Zeile... usw...
    Serial.println("Links       ");
    break;
    }
  case Tasteroben:
    {
    lcd.print("Oben        ");
    Serial.println("Oben        ");    
    break;
    }
  case Tasterunten:
    {
    lcd.print("Unten       ");
    Serial.println("Unten       ");
    break;
    }
  case Tasterselect:
    {
    lcd.print("SELECT      ");
    Serial.println("SELECT      ");
    break;
    }
  case KeinTaster:
    {
    lcd.print("Keine Taste ");
    Serial.println("Keine Taste ");    
    break;
    }
  } //switch-case Befehl beenden
} //Loop beenden
