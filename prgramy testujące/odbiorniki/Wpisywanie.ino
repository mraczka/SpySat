#include <CanSatKit.h>
#include <SPI.h>
#include <SD.h>
String metry = ""; //zmienna definiująca nadajnika od odbiornika

using namespace CanSatKit;

void setup() {
  // put your setup code here, to run once:
SerialUSB.begin(115200);
 delay(5000);
metry = Serial.readStringUntil('\n') ;
 SerialUSB.println("Witaj"+metry);
  
  
  }

void loop() {
  // put your main code here, to run repeatedly:

}
