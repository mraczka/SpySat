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


bool info_led_state = true;


void setup() {
  SerialUSB.begin(115200);
  Serial2.begin(9600);

  // assign pins the SERCOM functionality
  pinPeripheral(2, PIO_SERCOM_ALT);
  pinPeripheral(3, PIO_SERCOM_ALT);
  
  pinMode(13, OUTPUT);
 
  radio.begin(); // init radio

  SerialUSB.println("setup complete!"); //debug message
}

void loop() {

  // check if Serial2 has gps data
  if(Serial2.available()){
    // encode GPS data
     
    if(gps.encode(Serial2.read())){
      
      // write gps position to frame
      frame.println("Position: ");
      frame.print("Latitude:");
      frame.println(gps.location.lat(), 6);
      frame.print("Longitude:");
      frame.println(gps.location.lng(), 6);
      // transmit frame
      SerialUSB.println(frame);
      frame.clear();
      delay(1000);
    }
  }
  

}
