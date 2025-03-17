#include <NewPing.h>
#include <Adafruit_GFX.h>    
#include <Adafruit_ST7735.h> 
#include <SPI.h>

#define TRIGGER_PIN 6
#define ECHO_PIN 7

#define TFT_CS        10
#define TFT_RST        9
#define TFT_DC         8

#define RADIUS 5
#define MAX_DISTANCE 150
#define MAX_HEIGHT 100
#define MAX_VOLUME 1000

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing...");

  tft.initR(INITR_BLACKTAB);
  Serial.println("Initialized");
}

void loop() {
  tft.fillScreen(ST77XX_BLACK);
  long distance = sonar.ping_cm();
  
  if (distance > 0) {
    float volume = calculateVolume(distance);
    float waterLevel = calculateWaterLevel(volume);

    drawText("Volume (L):", ST77XX_WHITE, 1, 10, 10);
    drawFixedBox();
    if (volume > 0) {
      drawWaterLevel(waterLevel);
    }

    char volumeStr[10];
    dtostrf(volume / 1000, 6, 2, volumeStr);
    drawText(volumeStr, ST77XX_WHITE, 1, 70, 10);
  } else {
    drawText("Erro no sensor", ST77XX_RED, 1, 10, 10);
  }

  delay(1000);
}

void drawFixedBox() {
  int sideLength = MAX_HEIGHT;

  tft.drawRect(12, 50, sideLength, sideLength, ST77XX_WHITE);
}

void drawWaterLevel(float level) {
  tft.fillRect(12, 50 + MAX_HEIGHT - level, MAX_HEIGHT, level, ST77XX_BLUE); 
}

float calculateVolume(long height) {
  float volume = (1000 - (3.14159 * RADIUS * RADIUS * height));
  return volume;
}

float calculateWaterLevel(float volume) {
  float waterLevel = (100 * volume) / MAX_VOLUME;
  return waterLevel;
}

void drawText(char *text, uint16_t color, int size, int x, int y) {
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.setTextSize(size);
  tft.print(text);
}
