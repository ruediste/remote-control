/**
 * A simple example of sending data from 1 nRF24L01 transceiver to another.
 *
 * This example was written to be used on 2 devices acting as "nodes".
 * Use the Serial Monitor to change each node's behavior.
 */
#include <SPI.h>
#include "printf.h"
#include "RF24.h"

// instantiate an object for the nRF24L01 transceiver
RF24 radio(PB0, PA4); //CE , CSN

// Let these addresses be used for the pair, 1: sender->transmitter 2: receiver->transmitter
uint8_t address[][6] = {"00001", "00002"};
// It is very helpful to think of an address as a path instead of as
// an identifying device destination

const int pinOut = PA8;
void setup()
{

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pinOut, OUTPUT);
  analogWriteFrequency(50);
  // analogWriteResolution(255);

  Serial.begin(115200);


  // initialize the transceiver on the SPI bus
  if (!radio.begin())
  {
    Serial.println(F("radio hardware is not responding!!"));
    while (1)
    {
    } // hold in infinite loop
  }

  // print example's introductory prompt
  Serial.println(F("RF24/Receiver"));

  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to
  // each other.
  radio.setPALevel(RF24_PA_LOW); // RF24_PA_MAX is default.

  // save on transmission time by setting the radio to only transmit the
  // number of bytes we need to transmit a float
  radio.setPayloadSize(sizeof(float)); // float datatype occupies 4 bytes

  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[1]); // always uses pipe 0

  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[0]); // using pipe 1

  radio.startListening(); // put radio in RX mode

  // For debugging info
  // printf_begin();             // needed only once for printing details
  // radio.printDetails();       // (smaller) function that prints raw register values
  // radio.printPrettyDetails(); // (larger) function that prints human readable data

} // setup

void loop()
{

  // This device is a RX node

  uint8_t pipe;
  if (radio.available(&pipe))
  {                                         // is there a payload? get the pipe number that recieved it
    uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
    float payload;
    radio.read(&payload, bytes); // fetch payload from FIFO
    if (false)
    {
      Serial.print(F("Received "));
      Serial.print(bytes); // print the size of the payload
      Serial.print(F(" bytes on pipe "));
      Serial.print(pipe); // print the pipe number
      Serial.print(F(": "));
      Serial.println(payload); // print the payload's value
    }

    // period: 20ms
    // 0 -> 1ms
    // 1 -> 2.5ms
    // 1ms=65536*1/20
    // 2ms=65536*2/20
    analogWrite(pinOut, 256. / 20. * (0.4 + (2.5-0.4)*payload));
    digitalWrite(LED_BUILTIN, payload > 0.5 ? 1 : 0);
  }

} // loop
