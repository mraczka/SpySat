#include <CanSatKit.h>
#include <cmath>
using namespace CanSatKit;
//definition of temperature recording
const int lm35_pin = A0;
float lm35_raw_to_temperature(int raw) {
  float voltage = raw * 3.3 / (std::pow(2, 12));
  float temperature = 100.0 * voltage;
  return temperature;
}
//offsets and BMP
float offset_analog = 0;
float offset_digital = 0;
float offset_pressure = 0;
BMP280 bmp;

//radio
Radio radio(Pins::Radio::ChipSelect,
            Pins::Radio::DIO0,
            433.0,
            Bandwidth_125000_Hz,
            SpreadingFactor_9,
            CodingRate_4_8);
Frame frame;

void setup() {
  // put your setup code here, to run once:
  //starting radio and Serial
  radio.begin();
  SerialUSB.begin(9600);
  //checking if BMP280 connected
  if (!bmp.begin()) {
    SerialUSB.println("BMP init failed!");
    while (1);

  } else {
    frame.println("BMP init success!");
    radio.transmit("BMP init success");
  }
  bmp.setOversampling(16);
  analogReadResolution(12);
}

void loop() {
  // put your main code here, to run repeatedly:
  //reading data from sensors
  double T, P;
  int raw = analogRead(lm35_pin);
  float temperature = (lm35_raw_to_temperature(raw) + offset_analog);
  bmp.measureTemperatureAndPressure(T, P);

  //writing data reading to frame
  frame.print("Analog:  ");
  frame.print(temperature);
  frame.print("   Digital: ");
  frame.print((T + offset_digital), 2);
  frame.print("   Pressure: ");
  frame.print((P + offset_pressure), 2);
  //sending data
  radio.transmit(frame);
  SerialUSB.println(frame);
  frame.clear();
  delay(1000);
}
