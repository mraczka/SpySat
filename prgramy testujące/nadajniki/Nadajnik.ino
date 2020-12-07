/* Jest to klasyczny program wysyłający pustą date. Jego celem jest wykorzystanie do pomiarów mocy sygnału w zależności od odległości
Jest to wersja podstawowa - można ją dowolnie zmieniać i np. ustawiać to co program ma wysyłać poprzez dodanie:
frame.print("[to co chcemy wpisać); 
w odpowiednim miejscu w kodzie
*/

#include <CanSatKit.h>

using namespace CanSatKit;

// Remember to set the same radio parameters in
// transmitter and receiver boards!
Radio radio(Pins::Radio::ChipSelect,
            Pins::Radio::DIO0,
            433.0,
            Bandwidth_125000_Hz,
            SpreadingFactor_9,
            CodingRate_4_8);
Frame frame ;

void setup() {
  radio.begin();
}

void loop() {
  radio.transmit(frame);
  frame.clear();
  delay(1000);
}
