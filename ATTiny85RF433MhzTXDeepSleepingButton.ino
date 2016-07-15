/*
  ATTiny85-RF433Mhz-TX-DeepSleepingButton v0.1
  by Jon Silver, http://jfdi.info
  "Standing on the shoulders of giants"
  
  Deep-sleeping ATTiny wakes up when button is pressed, and sends a fixed code via an RF TX
  
  HW: ATTiny85, 433MHz TX, push button, LED, resistor
  
  Dependencies: RF:   https://github.com/sui77/rc-switch/
*/

#include <RCSwitch.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#define TXPIN 1  //pin where your transmitter is connected
#define TXCODE 12345678
#define SWITCHPIN 3
#define STATUSLED 2


RCSwitch mySwitch = RCSwitch();

void setup() {
  mySwitch.enableTransmit(TXPIN);
  //mySwitch.setProtocol(1);
  //mySwitch.setRepeatTransmit(3);
  mySwitch.setPulseLength(309); // Pulse length for TX - 309 works with Status RF sockets, not so well with Maplin or Energenie

  pinMode(SWITCHPIN, INPUT);
  digitalWrite(SWITCHPIN, HIGH);
  pinMode(STATUSLED, OUTPUT);

  // Flash quick sequence so we know setup has started
  for (int k = 0; k < 5; k++) {
    if (k % 2 == 0) {
        digitalWrite(STATUSLED, HIGH);
        }
    else {
        digitalWrite(STATUSLED, LOW);
        }
    
    delay(250);
  }
} 


void sleep() {
    GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
    PCMSK |= _BV(PCINT3);                   // Use PB3 as interrupt pin
    ADCSRA &= ~_BV(ADEN);                   // ADC off
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

    sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    sei();                                  // Enable interrupts
    sleep_cpu();                            // sleep

    cli();                                  // Disable interrupts
    PCMSK &= ~_BV(PCINT3);                  // Turn off PB3 as interrupt pin
    sleep_disable();                        // Clear SE bit
    ADCSRA |= _BV(ADEN);                    // ADC on

    sei();                                  // Enable interrupts
} // sleep


ISR(PCINT0_vect) {
    // This is called when the interrupt occurs, but I don't need to do anything in it
}


void loop() {
    sleep();
    
    digitalWrite(STATUSLED, HIGH);
    
    mySwitch.send(TXCODE, 24);  // transmit the actual code
    delay(1000);
    
    digitalWrite(STATUSLED, LOW);
}

