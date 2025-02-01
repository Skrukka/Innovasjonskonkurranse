// https://www.instructables.com/Value-Your-Project-Use-Graphic-Display/
// https://randomnerdtutorials.com/guide-to-1-8-tft-display-with-arduino/



#include <Adafruit_GFX.h> // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
 
//ESP32-WROOM for the 1.8" TFT shield
#define TFT_DC 2 //A0
#define TFT_CS 15 //CS
#define TFT_MOSI 4 //SDA
#define TFT_CLK 16 //SCK
#define TFT_RST 0 
#define TFT_MISO 0
 
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);



void setup() {
  tft.initR(INITR_BLACKTAB); 
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
}

void loop() {
  // Generate a random color
  uint16_t color = tft.color565(random(255), random(255), random(255));
  
  // Set text color
  tft.setTextColor(color);
  
  // Print text
  tft.setCursor(6, 57);
  tft.print("Hello, World!");

  delay(200);
}





