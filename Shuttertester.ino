#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#define I2C_ADDRESS 0x3C                  // change this to your I2C address for the display

// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiWire oled;

long Start;   // this is the time in microseconds that the shutter opens (the arduino runs a microsecond clock in the background always - it is reasonably accurate for this purpose)
long Stop;    // this is the time in microseconds that the shutter closes
int Fired = 0;  // this is a flag indicating when the shutter has been fired completely.  when fired =1, the shutter has been fired, and the computer needs to display the information related to the exposure time.
int Risingflag = 0;  // this is a flag that i set in my interrupt routine, Rising flag is set to = 1 when the voltage INCREASES in the interrupt
int Fallingflag = 0;  // this is a flag that i set in the interrupt routine, Fallingflag is set to =1 when the voltage DECREASES in the interrupt
int laserreceiverpin = 2; // pin connected to laser receiver
float voltage;

void displaytemplate() {
    oled.clear();
    oled.set1X();
    oled.setCursor(0, 0);
    oled.print(F("Shutter Speed Tester"));
    printdivider(6);
    printdivider(1);
    oled.setCursor(0, 2);
    oled.print(F("Shutter|"));
    oled.setCursor(0, 3);
    oled.print(F("  Speed|"));
    oled.setCursor(0, 4);
    oled.print(F("  Milli|"));
    oled.setCursor(0, 5);
    oled.print(F("Seconds|"));
    oled.setCursor(0,7);
    oled.print(F("Battery Level: "));
    oled.print(voltage, 1);
    oled.print(F("v"));
}

void printdivider(int row) {                // prints a divider
  oled.set1X();
  oled.setCursor(0, row);
  int count = 1;
  while (count < 23) {                      // 22 characters wide
    oled.print(F("-"));                     // choose your divider character...
   count++;
}
}

void setup() {                                                  //This part of the program is run exactly once on boot

Wire.begin();
oled.begin(&Adafruit128x64, I2C_ADDRESS);
oled.setFont(Adafruit5x7);

attachInterrupt(digitalPinToInterrupt(laserreceiverpin), CLOCK, CHANGE);    //run the function CLOCK, every time the voltage on pin 2 changes.

// get voltage
const long InternalReferenceVoltage = 1056L;  // Adjust this value to your boards specific internal BG voltage x1000
ADMUX = (0 << REFS1) | (1 << REFS0) | (0 << ADLAR) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0);
delay(100);  // Let mux settle a little to get a more stable A/D conversion
ADCSRA |= _BV( ADSC );
while ( ( (ADCSRA & (1 << ADSC)) != 0 ) );
voltage = (((InternalReferenceVoltage * 1024L) / ADC) + 5L) / 10L; // calculates for straight line value
voltage = voltage/100;


displaytemplate();

}

void loop() {                                                  // this part of the program is run, in order, over and over again, start to finish, unless INTERRUPTED by our interrupt
  if(Risingflag ==1){                       
    Start = micros();       //set the variable Start to current microseconds
  Risingflag=0;           //reset the rising flag to 0, so that this function isnt called again until the shutter actually fires
  }
  if(Fallingflag == 1){
  Stop = micros();      // set the variable Stop to current microseconds
  Fallingflag = 0;      //reset the falling flag to 0, so that this function isnt called again untill the shutter fires again.
  Fired = 1;            // set the fired flag to 1, triggering the calculation of a shutter speed, and its display over the serial monitor.
  }
  if(Fired == 1){                            //if the flag Fired = 1, print this information to the serial monitor"
    long speed = (Stop - Start);
    float T = (float)speed/1000000;
    uint8_t Tdisplay = 1;                       // Flag for shutter speed display style (fractional, seconds, minutes)
    double  Tfr = 0;                            // value for shutter speed fraction
    float   Tmin = 0;                           // shutter speed in minutes, if over 60 seconds
     if (T >= 60) {
       Tdisplay = 0;                             // Exposure is in minutes
     Tmin = T / 60;
  } else if (T < 60 && T >= 0.5) {            // speed in seconds
    Tdisplay = 2;                             // Exposure in in seconds
     } else if (T < 0.5) {            // speed in fractions
      Tdisplay = 1;                             // Display is in fractional form
      Tfr = 1 / T;
    }

    displaytemplate();
    oled.set2X();
    oled.setCursor(55, 2);
    if (Tdisplay == 0) {                // display shutter speed
    oled.print(Tmin, 1);              // in minutes
    oled.print(F("m"));
     } 
    if (Tdisplay == 1) {                // or in fractions
     oled.print(F("1/"));
       oled.print(Tfr, 0);
    }
     if (Tdisplay == 2) {                // or in seconds
      oled.print(T, 1);
      oled.print(F("s"));
   }
    oled.setCursor(55, 4);
    float ms(speed / 1000);
    oled.print(ms, 1);         // print time in microseconds

    Start = 0;                         // reset Start to 0
    Stop = 0;                           //reset Stop to 0 . *** these are not necessarily needed, but makes errors more evident should they occur
    Fired = 0;                          //reset Fired flag to 0, so that the shutter speed will not be calclulated and displayed, until the next full interrupt cycle, where a start and stop time are generated.
  } 
}

void CLOCK(){                     //this is the interrupt function, which is called everytime the voltage on pin 2 changes, no matter where in the main program loop that the computer is currently in
  if(digitalRead(2) == HIGH){
    Risingflag = 1;                // if the voltage on pin 2 is high, set the Risingflag to 1 : this will trigger the function called Rising from the main loop, which will set a start time
  }
  if(digitalRead(2) == LOW){        // . if the voltage on pin 2 is low, set the Fallingflag to 1 : this will trigger the function called Falling from the main loop, which will set the stop time, and also set the Fired flag to 1.
    Fallingflag =1;
  }
}
