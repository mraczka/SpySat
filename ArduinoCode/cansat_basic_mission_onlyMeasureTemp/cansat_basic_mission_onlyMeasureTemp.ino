#include <CanSatKit.h>
#include <cmath>
using namespace CanSatKit;
using namespace std

  Frame frame; // create empty frame
BMP280 bmpSensor; // create bmp object

const int lm35_pin = A0; //LM35 temperature port
const int lm35_power_pin = 5;
const int bmp_sensor_gnd_pin = 4;
bool info_led_state = true;

int averagegNum = 1;



// a function that calculates temperature (in *C) from analogRead raw reading
// see datasheet of LM35 and analogRead Arduino function documentations
float lm35_raw_to_temperature(int raw) {
  float voltage = raw * 3.3 / (std::pow(2, 12));
  float temperature = 100.0 * voltage;
  return temperature;
}

void setup() {
  SerialUSB.begin(115200);

  pinMode(13, OUTPUT);
  pinMode(bmp_sensor_gnd_pin, OUTPUT);
  pinMode(lm35_power_pin, OUTPUT);

  // use begin() function to check if sensor is conneted and is able to communicate
  // with the CanSat board
  if (bmpSensor.begin()) {
    SerialUSB.println("BMP init success!");
  }
  else {
    SerialUSB.println("BMP init failed!");
  }

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

  // Get BMP sensor data-----------------------
  if (bmpSensor.begin()) {
    double T, P;
    digitalWrite(bmp_sensor_gnd_pin, HIGH);
    // start measurement, wait for result and save variables
    bmpSensor.measureTemperatureAndPressure(T, P);
    // digitalWrite(bmp_sensor_gnd_pin, LOW);

    frame.print("bmp_temp_= ");
    frame.print(T, 2);
    frame.print("\t");
  }


  // Get Analog temperature sensor data------------------

  // read raw analog value from lm35_pin analog input
  digitalWrite(lm35_power_pin, HIGH);
  int raw = analogRead(lm35_pin);
  //  digitalWrite(lm35_power_pin, LOW);

  // use lm35_raw_to_temperature function to calculate temperature
  float temperature = lm35_raw_to_temperature(raw);

  // Temperature avarage variables
  float reading;
  float averageOUT;

  reading =+ temperature;
  averageOUT = reading/averageNum;
  averageNum++;

  frame.print("analog_temp_= ");
  frame.print(avarageOut);
  frame.println("\t");

  // tsransmit frame-----------------------------
  SerialUSB.println(frame);

  frame.clear();

  digitalWrite(13, info_led_state);
  info_led_state = !info_led_state;
  delay(1000);
}
