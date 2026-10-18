#include <LiquidCrystal.h>
#include <Servo.h>

#define TRIG1 2
#define ECHO1 3
#define TRIG2 4
#define ECHO2 5
#define SERVO_PIN 6
#define TRIG3 A0
#define ECHO3 A1

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
Servo bariera;

const int ANGLE_OPEN = 0;                        // ridică bariera
const int ANGLE_CLOSED = 90;                     // coboară bariera
const unsigned long TIMP_BARIERA_DESCHISA = 4000;  // ms
const unsigned long PERIOADA_CITIRE = 200;         // ms între serii de măsurători
const byte CONFIRMARI = 3;                         // citiri identice la rând pentru a accepta o schimbare

bool bariereDeschisa = false;
unsigned long momentDeschidere = 0;
unsigned long ultimaCitire = 0;
byte maskConfirmat = 0;     // bit0 = loc 1, bit1 = loc 2, bit2 = loc 3
byte maskCandidat = 0;
byte nrConfirmari = 0;
byte nrIntrare = 0;

long readDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);   // 0 = fără ecou (timeout)
  long distance = duration * 0.034 / 2;
  return distance;
}

void afiseazaStatus(byte mask) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Locuri ocupate:");
  lcd.setCursor(0, 1);
  if (mask & 1) lcd.print("1 ");
  if (mask & 2) lcd.print("2 ");
  if (mask & 4) lcd.print("3 ");
  if (mask == 0) lcd.print("niciunul");
}

// Ridică bariera și pornește cronometrul; închiderea se face în loop, fără delay(4000)
void deschideBariera(const char *mesaj) {
  lcd.clear();
  lcd.print(mesaj);
  bariera.write(ANGLE_OPEN);
  bariereDeschisa = true;
  momentDeschidere = millis();
}

void setup() {
  lcd.begin(16, 2);
  bariera.attach(SERVO_PIN);
  bariera.write(ANGLE_CLOSED);        // pornim cu bariera jos
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(TRIG3, OUTPUT);
  pinMode(ECHO3, INPUT);
  lcd.print("Sistem parcare");
  delay(2000);
  afiseazaStatus(0);
}

void loop() {
  unsigned long acum = millis();

  // Coboară bariera după timpul stabilit (înlocuiește delay(4000) + write(90))
  if (bariereDeschisa && acum - momentDeschidere >= TIMP_BARIERA_DESCHISA) {
    bariera.write(ANGLE_CLOSED);
    bariereDeschisa = false;
    afiseazaStatus(maskConfirmat);
  }

  if (acum - ultimaCitire < PERIOADA_CITIRE) return;
  ultimaCitire = acum;

  long distIntrare = readDistanceCM(TRIG1, ECHO1);
  long distSenzor2 = readDistanceCM(TRIG2, ECHO2); // Loc 1 & 2
  long distSenzor3 = readDistanceCM(TRIG3, ECHO3); // Loc 2 & 3

  // Ridica bariera la intrare (2 citiri consecutive, ca să nu reacționeze la zgomot)
  if (distIntrare > 0 && distIntrare <= 9) {
    if (nrIntrare < 2) nrIntrare++;
  } else {
    nrIntrare = 0;
  }
  if (nrIntrare >= 2 && !bariereDeschisa) {
    deschideBariera("Masina detectata");
    nrIntrare = 0;
  }

  // Aceleași praguri calibrate manual; pe valori întregi "> 7.1" înseamnă "> 7", "> 11.1" înseamnă "> 11"
  bool loc1 = false, loc2 = false, loc3 = false;

  if (distSenzor2 > 0 && distSenzor2 <= 7) loc1 = true;

  if ((distSenzor2 > 7 && distSenzor2 <= 16) ||
      (distSenzor3 > 11 && distSenzor3 <= 20)) loc2 = true;

  if (distSenzor3 > 0 && distSenzor3 <= 10) loc3 = true;

  byte mask = (loc1 ? 1 : 0) | (loc2 ? 2 : 0) | (loc3 ? 4 : 0);

  // Acceptăm o schimbare doar după CONFIRMARI citiri identice: un singur ecou greșit nu mai deschide bariera
  if (mask == maskCandidat) {
    if (nrConfirmari < CONFIRMARI) nrConfirmari++;
  } else {
    maskCandidat = mask;
    nrConfirmari = 1;
  }

  if (nrConfirmari >= CONFIRMARI && maskCandidat != maskConfirmat) {
    bool pleaca = (maskConfirmat & ~maskCandidat) != 0;   // un loc a trecut din ocupat în liber
    maskConfirmat = maskCandidat;
    if (pleaca) {
      deschideBariera("Masina pleaca...");
    } else if (!bariereDeschisa) {
      afiseazaStatus(maskConfirmat);
    }
  }
}
