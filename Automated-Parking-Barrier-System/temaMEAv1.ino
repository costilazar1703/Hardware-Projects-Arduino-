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

// mașină de stări, fără delay()
enum Stare { LIBER, DESCHIDE, TRECE, INCHIDE, ASTEAPTA_LOC, AFISEAZA };
Stare stare = LIBER;
bool esteIntrare = true;
int pos = 0;                          // 0 = bariera închisă, 90 = deschisă
unsigned long tStare = 0;             // momentul intrării în starea curentă
unsigned long tPas = 0;               // ultimul pas al servo-ului
unsigned long ultimaCitire = 0;
byte nrIntrare = 0;
byte nrIesire = 0;

const unsigned long PAS_SERVO_MS = 10;
const unsigned long TIMP_TRECERE = 3000;
const unsigned long PERIOADA_CITIRE = 300;

long masurareDistanta(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long durata = pulseIn(echo, HIGH, 30000);   // timeout: fără ecou returnează 0
  return durata * 0.034 / 2;  // în cm
}

void treciLa(Stare nou) {
  stare = nou;
  tStare = millis();
}

void afiseazaLocuri(long distParcare) {
  lcd.clear();
  if (esteIntrare) {
    if (distParcare > 24) {
      lcd.print("Locuri libere");
    } else if (distParcare > 15) {
      lcd.print("Loc ocupat: 1");
    } else if (distParcare > 6) {
      lcd.print("Loc ocupat: 2");
    } else {
      lcd.print("Loc ocupat: 3");
    }
  } else {
    if (distParcare > 24) {
      lcd.print("Toate libere");
    } else if (distParcare > 15) {
      lcd.print("Locuri 2-3 libere");
    } else if (distParcare > 6) {
      lcd.print("Loc 3 liber");
    } else {
      lcd.print("Toate ocupate");
    }
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
  unsigned long acum = millis();

  switch (stare) {
    case LIBER: {
      if (acum - ultimaCitire < PERIOADA_CITIRE) break;
      ultimaCitire = acum;

      long distIntrare = masurareDistanta(trigIntrare, echoIntrare);
      long distIesire = masurareDistanta(trigIesire, echoIesire);

      // 0 = fără ecou; cer 2 citiri la rând
      nrIntrare = (distIntrare > 0 && distIntrare < 10) ? nrIntrare + 1 : 0;
      nrIesire = (distIesire > 0 && distIesire < 10) ? nrIesire + 1 : 0;

      // === Mașină la intrare === (are prioritate)
      if (nrIntrare >= 2) {
        tone(buzzer, 1000, 200);
        lcd.clear();
        lcd.print("Masina intra...");
        esteIntrare = true;
        nrIntrare = 0;
        treciLa(DESCHIDE);
      }
      // === Mașină la ieșire ===
      else if (nrIesire >= 2) {
        tone(buzzer, 800, 200);
        lcd.clear();
        lcd.print("Masina iese...");
        esteIntrare = false;
        nrIesire = 0;
        treciLa(DESCHIDE);
      }
      break;
    }

    case DESCHIDE:                        // 0 -> 90 grade, un grad la 10 ms
      if (acum - tPas >= PAS_SERVO_MS) {
        tPas = acum;
        pos++;
        bariera.write(pos);
        if (pos >= 90) treciLa(TRECE);
      }
      break;

    case TRECE:                           // timp să treacă mașina
      if (acum - tStare >= TIMP_TRECERE) treciLa(INCHIDE);
      break;

    case INCHIDE:                         // 90 -> 0 grade
      if (acum - tPas >= PAS_SERVO_MS) {
        tPas = acum;
        pos--;
        bariera.write(pos);
        if (pos <= 0) treciLa(ASTEAPTA_LOC);
      }
      break;

    case ASTEAPTA_LOC: {                  // timp să ajungă la loc / să se elibereze
      unsigned long asteptare = esteIntrare ? 2000 : 1000;
      if (acum - tStare >= asteptare) {
        afiseazaLocuri(masurareDistanta(trigParcare, echoParcare));
        treciLa(AFISEAZA);
      }
      break;
    }

    case AFISEAZA:
      if (acum - tStare >= 3000) {
        lcd.clear();
        treciLa(LIBER);
      }
      break;
  }
}
