#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define servoPinP 11
#define servoPinS 10
#define servoPinT 8
#define servoPinQ 9

#define col 16
#define lin  2
#define ende  0x27

LiquidCrystal_I2C lcd(ende, col, lin);

Servo p, s, t, q;
int pos;

int port = 1;
bool ready = false;

//172800
const unsigned long tempoDeEspera = 172800;

void setup() {
  pinMode(7, INPUT_PULLUP); 
  
  p.attach(servoPinP); p.write(0); delay(1000);
  s.attach(servoPinS); s.write(0); delay(1000);
  t.attach(servoPinT); t.write(0); delay(1000);
  q.attach(servoPinQ); q.write(0); delay(1000);

  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() { 
  if (digitalRead(7) == LOW) {
    ready = true;
  }

  if (ready) {
    esperarComDisplay(tempoDeEspera);

    if (port == 1) {
      incrasePos(180, p);
    }

    if (port == 2) {
      incrasePos(180, s);
    }
  
    if (port == 3) {
      incrasePos(180, t);
    }
  
    if (port == 4) {
      incrasePos(180, q);
    }

    if (port < 4) {
      port += 1;
    } else {
      port = 1;
      ready = false;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Resetando...");

      decrasePos(180, q, 0);
      decrasePos(180, t, 0);
      decrasePos(180, s, 0);
      decrasePos(180, p, 0);
    }
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Aguardando");
    lcd.setCursor(0, 1);
    lcd.print("Confirmacao");
  }
}

void incrasePos(int p, Servo s) {
  for (pos = 0; pos < p; pos++) {
    s.write(pos);
    delay(15);
  }
}

void decrasePos(int p, Servo s, int x) {
  for (pos = p; pos >= x; pos--) {
    s.write(pos);
    delay(15);
  }
}

void esperarComDisplay(unsigned long segundos) {
  for (unsigned long i = segundos; i > 0; i--) {
    int dias = i / 86400;
    int horas = (i % 86400) / 3600;
    int minutos = (i % 3600) / 60;
    int segs = i % 60;

    lcd.setCursor(0, 0);
    lcd.print("Aguardando...   ");

    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(dias); lcd.print("d ");
    if (horas < 10) lcd.print("0");
    lcd.print(horas); lcd.print("h ");
    if (minutos < 10) lcd.print("0");
    lcd.print(minutos); lcd.print("m ");
    if (segs < 10) lcd.print("0");
    lcd.print(segs); lcd.print("s ");

    delay(1000);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Abrindo...");
}

void waitForButtonPress() {
  while (digitalRead(7) == HIGH) {
    delay(10);
  }

  while (digitalRead(7) == LOW) {
    delay(10);
  }
}
