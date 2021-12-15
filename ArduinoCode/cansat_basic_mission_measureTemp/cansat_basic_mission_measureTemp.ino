#include <Arduino.h>
#include "wiring_private.h"
#include <TinyGPS++.h>
#include <CanSatKit.h>
#include <cmath>
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

const int lm35_pin = A0; //LM35 temperature port
const int lm35_power_pin = 5;
const int bmp_sensor_power_pin = 4;
bool info_led_state = true;


// a function that calculates temperature (in *C) from analogRead raw reading
// see datasheet of LM35 and analogRead Arduino function documentations
float lm35_raw_to_temperature(int raw) {
  float voltage = raw * 3.3 / (std::pow(2, 12));
  float temperature = 100.0 * voltage;
  return temperature;
}

void setup() {
  SerialUSB.begin(115200);
  Serial2.begin(9600);

  // assign pins the SERCOM functionality
  pinPeripheral(2, PIO_SERCOM_ALT);
  pinPeripheral(3, PIO_SERCOM_ALT);
  
  pinMode(13, OUTPUT);
  pinMode(bmp_sensor_power_pin, OUTPUT);
  pinMode(lm35_power_pin, OUTPUT);
 
  radio.begin(); // init radio

  // use begin() function to check if sensor is conneted and is able to communicate
  // with the CanSat board
  if(bmpSensor.begin()) {SerialUSB.println("BMP init success!");}
  else {SerialUSB.println("BMP init failed!");}

  // setOversampling function allows to set resolution of the pressure sensor
  // possible values of setOversampling:
  //  1 -- 2.62 Pa
  //  2 -- 1.31 Pa
  //  4 -- 0.66 Pa
  //  8 -- 0.33 Pa
  // 16 -- 0.16 Pa 
  bmpSensor.setOversampling(16);

  // set the best resolution of measurement available for analogRead
  // it's 12 bits for SAM21D-based Arduino boards
  analogReadResolution(12);

  SerialUSB.println("setup complete!"); //debug message
}

void loop() {
/*
  // Get GPS sensor data-------------------
  // check if Serial2 has data available from the GPS, then encode the GPS data. 
  // When the GPS is done transmiting data, the while stops.
  while(Serial2.available()){
    gps.encode(Serial2.read());
  }

  if(gps.location.isUpdated()){
    frame.println("Position: ");
    frame.print("Latitude:");
    frame.println(gps.location.lat(), 6);
    frame.print("Longitude:");
    frame.println(gps.location.lng(), 6);
  }
*/
  
  // Get BMP sensor data-----------------------
  if(bmpSensor.begin()){
    double T, P;
    digitalWrite(bmp_sensor_power_pin, HIGH);
    // start measurement, wait for result and save variables
    bmpSensor.measureTemperatureAndPressure(T, P);
//    digitalWrite(bmp_sensor_power_pin, LOW);
    frame.print("Pressure = ");
    frame.println(P, 2);
//    frame.println(" hPa");
    frame.print("Temperature = ");
    frame.println(T, 2);
//    frame.println(" deg C");
  }

  // Get Analog temperature sensor data------------------
  
  // read raw analog value from lm35_pin analog input
  digitalWrite(lm35_power_pin, HIGH);
  int raw = analogRead(lm35_pin);
//    digitalWrite(lm35_power_pin, LOW);
    
  // use lm35_raw_to_temperature function to calculate temperature
  float temperature = lm35_raw_to_temperature(raw);

  frame.print("Analog Temperature = ");
  frame.println(temperature);
//  frame.println(" deg C");

  
  // tsransmit frame-----------------------------
  radio.transmit(frame);
  SerialUSB.println(frame);
  
  frame.clear();

  digitalWrite(13, info_led_state);
  info_led_state = !info_led_state;
  delay(1000);
}
