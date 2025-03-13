#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Serial.println("\nEscaneando dispositivos I2C...");

  Wire.begin();
}

void loop() {
  byte error, address;
  int nDevices = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Dispositivo encontrado no endereço 0x");
      Serial.println(address, HEX);
      nDevices++;
    }
  }

  if (nDevices == 0)
    Serial.println("Nenhum dispositivo I2C encontrado.");
  else
    Serial.println("Escaneamento concluído.");

  delay(5000);
}
