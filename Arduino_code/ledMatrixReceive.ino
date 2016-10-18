#define MATRICES 4

#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_8x8matrix matrix[MATRICES];

void setup() {
  //Start serial for output
  Serial.begin(9600);
    
  //Initialize the four matrixes
  for (int i=0;i<=3;i++)
  {
    //Passing the addresses and setting brightness
    matrix[i].begin(0x73-i);
    matrix[i].setBrightness(0);
  }
}


void loop() {
  uint8_t i;
  delay(30);
  byte package[32];

  //Main loop 
  if(Serial.available())
  { 
    for(int i=0;i<32;i++){
      package[i]=0;
    }
    
    //Waits for last byte of package
    while (Serial.read()!=0xAB){}

    //Wait for the next 34 bytes
    while (Serial.available()<35){}

    //Check if array is aligned
    if(Serial.read()==0xCD && Serial.read()==0xEF){
      for(int i=0;i<32;i++){
        package[i]=Serial.read();
      }
    }

    // Clean matrix
    for(i=0; i<MATRICES; i++) {
      matrix[i].clear();  
      matrix[i].writeDisplay();
    }

    // Draw message in each matrix buffer
    for (int i=0;i<32;i++){
      for(int j=0;j<8;j++){
        if((package[i] & (1<< j))==(1<< j)){
          if(i<8){
            matrix[0].drawPixel(i, j, LED_ON);
          }
          else if(i<16){
            matrix[1].drawPixel(i-8, j, LED_ON);
          }
          else if(i<24){
            matrix[2].drawPixel(i-16, j, LED_ON);
          }
          else if(i<32){
            matrix[3].drawPixel(i-24, j, LED_ON);
          }
        }
      }  
    }

    // Write matrix buffer to the display
    for(i=0; i<MATRICES; i++) {
      matrix[i].writeDisplay();
    }
  }
}
