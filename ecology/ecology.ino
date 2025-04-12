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

#define SERIAL_DEBUG   true

#define TFT_CS        10
#define TFT_RST        9
#define TFT_DC         8

#define BMP_ERROR_LED 7
#define BH1750_ERROR_LED 6

#define LAMP_PIN 5
#define BOMB_PIN 4
#define CSMS_PIN A3

#define POTENCIOMETER_PIN A0
#define POTENCIOMETER_LUMINOSITY_PIN A1

bool BMP_READY = true;
bool BH1750_READY = true;
bool CSMS_READY = true;

bool LAMP_STATUS = false;
bool BOMB_STATUS = false;

Adafruit_BMP280 bmp;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

BH1750 lightMeter;

void setup(void) {
  Serial.begin(9600);
  Serial.println("Initializing...");

  pinMode(BMP_ERROR_LED, OUTPUT);
  pinMode(BH1750_ERROR_LED, OUTPUT);

  pinMode(LAMP_PIN, OUTPUT);
  pinMode(BOMB_PIN, OUTPUT);

  if (!bmp.begin(0x76)) {
    BMP_READY = false;
  }

  if (!lightMeter.begin()) {
    BH1750_READY = false;
  }

  if (analogRead(CSMS_PIN) == 0) {
    CSMS_READY = false;
  }

  checkSensorError();

  tft.initR(INITR_BLACKTAB);
  Serial.println("Initialized");
}

void loop() {
  tft.fillScreen(ST77XX_BLACK);
  drawText("{S}amba Code", ST77XX_MAGENTA, 1, 25, 150);

  checkSensorError();
  checkLuminosity();
  checkMoisture();

  displaySensorData();
  
  if (SERIAL_DEBUG) {
    debug();
  }

  delay(5000);
}

void formatAndDraw(const char* label, float value, const char* unit, uint16_t color, int textSize, int x, int y) {
  char buffer[30];  
  char valueStr[10];  

  if (isnan(value)) {
    strcpy(buffer, label);
    strcat(buffer, ": Err");
  } else {
    dtostrf(value, 6, 2, valueStr);
    strcpy(buffer, label);
    strcat(buffer, ": ");
    strcat(buffer, valueStr);
    strcat(buffer, " ");
    strcat(buffer, unit);
  }
  
  drawText(buffer, color, textSize, x, y);
}

void displaySensorData() {
  if (BMP_READY) {
    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure() / 1013.25F / 100.0F;
    float altitude = bmp.readAltitude(1013.25);

    formatAndDraw("Temperatura", temperature, "C", ST77XX_WHITE, 1, 0, 0);
    formatAndDraw("Pressao", pressure, "atm", ST77XX_WHITE, 1, 0, 10);
    formatAndDraw("Altitude", altitude, "m", ST77XX_WHITE, 1, 0, 20);
  }

  if (BH1750_READY) {
    uint16_t luminosity = lightMeter.readLightLevel();
    char luxBuffer[25];
    snprintf(luxBuffer, sizeof(luxBuffer), "Luminosidade: %ulux", luminosity);
    drawText(luxBuffer, ST77XX_WHITE, 1, 0, 30);
  }

  if (CSMS_READY) {
    int16_t moisture = analogRead(CSMS_PIN);
    char moistureBuffer[20];
    snprintf(moistureBuffer, sizeof(moistureBuffer), "U (01): %u (%u)", moisture, analogRead(POTENCIOMETER_PIN));
    drawText(moistureBuffer, ST77XX_WHITE, 1, 0, 40);
  }

  drawText("Bomba", BOMB_STATUS ? ST77XX_GREEN : ST77XX_RED, 2, 0, 100);
  drawText("Lampada", LAMP_STATUS ? ST77XX_GREEN : ST77XX_RED, 2, 0, 125);
}

void debug() {
  if (BMP_READY) {
    Serial.print("temperatura:");
    Serial.print(bmp.readTemperature());
    Serial.print(",");

    Serial.print("pressao:");
    Serial.print(bmp.readPressure() / 1013.25F / 100.0F);
    Serial.print(",");

    Serial.print("altitude:");
    Serial.print(bmp.readAltitude(1013.25));
    Serial.print(",");
  }

  if (BH1750_READY) {
    Serial.print("luminosidade:");
    Serial.print(lightMeter.readLightLevel());
    Serial.print(",");
    Serial.print("calibragemluminosidade:");
    Serial.print(analogRead(POTENCIOMETER_LUMINOSITY_PIN));
    Serial.print(","),
  }

  if (CSMS_READY) {
    Serial.print("umidade:");
    Serial.print(analogRead(CSMS_PIN));
    Serial.print(",");
    Serial.print("calibragem:");
    Serial.print(analogRead(POTENCIOMETER_PIN));
    Serial.print(",");
  }

  Serial.print("lampada:");
  Serial.print(LAMP_STATUS ? 1 : 0);
  Serial.print(",");

  Serial.print("bomba:");
  Serial.println(BOMB_STATUS ? 1 : 0);
}

void drawText(char *text, uint16_t color, int size, int x, int y) {
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.setTextSize(size);
  tft.print(text);
}

void testSensors() {
  float altitude = bmp.readAltitude(1013.25);
  
  if (isnan(altitude)) {
    Serial.println("Erro de leitura do sensor BMP280");
    BMP_READY = false;
  } else {
    BMP_READY = true;
  }

  uint16_t luminosity = lightMeter.readLightLevel();
  
  if (luminosity >= 65000) {
    Serial.println("Erro de leitura do sensor BH1750");
    BH1750_READY = false;
  } else {
    BH1750_READY = true;
  }

  if (analogRead(CSMS_PIN) > 0) {
    CSMS_READY = true;
  } else {
    CSMS_READY = false;
  }
}

void checkSensorError() {
  testSensors();

  if (BMP_READY == false) {
    digitalWrite(BMP_ERROR_LED, HIGH);
  } else {
    digitalWrite(BMP_ERROR_LED, LOW);
  }

  if (BH1750_READY == false) {
    digitalWrite(BH1750_ERROR_LED, HIGH);
  } else {
    digitalWrite(BH1750_ERROR_LED, LOW);
  }
}

void checkLuminosity() {
  if (BH1750_READY) {
    uint16_t luminosity = lightMeter.readLightLevel();
    
    if (luminosity <= analogRead(POTENCIOMETER_LUMINOSITY_PIN)) {
      LAMP_STATUS = true;
      digitalWrite(LAMP_PIN, LOW);
    } else {
      LAMP_STATUS = false;
      digitalWrite(LAMP_PIN, HIGH);
    }
  } else {
    LAMP_STATUS = false;
    digitalWrite(LAMP_PIN, HIGH);
  }
}

void checkMoisture() {
  if (CSMS_READY) {
    if (analogRead(CSMS_PIN) >= analogRead(POTENCIOMETER_PIN)) {
      BOMB_STATUS = true;
      digitalWrite(BOMB_PIN, LOW);
    } else {
      BOMB_STATUS = false;
      digitalWrite(BOMB_PIN, HIGH);
    }
  } else {
    BOMB_STATUS = false;
    digitalWrite(BOMB_PIN, HIGH);
  }
}
