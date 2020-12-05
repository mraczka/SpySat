#include <OneWire.h>
#include <DallasTemperature.h>
#include <CanSatKit.h>
#include <SPI.h>
#include <SD.h>
#include <Servo.h>// biblioteka serwa
#include "Wire.h"
#include "Time.h"
#include "DS1307RTC.h"

using namespace CanSatKit;
//kodujesz tak szybko jak ja stoje
OneWire oneWire(A0);
DallasTemperature sensors(&oneWire);

Servo myservo;//tworzymy obiekt serva aby posługiwać się nim
File file;
BMP280 bmp;
bool led_state = true;
float PP; //defines Previous Preassure
float Ground; //defines ground variable
float FP; //defines Final Preassure
double T, P;
float distance;
bool canCheck = true;
bool canSDwrite = false;
//bool calibrate = false;

bool DBS1 = false; //Deploy Bio-Storage 1
bool DBS2 = false; //Deploy Bio-Storage 2
bool DBS3 = false; //Deploy Bio-Storage 3
bool CHTG = false; //CASNAT Hit The Ground

bool isdelay = true;

Radio radio(Pins::Radio::ChipSelect,
            Pins::Radio::DIO0,
            433.0,
            Bandwidth_125000_Hz,
            SpreadingFactor_9,
            CodingRate_4_8);
Frame frame;

void setup() {
  myservo.attach(9); //definiujemy pin na którym jest servo - pin 9
  myservo.write(10);
  while (!SerialUSB);
  SerialUSB.begin(9600);

  sensors.begin();

  pinMode(5, INPUT);
  pinMode(4, OUTPUT); //additional 5 voltage pin
  digitalWrite(4, HIGH);
  pinMode(LED_BUILTIN, OUTPUT);

  // use begin() function to check if sensor is conneted and is able to communicate
  if (!bmp.begin()) {
    SerialUSB.println("BMP init failed!");
    while (1);
  } else {
    SerialUSB.println("BMP init success!");
  }

  if (!SD.begin(11)) {
    SerialUSB.println("SD >> Card failed, or not present");
    canSDwrite = false;
  } else {
    SerialUSB.println("SD >> card present");
    canSDwrite = true;
  }

  bmp.setOversampling(16);
  radio.begin();

  bmp.measureTemperatureAndPressure(T, P);
  if (digitalRead(5) == HIGH) {
    Ground = P;
    SerialUSB.println("Ground Calibrated");

  } else {
    SerialUSB.println("Ground not Calibrated - please calibrate");
    while (digitalRead(5) == LOW);
  }

  tmElements_t tm;
  if (RTC.read(tm)) {
    SerialUSB.println("URUCHOMIENIE O : ");
    SerialUSB.print(tm.Hour); //Wyswietlenie godzin
    SerialUSB.print(":");
    SerialUSB.print(tm.Minute);//Minut
    SerialUSB.print(":");

    SerialUSB.print(tm.Day);
    SerialUSB.print('/');
    SerialUSB.print(tm.Month);
    SerialUSB.print('/');
    SerialUSB.print(tmYearToCalendar(tm.Year));
  }
  else {
    SerialUSB.println ("SOMETHING WENT WRONG");
    while (1);
  }

  PP = P + 1;
}

