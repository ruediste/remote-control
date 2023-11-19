/**
 * A simple example of sending data from 1 nRF24L01 transceiver to another.
 *
 * This example was written to be used on 2 devices acting as "nodes".
 * Use the Serial Monitor to change each node's behavior.
 */
#include <SPI.h>
#include "printf.h"
#include "RF24.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x3F for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 4);

// instantiate an object for the nRF24L01 transceiver
RF24 radio(PB0, PA4); // CE , CSN

// Let these addresses be used for the pair, 1: sender->transmitter 2: receiver->transmitter
uint8_t address[][6] = {"00001", "00002"};
// It is very helpful to think of an address as a path instead of as
// an identifying device destination

const int pinX1 = PA0;
const int pinY1 = PA1;
const int pinX2 = PA2;
const int pinY2 = PA3;
const int pinSW1 = PC14;
const int pinSW2 = PC15;

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

void setup()
{

  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("I2C LCD with");
  lcd.setCursor(0, 1);
  lcd.print("STM32F103C8T6");

  // initialize the transceiver on the SPI bus
  while (!radio.begin())
  {
    Serial.println(F("radio hardware is not responding!!"));
    delay(1000);
  }

  // print example's introductory prompt
  Serial.println(F("RF24/Transmitter"));

  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to
  // each other.
  // radio.setPALevel(RF24_PA_LOW); // RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_HIGH); // RF24_PA_MAX is default.
  // radio.setPALevel(RF24_PA_MIN); // RF24_PA_MAX is default.

  // save on transmission time by setting the radio to only transmit the
  // number of bytes we need to transmit a float
  radio.setPayloadSize(sizeof(Message)); // float datatype occupies 4 bytes

  // radio.setDataRate( RF24_250KBPS ) ;

  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[0]);
  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[1]); // using pipe 1

  // For debugging info
  // printf_begin();             // needed only once for printing details
  // radio.printDetails();       // (smaller) function that prints raw register values
  // radio.printPrettyDetails(); // (larger) function that prints human readable data

} // setup

unsigned long lastTime = 0;
void loop()
{

  if (millis() - lastTime > 1000)
  {
    lastTime = millis();
    Serial.println("ping");
  }

  // This device is a TX node

  message.x1 = analogRead(pinX1) / 1024.;
  message.y1 = analogRead(pinY1) / 1024.;
  message.x2 = analogRead(pinX2) / 1024.;
  message.y2 = analogRead(pinY2) / 1024.;
  message.sw1 = digitalRead(pinSW1);
  message.sw2 = digitalRead(pinSW2);

  unsigned long start_timer = micros();                 // start the timer
  radio.stopListening();                                // put radio in TX mode
  bool report = radio.write(&message, sizeof(Message)); // transmit & save the report
  unsigned long end_timer = micros();                   // end the timer

  if (true)
  {
    lcd.setCursor(0, 0);
    lcd.print(message.x1);
    lcd.print(" ");
    lcd.print(message.y1);
    lcd.print(" ");
    lcd.print(message.sw1);
    lcd.setCursor(0, 1);
    lcd.print(message.x2);
    lcd.print(" ");
    lcd.print(message.y2);
    lcd.print(" ");
    lcd.print(message.sw2);
  }
  if (report)
  {
    if (true)
    {
      Serial.print(F("Transmission successful! ")); // payload was delivered
      Serial.print(F("Time to transmit = "));
      Serial.print(end_timer - start_timer); // print the timer result
      Serial.println(F(" us"));
      // Serial.println(x1); // print payload sent
    }
  }
  else
  {
    Serial.println(F("Transmission failed or timed out")); // payload was not delivered
  }
  // delay(500);

} // loop
