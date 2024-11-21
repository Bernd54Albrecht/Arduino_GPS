/*********************************************************************************
  GPS mit LCD-Anzeige
  basierend auf Bibliothek DFRobot_sim808 und Beispiel SIM808_GetGPS von jason.ling@dfrobot.com
  Angepasst für UNO, Nano und kompatible MCUs und LCD1602 Keypad Shield
  Offset für MEZ = 1 Stunde, ggf. in Zeile 53 anpassen für MESZ --> Offset=2
  Uhrzeit und Tag werden kurz nach Mitternacht angepasst, jedoch Monat und Jahr nicht.
  Für SMS eigene Mobiltelefonnummer in Zeile 30 eingeben und Zeile 209 entkommentieren

  Anschlüsse:
  TX des GPS verbinden mit A3 = GPIO 17 des LCD Keypad
  RX des GPS verbinden mit A4 = GPIO 18 des LCD Keypad

  Die Umlaute der Texte wurden durch die Escape-Sequenzen ersetzt.
  lcd.print("\xE1"); // gibt ein ä aus
  lcd.print("\xEF"); // gibt ein ö aus
  lcd.print("\xF5"); // gibt ein ü aus
  lcd.print("\xE2"); // gibt ein ß aus
  lcd.print("\xDF"); // gibt ein ° aus
  lcd.print("\x22"); // gibt ein " aus  
  lcd.print("\xE4"); // gibt ein µ aus
  lcd.print("\xF4"); // gibt ein Ω aus
  
*********************************************************************************/
#include <DFRobot_SIM808.h>
#include <SoftwareSerial.h>
#define PIN_TX    17
#define PIN_RX    18

//Mobile phone number,need to change
#define PHONE_NUMBER "00491xxxxxxxxxx"    // enter your mobile phone number
char MESSAGE[160] = "";

SoftwareSerial mySerial(PIN_TX,PIN_RX);
DFRobot_SIM808 sim808(&mySerial);  //Connect RX,TX,PWR
//DFRobot_SIM808 sim808(&Serial);

//  LCD has no I2C-Adapter, data transfer with Pins D4 to D7 
#include <LiquidCrystal.h>
//LCD pin to Arduino
//const int pin_BL = 15; 
const int pin_EN = 9; 
const int pin_RS = 8; 
const int pin_D4 = 4; 
const int pin_D5 = 5; 
const int pin_D6 = 6; 
const int pin_D7 = 7;  

LiquidCrystal lcd( pin_RS,  pin_EN,  pin_D4,  pin_D5,  pin_D6,  pin_D7);

// Offset for Time, here UTC zu MEZ / MESZ
// Summertime MESZ: 2, Wintertime MEZ: 1
#define Offset 1

// unterbrechungsfreie Zeitsteuerung
unsigned long previousMillis = 0;
const long interval = 1000;
unsigned long previousSMS = 0;
const long SMSinterval = 300000;     // 3.600.000 msec = 3.600 sec = 60 min

// Buttons
int buttonInput = -1;
int buttonSelect = 0;

int YEAR = 2024;
int MONTH = 0;
int DAY = 0;
int DAYLCL = 0;
int HOUR = 0;
int HOURLCL = 0;
int MINUTE = 0;
int SECOND = 0; 
float LAT = 0.0;
int LATDD = 0;
float LATMMmmmm = 0.0;
int LATMM = 0;
float LATSSs = 0.0;
float LON = 0.0;
int LONDDD = 0;
float LONMMmmmm = 0.0;
int LONMM = 0;
float LONSSs = 0.0;

void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);
  lcd.begin(16,2);         // initialize the lcd 
  lcd.clear(); 
  lcd.setCursor(0,0);     //Zählung beginnt bei Null, erst Zeichen, dann Zeile
  lcd.print("GPS data");
  lcd.setCursor(0,1);     // 0=Erstes Zeichen, 1=zweite Zeile

  //******** Initialize sim808 module *************
  while(!sim808.init()) { 
      delay(1000);
      Serial.print("Sim808 init error\r\n");
  }

  //************* Turn on the GPS power************
  if( sim808.attachGPS())   {
    Serial.println("Open the GPS power success");
    lcd.setCursor(0,1);     //Zählung beginnt bei Null, erst Zeichen, dann Zeile
   lcd.print("GPS power success");      
  }
  else   {
    Serial.println("Open the GPS power failure");
    lcd.setCursor(0,1);     //Zählung beginnt bei Null, erst Zeichen, dann Zeile
    lcd.print("GPS power failure");  
  }
}