void loop() {

  sensors.requestTemperatures();
  tmElements_t tm;

  isdelay = true;
  bmp.measureTemperatureAndPressure(T, P);

  digitalWrite(LED_BUILTIN, led_state); //blink
  led_state = !led_state;
  //wyświetlanie czasu i daty
  frame.print(tm.Hour); 
  frame.print(":");
  frame.print(tm.Minute);
  frame.print(":");
  frame.print(tm.Day);
  frame.print('/');
  frame.print(tm.Month);
  frame.print('/');
  frame.print(tmYearToCalendar(tm.Year));
  //odczyty
  frame.print("P = ");
  frame.print(P);
  frame.println(" hPa");
  frame.print("P.P = ");
  frame.print(PP);
  frame.println(" hPa");
  frame.print("T.ins. = ");
  frame.print(T);
  frame.println("C");
  frame.print("T.out. =  ");
  frame.print(sensors.getTempCByIndex(0));
  frame.println("C");

  //send frame by radio
  radio.transmit(frame);

  //send frame to serial port
  SerialUSB.println(frame);

  //save on SD if present
  if (canSDwrite == true) {
    file = SD.open("Readings.txt", FILE_WRITE);
    SerialUSB.println("SD>Saving on SD");
    file.println(frame);
    file.close();
  }

  frame.clear();

  //checks for the cansat highest position
  if ((PP + 0.1) < P & canCheck == true) { // w trakcie lotu liczby trzeba zmienić
    SerialUSB.println("2.CANSAT HIGHEST POSITION");
    radio.transmit("2.CANSAT HIGHEST POSITION");
    //save on SD
    file = SD.open("Readings.txt", FILE_WRITE);
    file.println("2.CANSAT HIGHEST POSITION");

    FP = PP;
    distance = Ground - FP;
    canCheck = false;
    file.println(distance);
    file.close();

    SerialUSB.println(distance);
    //    radio.transmit(distance);
    DBS1 = true; //Deploy Bio-Storage 1
    DBS2 = true; //Deploy Bio-Storage 2
    DBS3 = true; //Deploy Bio-Storage 3
    CHTG = true; //CASNAT Hit The Ground
  }

  file = SD.open("Readings.txt", FILE_WRITE);

  if (((distance / 4) + FP) <= P & P <= ((distance / 4) + FP + 0.2) & DBS1 == true) { // w trakcie lotu liczby trzeba zmienić

    //jeśli distance (różnica ciśnienia od ziemi do najwyższego punktu) / 4 dodać finalne ciśnienie
    //jest mniejsze od obecnego ciśnienia to cansat otwiera 1. probówkę

    SerialUSB.println("3.DEPLOY FIRST BIO-STORAGE");
    radio.transmit("3.DEPLOY FIRST BIO-STORAGE");
    //save on SD
    file.println("3.DEPLOY FIRST BIO-STORAGE");

    myservo.write(90);
    delay(500);
    myservo.write(120);
    delay(500);

    DBS1 = false;
    isdelay = false;
  }
  if ((((distance / 4) * 2) + FP) <= P & P <= (((distance / 4) * 2) + FP + 0.2) & DBS2 == true) {

    // w trakcie lotu liczby trzeba zmienić

    SerialUSB.println("4.DEPLOY SECOND BIO-STORAGE");
    radio.transmit("4.DEPLOY SECOND BIO-STORAGE");
    //save on SD
    file.println("4.DEPLOY SECOND BIO-STORAGE");

    myservo.write(150);
    delay(500);
    myservo.write(170);
    delay(500);
    DBS2 = false;
    isdelay = false;
  }
  if ((((distance / 4) * 3) + FP) <= P & P <= (((distance / 4) * 3) + FP + 0.2) & DBS3 == true) {

    // w trakcie lotu liczby trzeba zmienić

    SerialUSB.println("5.DEPLOY THIRD BIO-STORAGE");
    radio.transmit("5.DEPLOY THIRD BIO-STORAGE");
    
    file.println("5.DEPLOY THIRD BIO-STORAGE");
    
    delay(500);
    myservo.write(60);
    delay(500);
    DBS3 = false;
    isdelay = false;
  }
  if (((Ground - 0.3) <= P) & CHTG == true) {

    // w trakcie lotu liczby trzeba zmienić

    SerialUSB.println("6.CANSAT HIT THE GROUND");
    radio.transmit("6.CANSAT HIT THE GROUND");
    //save on SD
    file.println("6.CANSAT HIT THE GROUND");
    CHTG = false;
  }
  if (CHTG = false) {
    SerialUSB.println("CANSAT IS ON THE GROUND");
    radio.transmit("CANSAT IS ON THE GROUND");
    file.println("CANSAT IS ON THE GROUND");
  }

  PP = P;//updates Previous Preassure
  file.close();

  if (isdelay = true) {
    delay(1000);
  }

}
