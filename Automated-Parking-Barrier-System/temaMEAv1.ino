#include <LiquidCrystal.h>
#include <Servo.h>

// LCD: rs, en, d4, d5, d6, d7
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Pines pentru senzorii HC-SR04
const int trigIntrare = 2;
const int echoIntrare = 3;
const int trigParcare = 4;
const int echoParcare = 5;
const int trigIesire = A0;
const int echoIesire = A1;

const int buzzer = 6;
const int servoPin = 13;

Servo bariera;

long masurareDistanta(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long durata = pulseIn(echo, HIGH);
  return durata * 0.034 / 2;  // în cm
}

void deschideBariera() {
  for (int pos = 0; pos <= 90; pos++) {
    bariera.write(pos);
    delay(10);
  }
}

void inchideBariera() {
  for (int pos = 90; pos >= 0; pos--) {
    bariera.write(pos);
    delay(10);
  }
}

void setup() {
  lcd.begin(16, 2);

  pinMode(trigIntrare, OUTPUT);
  pinMode(echoIntrare, INPUT);
  pinMode(trigParcare, OUTPUT);
  pinMode(echoParcare, INPUT);
  pinMode(trigIesire, OUTPUT);
  pinMode(echoIesire, INPUT);
  pinMode(buzzer, OUTPUT);

  bariera.attach(servoPin);
  bariera.write(0);  // Bariera închisă inițial

  lcd.print("Sistem parcare");
  delay(2000);
  lcd.clear();
}

void loop() {
  long distIntrare = masurareDistanta(trigIntrare, echoIntrare);
  long distIesire = masurareDistanta(trigIesire, echoIesire);

  // === Mașină la intrare ===
  if (distIntrare < 10) {
    tone(buzzer, 1000, 200);
    lcd.clear();
    lcd.print("Masina intra...");
    
    deschideBariera();
    delay(3000);  // timp să treacă mașina
    inchideBariera();

    delay(2000);  // timp să ajungă la loc
    long distParcare = masurareDistanta(trigParcare, echoParcare);
    
    lcd.clear();
    if (distParcare > 24) {
      lcd.print("Locuri libere");
    } else if (distParcare > 15) {
      lcd.print("Loc ocupat: 1");
    } else if (distParcare > 6) {
      lcd.print("Loc ocupat: 2");
    } else {
      lcd.print("Loc ocupat: 3");
    }

    delay(3000);
    lcd.clear();
  }

  // === Mașină la ieșire ===
  else if (distIesire < 10) {
    tone(buzzer, 800, 200);
    lcd.clear();
    lcd.print("Masina iese...");

    deschideBariera();
    delay(3000);
    inchideBariera();

    delay(1000);  // Așteaptă să se elibereze
    long distParcare = masurareDistanta(trigParcare, echoParcare);

    lcd.clear();
    if (distParcare > 24) {
      lcd.print("Toate libere");
    } else if (distParcare > 15) {
      lcd.print("Locuri 2-3 libere");
    } else if (distParcare > 6) {
      lcd.print("Loc 3 liber");
    } else {
      lcd.print("Toate ocupate");
    }

    delay(3000);
    lcd.clear();
  }

  delay(300);
}
