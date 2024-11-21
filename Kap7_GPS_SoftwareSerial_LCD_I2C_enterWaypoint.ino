/* Code based on Adafruit GPS library and example and LiquidCrystal_I2C Library
  only northern hemisphere, eastern longitude due to resources (timing and memory)
  Anzeige: Uhrzeit und Datum, GPS Position, Position Wegpunkt, Kurs und Fahrt
  C: Bernd54Albrecht  
*/
// include the library code:
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line display
#include <Keypad.h>
const byte COLS = 4;     // Columns=Spalten
const byte ROWS = 4;     // Rows = Reihen
char KEYS[ROWS][COLS]={
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'} };
byte COLPINS[COLS] = { 5, 4, 3, 2};     // digitale Ein-/Ausgänge
byte ROWPINS[ROWS] = { 9, 8, 7, 6 };    // digitale Ein-/Ausgänge
Keypad myKeypad = Keypad(makeKeymap(KEYS), ROWPINS, COLPINS, ROWS, COLS);

// Connect the GPS Power pin to 5V
// Connect the GPS Ground pin to ground
// Connect the GPS TX (transmit) pin to Digital 16 = A2
// Connect the GPS RX (receive) pin to Digital 17 = A3

// you can change the pin numbers to match your wiring:
SoftwareSerial mySerial(16,17);
Adafruit_GPS GPS(&mySerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
// #define GPSECHO  false

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
//float SPEED = 0.0;
int tCourse;
float distance;
char pressedKey;
//int i=0;
int EW = 1;

String LatLonWP = "5354.6782N0952.2263E";      // enter your favourite address
int BBMM = LatLonWP.substring(0,4).toInt();    // Latitude/geogr.Breite Format DDMM
int bbbb = LatLonWP.substring(5,9).toInt();    // Latitude/geogr.Breite Format mmmm
int LLMM = LatLonWP.substring(10,14).toInt();  // Longitude/geogr.Länge Format DDMM
int llll = LatLonWP.substring(15,19).toInt();  // Longitude/geogr.Länge Format mmmm


void setup()   {
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  delay(1000);
  Serial.println("Adafruit GPS library");
  Serial.print("LatLonWP: ");
  Serial.println(LatLonWP);
  // uncomment for test  
  Serial.print(BBMM);
  Serial.print(".");
  Serial.println(bbbb);
  Serial.print(LLMM);
  Serial.print(".");
  Serial.println(llll);

  // 9600 NMEA is the default baud rate 

  GPS.begin(9600);
  //set up the LCD's number of columns and rows:
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Time and Date");
  lcd.setCursor(0,1);
  lcd.print("Present Position");
  lcd.setCursor(0,2);
  lcd.print("Position Waypoint");
  lcd.setCursor(0,3);
  lcd.print("Course and Distance");
  delay(5000);
  lcd.clear();
  lcd.setCursor(0,2);  
  lcd.print(LatLonWP);  

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  // GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  // GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);
  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);
}

void keyInput()   {
  Serial.print("LatLonWP: ");
  Serial.println(LatLonWP);
  int i = 0;
  char nextKey = 0;
  while (nextKey !='*')   {
    nextKey=myKeypad.getKey();  
    if (nextKey)   {      
    Serial.print(" nextKey = ");
    Serial.println(nextKey);  

    if (isDigit(int(nextKey)))   {
    LatLonWP[i] = nextKey;
    Serial.print("i = ");
    Serial.print(i);
    Serial.print(" nextKey = ");
    Serial.println(nextKey);
    i +=1;
    Serial.println(LatLonWP);
    lcd.setCursor(0,2);
    lcd.print(LatLonWP);
    }  
    else if (nextKey == '#' && i < 9)   {i = 5;}  
    else if (nextKey == '#' && i > 9)    {i = 15;}
    else if (nextKey == 'A')   {     // 'A' = North
      LatLonWP[9] = 'N';
      i = 10;
    }  
    else if (nextKey == 'B')   {     // 'B' = South
      LatLonWP[9] = 'S';
      i = 10;
    }
    else if (nextKey == 'C')   {     // 'C' = East
      LatLonWP[19] = 'E';
      EW = 1;
      i = 20;
    }
    else if (nextKey == 'D')   {     // 'D' = West
      LatLonWP[19] = 'W';
      EW = -1;
      i = 20;
    } 
    if (i == 20)   {
    Serial.println(LatLonWP);
    lcd.setCursor(0,2);
    lcd.print(LatLonWP);
    i = 21;
    }
    if (i == 21)   {
      BBMM = LatLonWP.substring(0,4).toInt();    // Latitude/geogr.Breite Format DDMM
      bbbb = LatLonWP.substring(5,9).toInt();    // Latitude/geogr.Breite Format mmmm
      LLMM = LatLonWP.substring(10,14).toInt();  // Longitude/geogr.Länge Format DDMM
      llll = LatLonWP.substring(15,19).toInt();  // Longitude/geogr.Länge Format mmmm

    // uncomment for test  
//       Serial.print(BBMM);
//       Serial.print(".");
//       Serial.println(bbbb);
//       Serial.print(LLMM);
//       Serial.print(".");
//       Serial.println(llll);'
      lcd.setCursor(0,2); 
      lcd.print(LatLonWP);   
      i=22;
      }
    }  
  }
}

uint32_t timer = millis();
void loop()   {                     // run over and over again
  pressedKey = myKeypad.getKey();
  if (pressedKey)   {
    Serial.println("press * for waypoint entry");
  }
    if (pressedKey == '*')   {
      Serial.println("waypoint entry");
      keyInput();
    }    

  // if a sentence is received, we can check the checksum, parse it...
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  // if ((c) && (GPSECHO))     Serial.write(c);
 
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
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
    Serial.println(GPS.seconds, DEC); 
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('.');
    Serial.print(GPS.month, DEC); Serial.print(".20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.println((int)GPS.fix);
  //  Serial.print(" quality: "); Serial.println((int)GPS.fixquality);

    lcd.setCursor(0, 0);
    if (GPS.hour < 10) { lcd.print('0'); }
    lcd.print(GPS.hour, DEC); lcd.print(':');
    if (GPS.minute < 10) { lcd.print('0'); }
    lcd.print(GPS.minute, DEC); lcd.print(" UTC ");
    if (GPS.day < 10) { lcd.print('0'); }
    lcd.print(GPS.day, DEC); lcd.print('.');
    if (GPS.month < 10) { lcd.print('0'); }
    lcd.print(GPS.month, DEC); lcd.print(".");
    lcd.println(GPS.year, DEC);

    if (GPS.fix) {
      Lat = GPS.latitude;
      int LatDD = int(Lat/100);
      LatMM = Lat - 100*LatDD;
      LatDDWP = BBMM/100;
      LatMMWP = BBMM - 100*LatDDWP + bbbb/10000;
      // Calculation for Northern Hemisphere only
      deltaPhi = 60*(LatDDWP - LatDD) + (LatMMWP - LatMM);
      deltaY = deltaPhi * 1.852;

      Serial.print("  deltaPhi ");
      Serial.println(deltaPhi);  

      Lon = GPS.longitude;
      LonDDD = int(Lon/100);
      LonMM = Lon - 100*LonDDD;
      LonDDDWP = LLMM/100;
      LonMMWP = LLMM - 100*LonDDDWP + llll/10000;
      // Calculation for Eastern Longitudes only 
      deltaLambda = 60*(LonDDDWP - LonDDD) + (LonMMWP - LonMM);
      
      deltaX = deltaLambda * 1.852 * cos(DEG_TO_RAD * ((LatDDWP+LatDD)/2));
      Serial.print("  deltaLambda ");
      Serial.println(deltaLambda);  

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
      Serial.print("Kurs ");
        if (tCourse < 100)  Serial.print('0'); 
        if (tCourse < 10)   Serial.print('0');      
      Serial.print(tCourse);
      Serial.print("  Distanz ");
      Serial.println(distance);      

      Serial.print("Location: ");
      Serial.print(Lat, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(Lon, 4); Serial.println(GPS.lon);

      // Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      // Serial.print("Angle: "); Serial.println(GPS.angle);
      // Serial.print("Altitude: "); Serial.println(GPS.altitude);
      // Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
      // Serial.print("Antenna status: "); Serial.println((int)GPS.antenna);
     

      lcd.setCursor(0,1);
      if (LatDD<10) lcd.print("0");
      lcd.print(LatDD);
      lcd.print("\xDF");
      lcd.print(LatMM);
      lcd.print("'");
      lcd.print(GPS.lat);
      if (LonDDD<10) lcd.print("0");
      lcd.print(LonDDD);
      lcd.print("\xDF");
      lcd.print(LonMM);     
      lcd.print("'");
      lcd.print(GPS.lon);
    
      lcd.setCursor(0, 3);
      lcd.print("Kurs=");
      if (tCourse < 100)  lcd.print('0'); 
      if (tCourse < 10)  lcd.print('0'); 
      lcd.print(tCourse);
      lcd.print("\xDF");
      lcd.print("D=");
      lcd.print(distance);
      lcd.print("km");
    } 
  }
}
