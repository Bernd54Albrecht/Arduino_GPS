/* 
 GPS Receiver NEO-6M
 Connection: Software Serial communications
 
 RX is digital pin 17 of Arduino board (connect to TXD)
 TX is digital pin 18 of Arduino board (connect to RXD)
 */
//********************************************
#include <SoftwareSerial.h>   //include library code
//********************************************
SoftwareSerial GPSmodule(17, 18);//RX, TX
//**********************************************
void setup()   {
  GPSmodule.begin(9600);   
  Serial.begin(9600);  
  Serial.print("Easy GPS Receiver");  
  delay(100);
}

void loop()   {
  if (Serial.available()>0)
  GPSmodule.write(Serial.read());
  if (GPSmodule.available()>0)
  Serial.write(GPSmodule.read());
}