void loop() {
// if wanted, add     sim808.sendSMS(PHONE_NUMBER, "Warning");  
  buttonInput = Button();
  switch (buttonInput) {
    case 0: Serial.println("0");buttonSelect=0; break;
    case 1: Serial.println("1");buttonSelect=1; break;
    case 2: Serial.println("2");buttonSelect=2; break;
    case 3: Serial.println("3");buttonSelect=3; break;
    case 4: Serial.println("4");buttonSelect=4; break;
    default: break;
  }
    

  if (millis() - previousMillis >= interval) {  
     //************** Get GPS data *******************
      if (sim808.getGPS()) {
      MONTH = sim808.GPSdata.month;
      DAY = sim808.GPSdata.day;
      DAYLCL = DAY;
      HOUR = sim808.GPSdata.hour;
      HOURLCL = HOUR + Offset;
      MINUTE = sim808.GPSdata.minute;
      SECOND = sim808.GPSdata.second;      
      if (HOURLCL>24)  {
        HOURLCL = HOURLCL-24;
        DAY = DAYLCL + 1; }      
      if (DAY<10) Serial.print("0");
      Serial.print(DAY);
      Serial.print(".");
      Serial.print(MONTH);
      Serial.print(".");
      Serial.print(sim808.GPSdata.year);
      Serial.print("  ");
      if (HOUR<10) Serial.print("0");
      Serial.print(HOURLCL);
      Serial.print(":");
      if (MINUTE<10) Serial.print("0");
      Serial.print(MINUTE);
      Serial.print(":");
      if (SECOND<10) Serial.print("0");
      Serial.print(SECOND);
      Serial.println(" UTC ");

 Serial.print("latitude:  ");
    Serial.println(sim808.GPSdata.lat,6);
    sim808.latitudeConverToDMS();
    Serial.print("latitude:  ");
    LATDD = sim808.latDMS.degrees;
    Serial.print(LATDD);
    Serial.print("°");
    LATMM = sim808.latDMS.minutes;
    Serial.print(LATMM);
    Serial.print("'");
    LATSSs = sim808.latDMS.seconeds;
    Serial.print(LATSSs,4);
    Serial.println("\x22");
    LATMMmmmm = LATMM + LATSSs/60;
    Serial.print("latitude:  ");
    Serial.print(LATDD);
    Serial.print("°");    
    Serial.print(LATMMmmmm,4);
    Serial.println("'N");

    Serial.print("longitude:  ");
    Serial.println(sim808.GPSdata.lon,6);
    sim808.LongitudeConverToDMS();
    Serial.print("longitude:  ");
    LONDDD = sim808.longDMS.degrees;
    Serial.print(LONDDD);
    Serial.print("°");
    LONMM = sim808.longDMS.minutes;
    Serial.print(LONMM);
    Serial.print("'");
    LONSSs = sim808.longDMS.seconeds;
    Serial.print(LONSSs,4);
    Serial.println("\x22");
    LONMMmmmm = LONMM + LONSSs/60;
    Serial.print("longitude:  ");
    Serial.print(LONDDD);
    Serial.print("°");
    Serial.print(LONMMmmmm,4);
    Serial.println("'E");
    Serial.print("speed_kph :");
    Serial.println(sim808.GPSdata.speed_kph);
    Serial.print("heading :");
    Serial.println(sim808.GPSdata.heading);
    Serial.println();

      //The content of messages sent
      sprintf(MESSAGE, "SIM808 %02d.%02d.%4d,%02d:%02d,%02d.%02d.%02d N,%03d.%02d.%02d E",DAY,MONTH,YEAR,HOUR,MINUTE,LATDD,LATMM,int(LATSSs),LONDDD,LONMM,int(LATSSs));
 
      //******** define phone number and text **********

      if (millis()-previousSMS>SMSinterval)  {
        Serial.print("uncomment for SMS");
//        sim808.sendSMS(PHONE_NUMBER, MESSAGE); 
        previousSMS = millis();
      } 
      else  {
        Serial.println(MESSAGE);
      }   
     
      if (buttonSelect==0)  {
        lcd.clear(); 
        lcd.setCursor(0,0);  
        lcd.print(LATDD);
        lcd.print("\xDF");
        int LATMM = int(LATMMmmmm);
        float LATSSs = (LATMMmmmm-LATMM)*60;
        lcd.print(LATMM); 
        lcd.print("'");           
        lcd.print(LATSSs,1);
        lcd.print("\x22 N");
        lcd.setCursor(0,1); 
        if (LON<100.0) lcd.print("0");
        if (LON<10.0) lcd.print("0");               
        lcd.print(LONDDD);
        lcd.print("\xDF");
        int LONMM = int(LONMMmmmm);
        float LONSSs = (LONMMmmmm-LONMM)*60;
        if (LONMM<10) lcd.print("0");
        lcd.print(LONMM);
        lcd.print("'");
        if (LONSSs<10.0) lcd.print("0");
        lcd.print(LONSSs,1);
        lcd.print("\x22 E"); }  
        
      else if (buttonSelect==1)  {
        lcd.clear(); 
        lcd.setCursor(0,0);  
        lcd.print("EUR: ");
        if (DAYLCL<10) lcd.print("0");
        lcd.print(DAYLCL);
        lcd.print(".");
        if (MONTH<10) lcd.print("0");
        lcd.print(MONTH);
        lcd.print(".");
        lcd.print(sim808.GPSdata.year);
        lcd.setCursor(5,1);
        if (HOURLCL<10) lcd.print("0");
        lcd.print(HOURLCL);
        lcd.print(":");
        if (MINUTE<10) lcd.print("0");
        lcd.print(MINUTE);
        lcd.print(":");
        if (SECOND<10) lcd.print("0");
        lcd.print(SECOND);
      }
      else if (buttonSelect==2)  {
        lcd.clear(); 
        lcd.setCursor(0,0);  
        lcd.print("UTC: ");
        lcd.print(sim808.GPSdata.year);
        lcd.print("/");
        if (MONTH<10) lcd.print("0");
        lcd.print(MONTH);
        lcd.print("/");
        if (DAY<10) lcd.print("0");
        lcd.print(DAY);
        lcd.setCursor(5,1);
        if (HOUR<10) lcd.print("0");
        lcd.print(HOUR);
        lcd.print(":");
        if (MINUTE<10) lcd.print("0");
        lcd.print(MINUTE);
        lcd.print(":");
        if (SECOND<10) lcd.print("0");
        lcd.print(SECOND);
        }
      else if (buttonSelect==3)  {
        lcd.clear(); 
        lcd.setCursor(0,0);  
        lcd.print(LATDD);
        lcd.print("\xDF");            
        lcd.print(LATMMmmmm,4);
        lcd.print("' N");
        lcd.setCursor(0,1); 
        if (LON<100.0) lcd.print("0");
        if (LON<10.0) lcd.print("0");               
        lcd.print(LONDDD);
        lcd.print("\xDF"); 
        if (LONMMmmmm<10.0) lcd.print("0");           
        lcd.print(LONMMmmmm,4);
        lcd.print("' E"); }  
      else if (buttonSelect==4)  {
        lcd.clear(); 
        lcd.setCursor(0,0);  
        lcd.print("GPS SIM808 data");
        lcd.setCursor(0,1);  
        lcd.print("Bernd54Albrecht");  }                              
  //
  //    //************* Turn off the GPS power ************
  //    sim808.detachGPS();
      previousMillis = millis();
    }
  }
}

int Button()  {
  int A0;
  // all buttons are connected to A0 via voltage divider
  // Values of ADC are between 0 and 1023
  // if necessary, values must be changed slightly
  A0 = analogRead(0);        // 
  if (A0 < 60) {
    return 0;
  }
  else if (A0 >= 60 && A0 < 250) {
    return 1;
  }
  else if (A0 >= 250 && A0 < 450){
    return 2;
  }
  else if (A0 >= 450 && A0 < 700){
    return 3;
  }
  else if (A0 >= 700 && A0 < 900){
    return 4;
  }
  else {
    return -1;
  }
}   //end Button()

