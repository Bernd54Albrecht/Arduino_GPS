/* Based on SoftwareSerialExample
   all characters after $ will be concatenated
   and printed, when the next $ appears
   Do not forget to press the push button
 */
#include <SoftwareSerial.h>
SoftwareSerial mySerial(16,17); // RX, TX
String GPRMC ="GPRMC,084050.000,A,5354.6792,N,00952.2214,E,0.10,116.76,181124,,,A*68";

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial.println("Press Start Button for more than 1 second");
  Serial.println("before Command AT+CGPSPWR=1 is sent");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  mySerial.println("AT+CGPSPWR=1");
  delay(5000);
  mySerial.println("AT+CGPSINF=32");   // single NMEA GPRMC data output
  mySerial.println("AT+CGPSOUT=32");   // enable NMEA GPRMC data output
}

void loop() { // run over and over
  if (mySerial.available()) {
    char c = mySerial.read();
    if (c == '$')    {
      Serial.print("GPRMC output: ");
      Serial.println(GPRMC);
      GPRMC.replace(GPRMC,"");
    }
    else   {
        GPRMC.concat(c);
    }
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
}
