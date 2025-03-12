#include <Adafruit_GFX.h>    
#include <Adafruit_ST7735.h> 
#include <Adafruit_ST7789.h> 
#include <SPI.h>

#define TFT_CS        10
#define TFT_RST        9
#define TFT_DC         8

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup(void) {
  Serial.begin(9600);
  Serial.println("Initializing...");

  tft.initR(INITR_BLACKTAB);

  Serial.println("Initialized");

  tft.fillScreen(ST77XX_BLACK);

  testdrawtext("Desenvolvido por", ST77XX_WHITE, 0, 15, 45);
  testdrawtext("Samba Code", ST77XX_MAGENTA, 2, 5, 60);
  testdrawtext("{S}", ST77XX_MAGENTA, 5, 22, 100);
  delay(5000);

  Serial.println("Done!");
  delay(1000);
}

void loop() {
  //
}

void testdrawtext(char *text, uint16_t color, int size, int x, int y) {
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.setTextSize(size);
  tft.print(text);
}