#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

#define SDA_PIN 5
#define SCL_PIN 6
/*
  U8g2lib Example Overview:
    Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
    Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
    U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
    
*/
U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);   // EastRising 0.42" OLED
int i,j;
const int ledPin = 8; 


void setup(void) {
  Serial.begin(115200);//aspettare almeno 5 secondi e mettere 
  delay(5000);  //build_flags= -D ARDUINO_USB_CDC_ON_BOOT=1 -D ARDUINO_USB_MODE=1
  pinMode(ledPin, OUTPUT);  // digitalWrite(ledPin, HIGH);     
  Wire.begin(SDA_PIN, SCL_PIN);
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font  
  i=0;
  Serial.println("-----------");
}

void loop(void) {
// String fixed = F("COPY from FLASH");
// u8g2.firstPage();
// do {
//     u8g2.drawStr(0, 10, "Hello World!");  // write something to the internal memory
//     u8g2.drawStr(10, 20, name.c_str());  // write something to the internal memory
//     u8g2.drawStr(5, 30, fixed.c_str());  // write String from SRAM
//     u8g2.drawStr(0, 40, F("ORIGINAL IN FLASH").c_str());  // does not work
//     } while (u8g2.nextPage());
  u8g2.clearBuffer();
  u8g2.setCursor(0, i);
  u8g2.print("Hello World!");
  u8g2.sendBuffer();
  delay(200);                     
  i++;
  if (i==50)
    {i=0;
    Serial.println("-----------");
    for(j=0;j<5;j++){
      digitalWrite(ledPin, LOW); 
      u8g2.clearBuffer();
      u8g2.setCursor(0, 10*j);
      u8g2.print("bye bye world!!!!!");
      u8g2.sendBuffer();
      delay(250);
      u8g2.clearBuffer();
      u8g2.sendBuffer();
      digitalWrite(ledPin, HIGH);   
      delay(250);
    }
  }
}