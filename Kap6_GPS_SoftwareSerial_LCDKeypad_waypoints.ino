/* Code based on Adafruit GPS library and example  
  and LiquidCrystal Library - Hello World
  Anzeige: GPS Position, Uhrzeit und Datum, oder Kurs und Fahrt zu
  vor-definierten Wegpunkten (Zeilen 45 bis 49)
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

float Lat = 0.0;    // Latitude = geogr. Breite
int LatDD = 0;
float LatMM = 0.0;
int LatDDWP = 0;
float LatMMWP = 0.0;
float deltaPhi = 0.0;
float deltaY = 0.0;
float Lon = 0.0;    // Longitude = geogr. Länge
int LonDDD = 0;
float LonMM = 0.0;
int LonDDDWP = 0;
float LonMMWP = 0.0;
float deltaLambda = 0.0;
float deltaX = 0.0;
float SPEED = 0.0;

// String Location[] = {"Bad Bramstedt", "Kopenhagen","Hamburg","Bremen", "Hannover"};
float LatWP[] =     {5354.6800,     5540.5600,   5333.7500,  5306.0500,  5222.1500};
float LonWP[] =     {952.2100,     1234.1000,    958.6900,  846.8200,    944.4700};
int waypointIndex = 0;
int waypointMAX = 5;
int tCourse;
float distance;

// Buttons
int buttonInput = -1;
int buttonSelect = 0;
int buttonInput_old = 0;
bool buttonHold = false;

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

int Button()  {   // self-defined functions for keypad buttons
  int A0 = analogRead(A0);        // 
  if (A0 < 60) {
    return 0;
  }
  else if (A0 >= 60 && A0 < 195) {
    return 1;
  }
  else if (A0 >= 195 && A0 < 380){
    return 2;
  }
  else if (A0 >= 380 && A0 < 555){
    return 3;
  }
  else if (A0 >= 555 && A0 < 900){
    return 4;
  }
  else {
    buttonHold = false;
    return -1;
  }
}   //end Button()

void loop()   {                     // run over and over again
  buttonInput = Button();
  // Button muss losgelassen werden
  switch (buttonInput) {
    case 0: Serial.println("0");buttonSelect=0; break;
    case 1: Serial.println("1");buttonSelect=1; break;
    case 2: Serial.println("2");buttonSelect=2; break;
    case 3: Serial.println("3");buttonSelect=3; break;
    case 4: Serial.println("4");
    
    if (waypointIndex>waypointMAX-1) waypointIndex=0;
    buttonSelect=4; 
    Serial.print("waypointIndex ");
    Serial.println(waypointIndex); break;
    default: break;
  }
  //Button loslassen pruefen
  if (buttonInput != buttonInput_old) {
    buttonHold = false;
  }  
  
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
  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
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
      Lat = GPS.latitude;
      int LatDD = int(Lat/100);
      LatMM = Lat - 100*LatDD;
      LatDDWP = int(LatWP[waypointIndex]/100);
      LatMMWP = LatWP[waypointIndex] - 100*LatDDWP;
      deltaPhi = 60*(LatDDWP - LatDD) + (LatMMWP - LatMM);
      deltaY = deltaPhi * 1.852;

      Lon = GPS.longitude;
      LonDDD = int(Lon/100);
      LonMM = Lon - 100*LonDDD;
      LonDDDWP = int(LonWP[waypointIndex]/100);
      LonMMWP = LonWP[waypointIndex] - 100*LonDDDWP;
      deltaLambda = 60*(LonDDDWP - LonDDD) + (LonMMWP - LonMM);
      deltaX = deltaLambda * 1.852 * cos(DEG_TO_RAD * ((LatDDWP+LatDD)/2));

      if (abs(deltaY)<0.5) {
        if (deltaX < 0.1 && deltaX > -0.1) {
          tCourse = 1359;
          distance = 0;
          }        
        else if (deltaX > 0) {
          tCourse = 90;
          distance = deltaX;
          }
        else if (deltaX < 0) {
          tCourse = 270;
          distance = - deltaX;
          }
      }

      else if (deltaY>0) { 
        if (deltaX < 0.1 && deltaX > -0.1) {
          tCourse = 360;
          distance = deltaY;
          }        
        else if (deltaX > 0) {
          tCourse = (90 - RAD_TO_DEG*atan2(deltaY,deltaX));
          distance = sqrt(square(deltaX) + square(deltaY));
          }
        else if (deltaX < 0) {
          tCourse = (90 - RAD_TO_DEG*atan2(deltaY,deltaX));
          distance = sqrt(square(deltaX) + square(deltaY));
          }
        }
      else if (deltaY<0) { 
        if (deltaX < 0.1 && deltaX > -0.1) {
          tCourse = 180;
          distance = -deltaY;
          }        
        else if (deltaX > 0) {
          tCourse = (90 - RAD_TO_DEG*atan2(deltaY,deltaX));
          distance = sqrt(square(deltaX) + square(deltaY));
          }
        else if (deltaX < 0) {
          tCourse = (90 - RAD_TO_DEG*atan2(deltaY,deltaX));
          distance = sqrt(square(deltaX) + square(deltaY));
          }
        }       
      if (tCourse < 0)  tCourse = tCourse+360;
      if (tCourse > 360)  tCourse = tCourse-360;      
      Serial.print("Waypoint");
      Serial.print(waypointIndex);
      Serial.print(" Kurs ");
        if (tCourse < 100)  Serial.print('0'); 
        if (tCourse < 10)   Serial.print('0');      
      Serial.print(tCourse);
      Serial.print(" Distanz ");
      Serial.println(distance);      

      Serial.print("Location: ");
      Serial.print(Lat, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(Lon, 4); Serial.println(GPS.lon);

      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
      Serial.print("Antenna status: "); Serial.println((int)GPS.antenna);
     
      if (buttonSelect==0)  {
        lcd.clear();
        lcd.setCursor(0, 0);
        if (LatDD<10) lcd.print("0");
        lcd.print(LatDD);
        lcd.print("\xDF");
        lcd.print(LatMM);
        lcd.print("'");
        lcd.print(GPS.lat);
        lcd.setCursor(0, 1);
        if (LonDDD<100) lcd.print("0");
        if (LonDDD<10) lcd.print("0");
        lcd.print(LonDDD);
        lcd.print("\xDF");
        lcd.print(LonMM);     
        lcd.print("'");
        lcd.print(GPS.lon);
      }
      if (buttonSelect==1)  {
        waypointIndex += 1;
        if (waypointIndex>4)   waypointIndex=0;
        buttonSelect = 4;
        lcd.clear();
      }  
      if (buttonSelect==2)  {
        waypointIndex -= 1;
        if (waypointIndex<0)   waypointIndex=4;
        buttonSelect = 4;
        lcd.clear();
      }
      if (buttonSelect==3)  {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Time: ");
        if (GPS.hour < 10) { lcd.print('0'); }
        lcd.print(GPS.hour, DEC); lcd.print(':');
        if (GPS.minute < 10) { lcd.print('0'); }
        lcd.print(GPS.minute, DEC); lcd.print(" UTC");
        lcd.setCursor(0, 1);        
        lcd.print("Date: ");
        if (GPS.day < 10) { lcd.print('0'); }
        lcd.print(GPS.day, DEC); lcd.print('.');
        if (GPS.month < 10) { lcd.print('0'); }
        lcd.print(GPS.month, DEC); lcd.print(".20");
        lcd.println(GPS.year, DEC);
      }
      if (buttonSelect==4)  {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("WP");lcd.print(waypointIndex);
        lcd.print("  Kurs ");
        if (tCourse < 100)  lcd.print('0'); 
        if (tCourse < 10)  lcd.print('0'); 
        lcd.print(tCourse);
        lcd.print("\xDF");
        lcd.setCursor(0, 1);
        lcd.print("Dist. ");
        lcd.print(distance);
        lcd.print(" km");
      }  
    }
  }
}
