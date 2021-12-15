#include <Arduino.h>
#include "wiring_private.h"
#include <TinyGPS++.h>
#include <CanSatKit.h>
using namespace CanSatKit;


// create Serial2 on pins 2,3
// RX-3, TX-2
Uart Serial2 (&sercom2, 3, 2, SERCOM_RX_PAD_1, UART_TX_PAD_0);
void SERCOM2_Handler(){Serial2.IrqHandler();}


// create radio object
Radio radio(Pins::Radio::ChipSelect,
            Pins::Radio::DIO0,
            433.0,
            Bandwidth_125000_Hz,
            SpreadingFactor_9,
            CodingRate_4_8);

TinyGPSPlus gps; // create GPS object
Frame frame; // create empty frame
BMP280 bmpSensor; // create bmp object


bool info_led_state = true;


void setup() {
  SerialUSB.begin(115200);
  Serial2.begin(9600);

  // assign pins the SERCOM functionality
  pinPeripheral(2, PIO_SERCOM_ALT);
  pinPeripheral(3, PIO_SERCOM_ALT);
  
  pinMode(13, OUTPUT);
 
  radio.begin(); // init radio

  // use begin() function to check if sensor is conneted and is able to communicate
  // with the CanSat board
  if(bmpSensor.begin()) {SerialUSB.println("BMP init success!");}
  else {SerialUSB.println("BMP init failed!");}

  bmpSensor.setOversampling(16);
  // setOversampling function allows to set resolution of the pressure sensor
  // possible values of setOversampling:
  //  1 -- 2.62 Pa
  //  2 -- 1.31 Pa
  //  4 -- 0.66 Pa
  //  8 -- 0.33 Pa
  // 16 -- 0.16 Pa 


  SerialUSB.println("setup complete!"); //debug message
}

void loop() {

  // Get GPS sensor data--------
  
  bool GPSwhile = true;
  // check if Serial2 has gps data
  while(Serial2.available() && GPSwhile == true){
    // encode GPS data
    if(gps.encode(Serial2.read())){
      GPSwhile = false;
      frame.println("Position: ");
      frame.print("Latitude:");
      frame.println(gps.location.lat(), 6);
      frame.print("Longitude:");
      frame.println(gps.location.lng(), 6);
    }
  }

  // Get BMP sensor data-----------
  
  if(bmpSensor.begin()){
    double T, P;
    // start measurement, wait for result and save variables
    bmpSensor.measureTemperatureAndPressure(T, P);
    frame.print("Pressure = ");
    frame.print(P, 2);
    frame.println(" hPa");

  } else {SerialUSB.println("BMP unavailable");}

  
  // transmit frame
  radio.transmit(frame);
  SerialUSB.println(frame);
  
  frame.clear();
  delay(1000);
}
