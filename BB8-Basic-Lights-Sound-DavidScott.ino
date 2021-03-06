/*
    ===================================================================================================
    ==================================== BB-8 PADAWAN DOME ============================================
    ======================================  Version 0.07  =============================================
    ===================================================================================================

    Version History
    0.01 - Original Sketch by DavidScott
    0.02 - r0n_dL added annotations & variable for PIN_sound_BUSY as suggested by DavidScott
    0.03 - r0n_dL reassigned pins to align with Padawan control system
         - Changed SoftwareSerial pins (RX/TX) 10/9 to 8/4
    0.04 - Change pinMode for PIN_trigger to INPUT_PULLUP to resolve issue using with Pro Micro
    0.05 - Added support for the BY8301-16P and BY8001-16P sound modules
    0.06 - Using an updated version of MP3FLASH16P (MP3Flash16Pv2) which now removes static serial
           mappings out of the .cpp file
         - Add support to select between using 3 different sound modules by changing 1 variable
    0.07 - Changed hard coded serial port in library

    r0n_dL IMPORTANT NOTE & RECOGNITON: This sketch was written by DavidScott. Thanks for a great base!

    // DavidScott's original comment below
    
    // This example plays a random file (001 to 010) forever
    // It uses the playFileAndWait() command so no extra code is needed, it will play another file as soon as 
    // the previous one finishes. If it doesn't work, try reversing the RX/TX wires as that's probably going to be
    // the most common mistake. Also check that the player's BUSY line is connected to Arduino pin 3
    // Finally check that the player has a speaker connected as well as ground and VCC

    r0n_dL NOTES:  Basic sketch provided by DavidScott to give sounds and synchronized voice lights for 
                      scale BB-8 Domes

    REQUIRED Libaries to make this sketch work:
    
    MP3Flash16Pv2 Library  
    https://github.com/r0ndL/MP3Flash16Pv2
    
    BY8x0116P Library
    https://github.com/r0ndL/BY8x0116Pv2


    r0n_dL PARTS USED
    
    1 Arduino Pro Micro
    1 MP3-FLASH-16P MP3 Soundboard
    1 Red 3mm Flanged LED (Radar Eye)*
    1 Blue 3mm Flanged LED (Side Logic Display)*
    1 White 3mm Flanged LED (Top Front Logic Display)*
    1 White 5mm Flanged LED (PSI/Voice LED)*
    1 5v power supply
    2 1K ohm Resistors*
    2 220 ohm Resistors*
    1 56 ohm Resistor*
    1 Switch - local or remote

    *There are actually MORE LEDs required if using BB-8 Club spec (as of 4 Jan 16 T&M beta v033) pg 42
     If using additional LEDs, more resistors and/or different value resistors will be required.

     May want to consider adding a LED to the HP as well which will also require changes to the resistor 
     & wiring

    Example:  2 Front (Top & Bottom) Logic Lenses have 5 round 3mm LEDs each
              1 Side Logic Lens has 4 round 3mm LEDs

    r0n_dL BASIC WIRING for testing

 Switch | 5v Power | Pro Micro | MP3-FLASH-16P | Speaker | LEDs
        |   POS    |  VCC      |     VCC       |         | + (3mm Red)- +(3mm Blue) 56 R
    1   |   NEG    |  GND      |     GND       |         | GND all LEDs
        |          |  3        |     BUSY      |         |    
        |          |  9        |               |         | + (3mm Wht 220 R)   
        |          |  8*(1K R) |      TX       |         |     
        |          |  4 (1K R) |      RX       |         |     
        |          |  6        |               |         | + (5mm Wht 220 R) 
        |          |  A2       |     SPK1      |   +     |      
        |          |           |     SPK2      |   -     |     
    2   |          |  A3       |               |         |
    
 NOTE: 3mm Red LED and 3mm Blue LED wired in series with 56 ohm Resistor
 * MP3 TX to Pin 8 not required.  Do not use pin 8 for anything else
 
RESERVED DO NOT USE
  8 (Analog) - RX (SoftwareSerial)
 
FREE PINS on Arduino
  TX1
  RX0
  2
  5
  7 
  10
  16
  14
  15
  A0
  A1
  
*/

// SET AUDIO OPTIONS...

// NOTE:  This sketch currently supports 3 different sound modules (MP3-FLASH-16P, BY8001-16P, BY8301-16P)
  
#define AUDIO1  1   // 1=MP3-FLASH-16P
                    // 2=BY8001-16P or BY8301-16P

                            
// INCLUDE LIBS AND DECLARE VARIABLES...

#include "SoftwareSerial.h"
SoftwareSerial MP3Serial(8, 4); // SoftwareSerial port assignments (RX, TX) to communicate with sound module

#if (AUDIO1==2)
  //settings for BY8001-16P or BY8301-16P module... 

  #include "BY8x0116Pv2.h"
  BY8x0116Pv2 myPlayer(MP3Serial); // Use SoftwareSerial as the serial port
  
#else 
  //settings for MP3-FLASH-16P...
  
  #include "MP3Flash16Pv2.h"
  MP3Flash16Pv2 myPlayer(MP3Serial); // Use SoftwareSerial as the serial port
  
  
#endif


// ARDUINO PIN ASSIGNMENTS...
// Default Pin Assignments - BASIC WIRING in NOTES

#define PIN_sound_BUSY    3  //Connect to BUSY pin on sound module
#define PIN_pulse_LED     9  //Logic LED - 3mm White & 220 Ohm Resistor
#define PIN_voice_LED     6  //Voice LED - 5mm White & 220 Ohm Resistor
#define PIN_sound         A2 //Speaker lead for sound & LED sync
#define PIN_trigger       A3 //Connect to Switch


// ADDITIONAL SOUND OPTIONS...

#define number_of_sounds  5

//
//
//

void setup() 
{
    MP3Serial.begin(9600);
    myPlayer.init(PIN_sound_BUSY);      // Init the player with the MP3 BUSY pin connected to Arduino pin defined
    pinMode(PIN_voice_LED, OUTPUT);
    pinMode(PIN_pulse_LED, OUTPUT); 
    pinMode(PIN_sound, INPUT);
    pinMode(PIN_trigger, INPUT_PULLUP); // Changed INPUT to INPUT_PULLUP because of issues working with Pro Micro.  
                                        // Change back to INPUT if required
    randomSeed(analogRead(0));
    
}

int voiceBrightness;
int pulseBrightness;
void loop() {
    // Checking for trigger going LOW
    if(digitalRead(PIN_trigger) == LOW){
        if(!myPlayer.isBusy()){
            myPlayer.playFile(random(1, number_of_sounds+1));
            delay(100);
        }
    }
    
    // Voice light
    if(myPlayer.isBusy()){
        // Sets the brightness of the light based on the loudness of the voice
        voiceBrightness = constrain(map(analogRead(PIN_sound), 700, 1024, 0, 255), 0, 255);
        analogWrite(PIN_voice_LED, voiceBrightness);
    }else{
        // No voice playing, light is off
        analogWrite(PIN_voice_LED, 0);
    }
    
    // Slow pulsing light on the side of BB-8's head
    pulseBrightness = 170+(sin(millis()/400.00)*80);
    analogWrite(PIN_pulse_LED, pulseBrightness);

}
