/**
 * A simple example of sending data from 1 nRF24L01 transceiver to another.
 *
 * This example was written to be used on 2 devices acting as "nodes".
 * Use the Serial Monitor to change each node's behavior.
 */
#include <SPI.h>
#include "printf.h"
#include "RF24.h"
// #include <Servo.h>

// instantiate an object for the nRF24L01 transceiver
// PA4: CS
// PA5: SCK
// PA6: MISO
// PA7: MOSI
// PB0: CE
RF24 radio(PB0, PA4); // CE , CS

// Let these addresses be used for the pair, 1: sender->transmitter 2: receiver->transmitter
uint8_t address[][6] = {"00001", "00002"};
// It is very helpful to think of an address as a path instead of as
// an identifying device destination

// const int pinOut = PA8;
const int pinD1A = PB4;
const int pinD1B = PB5;
const int pinD2A = PB6;
const int pinD2B = PB7;
const int pinEN1 = PB3;
const int pinEN2 = PB9;

const double threshold = 0.1;

typedef struct
{
  float x1;
  float y1;
  float x2;
  float y2;
  bool sw1;
  bool sw2;
} Message;

Message message;
// Servo servo1;

void setup()
{

  pinMode(PC13, OUTPUT);
  pinMode(pinD1A, OUTPUT);
  pinMode(pinD1B, OUTPUT);
  pinMode(pinD2A, OUTPUT);
  pinMode(pinD2B, OUTPUT);
  analogWriteFrequency(1000);
  analogWriteResolution(8);

  Serial.begin(115200);

  // servo1.attach(PA8);

  // initialize the transceiver on the SPI bus
  while (!radio.begin())
  {
    Serial.println(F("radio hardware is not responding!!"));
    delay(1000);
  }

  // print example's introductory prompt
  Serial.println(F("RF24/Receiver"));

  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to
  // each other.
  radio.setPALevel(RF24_PA_LOW); // RF24_PA_MAX is default.

  // save on transmission time by setting the radio to only transmit the
  // number of bytes we need to transmit a float
  radio.setPayloadSize(sizeof(Message)); // float datatype occupies 4 bytes

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

unsigned long lastTime = 0;
void loop()
{
  // This device is a RX node

  if (millis() - lastTime > 1000)
  {
    lastTime = millis();
    Serial.println("ping receiver");
  }

  uint8_t pipe;
  if (radio.available(&pipe))
  {                                        // is there a payload? get the pipe number that recieved it
    radio.read(&message, sizeof(Message)); // fetch payload from FIFO
    if (true)
    {
      Serial.print(F("Received "));
      Serial.print(message.x1);
      Serial.print(F(" "));
      Serial.print(message.y1);
      Serial.print(F(" "));
      Serial.print(message.sw1);
      Serial.print(F(" "));
      Serial.print(message.x2);
      Serial.print(F(" "));
      Serial.print(message.y2);
      Serial.print(F(" "));
      Serial.println(message.sw2);
    }

    // period: 20ms
    // 0 -> 1ms
    // 1 -> 2.5ms
    // 1ms=65536*1/20
    // 2ms=65536*2/20
    // analogWrite(pinOut, 256. / 20. * (0.4 + (2.5 - 0.4) * message.x1));
    // servo1.write(180. * message.x1);

    digitalWrite(PC13, message.x1 > 0.5 ? 1 : 0);

    double value1 = message.y1;
    if (value1 > 0.5 + threshold)
    {
      digitalWrite(pinD1A, 1);
      digitalWrite(pinD1B, 0);
      analogWrite(pinEN1, (value1 - 0.5) * 2 * 255);
    }
    else if (message.y1 < 0.5 - threshold)
    {
      digitalWrite(pinD1A, 0);
      digitalWrite(pinD1B, 1);
      analogWrite(pinEN1, (0.5 - value1) * 2 * 255);
    }
    else
    {
      digitalWrite(pinD1A, 0);
      digitalWrite(pinD1B, 0);
    }

    double value2 = message.y2;
    if (value2 > 0.5 + threshold)
    {
      digitalWrite(pinD2A, 1);
      digitalWrite(pinD2B, 0);
      analogWrite(pinEN2, (value2 - 0.5) * 2 * 255);
    }
    else if (message.y2 < 0.5 - threshold)
    {
      digitalWrite(pinD2A, 0);
      digitalWrite(pinD2B, 1);
      analogWrite(pinEN2, (0.5 - value2) * 2 * 255);
    }
    else
    {
      digitalWrite(pinD2A, 0);
      digitalWrite(pinD2B, 0);
    }
  }

} // loop
