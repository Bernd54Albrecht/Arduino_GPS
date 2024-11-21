/* Test code based on Adafruit GPS library and example  
  and LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" and GPS position to the LCD

  The circuit:
 * LCD RS pin to digital pin 8
 * LCD Enable pin to digital pin 9
 * LCD D4 pin to digital pin 4
 * LCD D5 pin to digital pin 5
 * LCD D6 pin to digital pin 6
 * LCD D7 pin to digital pin 7
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)//
  Die Umlaute der Texte wurden durch die Escape-Sequenzen ersetzt.
  lcd.print("\xE1"); // gibt ein ä aus
  lcd.print("\xEF"); // gibt ein ö aus
  lcd.print("\xF5"); // gibt ein ü aus
  lcd.print("\xE2"); // gibt ein ß aus
  lcd.print("\xDF"); // gibt ein ° aus
  lcd.print("\x22"); // gibt ein " aus  
  lcd.print("\xE4"); // gibt ein µ aus
  lcd.print("\xF4"); // gibt ein Ω aus
*/
// include the library code:
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

// initialize the LCD library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Connect the GPS Power pin to 5V
// Connect the GPS Ground pin to ground
// Connect the GPS TX (transmit) pin to Digital 17 = A3
// Connect the GPS RX (receive) pin to Digital 18 = A4

// you can change the pin numbers to match your wiring:
SoftwareSerial mySerial(17, 18);
Adafruit_GPS GPS(&mySerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO  false

void setup()   {
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  delay(5000);
  Serial.println("Adafruit GPS library basic parsing test!");
  // 9600 NMEA is the default baud rate 
  GPS.begin(9600);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Hello World!");

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);
  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);
}

uint32_t timer = millis();
void loop()                     // run over and over again
{
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  // if ((c) && (GPSECHO))
  //   Serial.write(c);

  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false

    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // approximately every 5 seconds or so, print out the current stats
  if (millis() - timer > 5000) {
    timer = millis(); // reset the timer

    Serial.print("\nTime: ");
    if (GPS.hour < 10) { Serial.print('0'); }
    Serial.print(GPS.hour, DEC); Serial.print(':');
    if (GPS.minute < 10) { Serial.print('0'); }
    Serial.print(GPS.minute, DEC); Serial.print(':');
    if (GPS.seconds < 10) { Serial.print('0'); }
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    if (GPS.milliseconds < 10) {
      Serial.print("00");
    } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
      Serial.print("0");
    }
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);

      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
      Serial.print("Antenna status: "); Serial.println((int)GPS.antenna);
      // set the cursor to column 0, line 0
      // (note: counting begins with 0):
      lcd.clear();
      lcd.setCursor(0, 0);
      // print the number of seconds since reset:
      float Latitude = GPS.latitude;
      int Lat_deg = int(Latitude/100);
      float Lat_min = Latitude - 100*Lat_deg;
//      lcd.print(GPS.latitude);
      lcd.print(Lat_deg);
      lcd.print("\xDF");
      lcd.print(Lat_min);
      lcd.print("'");
      lcd.print(GPS.lat);
      lcd.setCursor(0, 1);
//      lcd.print(GPS.longitude);
      float Longitude = GPS.longitude;
      int Lon_deg = int(Longitude/100);
      float Lon_min = Longitude - 100*Lon_deg;
      if (Lon_deg<100) lcd.print("0");
      if (Lon_deg<10) lcd.print("0");
      lcd.print(Lon_deg);
      lcd.print("\xDF");
      lcd.print(Lon_min);     
      lcd.print("'");
      lcd.print(GPS.lon);
    }
  }
}
