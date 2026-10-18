#include <LiquidCrystal.h>

// LCD: rs, en, d4, d5, d6, d7
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Pini (schimbă-i dacă montajul tău e diferit)
const int trigPin = 2;
const int echoPin = 3;
const int ledPin = 5;        // pin PWM: luminozitatea crește când obstacolul e aproape
const int buzzerPin = 6;     // buzzer pasiv, comandat cu tone()

// Zone de distanță (cm)
const int DIST_MAX = 100;    // peste 100 cm senzorul tace
const int DIST_STOP = 10;    // sub 10 cm sunet continuu

const unsigned long PERIOADA_MASURARE = 60;   // HC-SR04 are nevoie de ~60 ms între măsurători
const unsigned long PERIOADA_LCD = 250;       // LCD-ul nu se rescrie la fiecare măsurare
const unsigned long DURATA_BIP = 50;          // cât ține un bip

long distanta = -1;          // -1 = nimic în față / fără ecou
unsigned long ultimaMasurare = 0;
unsigned long ultimulLcd = 0;
unsigned long ultimulBip = 0;

long masoaraDistanta() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long durata = pulseIn(echoPin, HIGH, 30000);   // timeout 30 ms (~5 m)
  if (durata == 0) return -1;
  return durata * 0.034 / 2;   // cm
}

// cu cât e mai aproape, cu atât bipurile sunt mai dese și mai ascuțite
void actualizeazaSunetSiLed(unsigned long acum) {
  if (distanta < 0 || distanta > DIST_MAX) {
    noTone(buzzerPin);
    analogWrite(ledPin, 0);
    return;
  }

  long d = constrain(distanta, DIST_STOP, DIST_MAX);
  analogWrite(ledPin, map(d, DIST_STOP, DIST_MAX, 255, 10));
  int frecventa = map(d, DIST_STOP, DIST_MAX, 2500, 1000);

  if (distanta <= DIST_STOP) {
    tone(buzzerPin, frecventa);          // foarte aproape: sunet continuu
    return;
  }

  unsigned long pauza = map(d, DIST_STOP, DIST_MAX, 80, 800);
  if (acum - ultimulBip >= DURATA_BIP + pauza) {
    ultimulBip = acum;
    tone(buzzerPin, frecventa, DURATA_BIP);
  }
}

void afiseazaLcd() {
  lcd.setCursor(0, 0);
  if (distanta < 0 || distanta > DIST_MAX) {
    lcd.print("Distanta: --    ");
  } else {
    lcd.print("Distanta: ");
    lcd.print(distanta);
    lcd.print(" cm   ");
  }

  lcd.setCursor(0, 1);
  if (distanta < 0 || distanta > DIST_MAX) lcd.print("Liber           ");
  else if (distanta <= DIST_STOP)          lcd.print("STOP!           ");
  else if (distanta <= 30)                 lcd.print("Foarte aproape  ");
  else                                     lcd.print("Atentie         ");
}

void setup() {
  lcd.begin(16, 2);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  lcd.print("Senzor parcare");
  delay(1500);
  lcd.clear();
}

void loop() {
  unsigned long acum = millis();

  if (acum - ultimaMasurare >= PERIOADA_MASURARE) {
    ultimaMasurare = acum;
    distanta = masoaraDistanta();
  }

  actualizeazaSunetSiLed(acum);

  if (acum - ultimulLcd >= PERIOADA_LCD) {
    ultimulLcd = acum;
    afiseazaLcd();
  }
}
