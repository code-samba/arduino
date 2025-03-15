//Display
#include <Adafruit_GFX.h>    
#include <Adafruit_ST7735.h> 
#include <Adafruit_ST7789.h> 
#include <SPI.h>

//BMP
#include <Wire.h>
#include <Adafruit_BMP280.h>

//BH1750
#include <BH1750.h>

#define TFT_CS        10
#define TFT_RST        9
#define TFT_DC         8

#define SERIAL_DEBUG   true          

Adafruit_BMP280 bmp;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

BH1750 lightMeter;

void setup(void) {
  Serial.begin(9600);
  Serial.println("Initializing...");

  if (!bmp.begin(0x76)) {
    Serial.println("Não foi possivel localizar o sensor BMP 280");
  }

  if (!lightMeter.begin()) {
    Serial.println("Não foi possivel localizar o sensor BH 1750");
  }

  tft.initR(INITR_BLACKTAB);
  Serial.println("Initialized");
}

void loop() {
  tft.fillScreen(ST77XX_BLACK);
  drawText("{S}amba Code", ST77XX_MAGENTA, 1, 25, 150);
  
  displaySensorData();
  
  if (SERIAL_DEBUG) {
    debug();
  }

  delay(5000);
}

void formatAndDraw(float value, const char* unit, uint16_t color, int textSize, int x, int y) {
  char buffer[16];  
  if (isnan(value)) {
    snprintf(buffer, sizeof(buffer), "Err%s", unit);
  } else {
    dtostrf(value, 6, 2, buffer);
    strncat(buffer, unit, sizeof(buffer) - strlen(buffer) - 1);
  }
  drawText(buffer, color, textSize, x, y);
}

void displaySensorData() {
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure() / 1013.25F / 100.0F;
  float altitude = bmp.readAltitude(1013.25);
  uint16_t luminosity = lightMeter.readLightLevel();

  formatAndDraw(temperature, "C", ST77XX_WHITE, 2, 0, 0);
  formatAndDraw(pressure, "atm", ST77XX_WHITE, 2, 0, 25);
  formatAndDraw(altitude, "m", ST77XX_WHITE, 2, 0, 50);

  char luxBuffer[12];
  snprintf(luxBuffer, sizeof(luxBuffer), "%ulux", luminosity);
  drawText(luxBuffer, ST77XX_WHITE, 2, 0, 75);
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

  Serial.println("DEBUG BH1750");
  uint16_t lux = lightMeter.readLightLevel();

  Serial.print("Luminosidade: ");
  Serial.print(lux);
  Serial.println(" lux");
  Serial.println();
}

void drawText(char *text, uint16_t color, int size, int x, int y) {
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.setTextSize(size);
  tft.print(text);
}