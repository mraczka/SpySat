#include <CanSatKit.h>
#include <SPI.h>
#include <SD.h>

using namespace CanSatKit;

// set radio receiver parameters - see comments below
// remember to set the same radio parameters in
// transmitter and receiver boards!
Radio radio(Pins::Radio::ChipSelect,
            Pins::Radio::DIO0,
            433.0,                  // frequency in MHz
            Bandwidth_125000_Hz,    // bandwidth - check with CanSat regulations to set allowed value
            SpreadingFactor_9,      // see provided presentations to determine which setting is the best
            CodingRate_4_8);        // see provided presentations to determine which setting is the best
            
File file;
bool canSDwrite;
void setup() {
  SerialUSB.begin(115200);
if (!SD.begin(11)) {
    SerialUSB.println("SD >> Card failed, or not present");
    canSDwrite = false;
  } else {
    SerialUSB.println("SD >> card present");
    canSDwrite = true;
  }
  // checking if SD card is present
  radio.begin();// start radio module  
}

void loop() {
  // prepare empty space for received frame
  // maximum length is maximum frame length + null termination
  // 255 + 1 byte = 256 bytes
  char data[256];

  // receive data and save it to string
  radio.receive(data);
  
  // get and print signal level (rssi)
  SerialUSB.print("RSSI = ");
  SerialUSB.println(radio.get_rssi_last());
  if (canSDwrite == true) {
    file = SD.open("Readings.txt", FILE_WRITE);
    SerialUSB.println("SD>Saving");
    file.println(data);
    file.print("RRSI: ");
    file.println(radio.get_rssi_last());
    file.close();
}
}
