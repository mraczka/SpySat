// rf95_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_server
// Tested with Anarduino MiniWirelessLoRa

#include <RH_RF95.h>
#include "wiring_private.h"
#include <Arduino.h>

// create Serial2 on pins 2,3
// RX-3, TX-2
Uart Serial2(&sercom2, 3, 2, SERCOM_RX_PAD_1, UART_TX_PAD_0);
void SERCOM2_Handler() {
  Serial2.IrqHandler();
}


// Singleton instance of the radio driver
RH_RF95<Uart> radio (Serial2);

void setup() {
  SerialUSB.begin(115200);
  Serial2.begin(9600);

  while(!SerialUSB);
  // assign pins the SERCOM functionality
  pinPeripheral(3, PIO_SERCOM_ALT);
  pinPeripheral(2, PIO_SERCOM_ALT);
  SerialUSB.println("radio test.");

  if (!radio.init()) {
    SerialUSB.println("init failed");
    while (1);
  }

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  //rf95.setTxPower(13, false);

  radio.setFrequency(433.0);
}

void loop() {
  SerialUSB.println("Sending to radio_server");
  // Send a message to rf95_server
  uint8_t data[] = "Hello World!";
  radio.send(data, sizeof(data));

  radio.waitPacketSent();

  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (radio.waitAvailableTimeout(3000)) {
    // Should be a reply message for us now
    if (radio.recv(buf, &len)) {
      SerialUSB.print("got reply: ");
      SerialUSB.println((char*)buf);
    } else {
      SerialUSB.println("recv failed");
    }
  } else {
    SerialUSB.println("No reply, is radio_server running?");
  }

  delay(1000);
}
