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

#define BMP_ERROR_LED 7
#define BH1750_ERROR_LED 6

#define LAMP_PIN 5

int BMP_READY = true;
int BH1750_READY = true;

bool LAMP_STATUS = false;

Adafruit_BMP280 bmp;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

BH1750 lightMeter;

void setup(void) {
  Serial.begin(9600);
  Serial.println("Initializing...");

  pinMode(BMP_ERROR_LED, OUTPUT);
  pinMode(BH1750_ERROR_LED, OUTPUT);

  pinMode(LAMP_PIN, OUTPUT);

  if (!bmp.begin(0x76)) {
    Serial.println("Não foi possivel localizar o sensor BMP 280");
    BMP_READY = false;
  }

  if (!lightMeter.begin()) {
    Serial.println("Não foi possivel localizar o sensor BH 1750");
    BH1750_READY = false;
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
  if (BMP_READY) {
    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure() / 1013.25F / 100.0F;
    float altitude = bmp.readAltitude(1013.25);
    formatAndDraw(temperature, "C", ST77XX_WHITE, 2, 0, 0);
    formatAndDraw(pressure, "atm", ST77XX_WHITE, 2, 0, 25);
    formatAndDraw(altitude, "m", ST77XX_WHITE, 2, 0, 50);
  }

  if (BH1750_READY) {
    uint16_t luminosity = lightMeter.readLightLevel();
    char luxBuffer[12];
    snprintf(luxBuffer, sizeof(luxBuffer), "%ulux", luminosity);
    drawText(luxBuffer, ST77XX_WHITE, 2, 0, 75);

    drawText("Lampada", LAMP_STATUS ? ST77XX_GREEN : ST77XX_RED, 2, 0, 100);
  }
}

void debug() {
  if (BMP_READY) {
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
  } else {
    Serial.println("Erro: BMP280 não detectado");
  }

  if (BH1750_READY) {
    Serial.println("DEBUG BH1750");
    uint16_t lux = lightMeter.readLightLevel();
    Serial.print("Luminosidade: ");
    Serial.print(lux);
    Serial.println(" lux");
    Serial.println();


    Serial.println("DEBUG LAMP STATUS");
    Serial.print("Status da lampada: ");
    Serial.println(LAMP_STATUS ? "Ligado" : "Desligado");
    Serial.println();
  } else {
    Serial.println("Erro: BH1750 não detectado");
  }
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
    
    if (luminosity <= 5) {
      LAMP_STATUS = true;
      digitalWrite(LAMP_PIN, HIGH);
    } else {
      LAMP_STATUS = false;
      digitalWrite(LAMP_PIN, LOW);
    }
  } else {
    LAMP_STATUS = false;
    digitalWrite(LAMP_PIN, LOW);
  }
}