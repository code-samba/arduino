//Display
#include <Adafruit_GFX.h>    
#include <Adafruit_ST7735.h> 
#include <Adafruit_ST7789.h> 
#include <SPI.h>

//BMP
#include <Wire.h>
#include <Adafruit_BMP280.h>

#define TFT_CS        10
#define TFT_RST        9
#define TFT_DC         8

#define SERIAL_DEBUG   true          

Adafruit_BMP280 bmp;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup(void) {
  Serial.begin(9600);
  Serial.println("Initializing...");

  if (!bmp.begin(0x76)) {
    Serial.println("Não foi possivel localizar o sensor BMP 280");
  }

  tft.initR(INITR_BLACKTAB);

  Serial.println("Initialized");

  tft.fillScreen(ST77XX_BLACK);

  drawText("Desenvolvido por", ST77XX_WHITE, 0, 15, 45);
  drawText("Samba Code", ST77XX_MAGENTA, 2, 5, 60);
  drawText("{S}", ST77XX_MAGENTA, 5, 22, 100);
  delay(2500);

  tft.fillScreen(ST77XX_BLACK);
  drawText("{S}amba Code", ST77XX_MAGENTA, 1, 25, 150);

  Serial.println("Done!");
}

void loop() {
  drawSensorsValues();
  
  if (SERIAL_DEBUG) {
    debug();
  }

  delay(5000);
}

void drawSensorsValues() {
  tft.fillRect(0, 0, 128, 140, ST77XX_BLACK);

  float temperature = bmp.readTemperature(); 
  char tempStr[5];
  dtostrf(temperature, 6, 2, tempStr);
  char displayStr[6];
  sprintf(displayStr, "%sC", tempStr);
  drawText(displayStr, ST77XX_WHITE, 2, 0, 0);

  float pressure = bmp.readPressure() / 1013.25F / 100.0F;
  char pressureStr[4];
  dtostrf(pressure, 6, 2, pressureStr);
  sprintf(displayStr, "%satm", pressureStr);
  drawText(displayStr, ST77XX_WHITE, 2, 0, 25);

  float altitude = bmp.readAltitude(1013.25);
  char altitudeStr[12];
  dtostrf(altitude, 6, 2, altitudeStr);
  sprintf(displayStr, "%sm", altitudeStr);
  drawText(displayStr, ST77XX_WHITE, 2, 0, 50);
}

void debug() {
  Serial.println("DEBUG BMP280");

  Serial.print(F("Temperatura: "));
  Serial.print(bmp.readTemperature());
  Serial.println(" C");

  Serial.print(F("Pressão: "));
  Serial.print(bmp.readPressure() / 1013.25F / 100.0F);
  Serial.println(" atm");

  Serial.print(F("Altitude Aprox: "));
  Serial.print(bmp.readAltitude(1013.25));
  Serial.println(" m");

  Serial.println();
}

void drawText(char *text, uint16_t color, int size, int x, int y) {
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.setTextSize(size);
  tft.print(text);
}