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

bool lastLoc1 = false, lastLoc2 = false, lastLoc3 = false;

long readDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  long distance = duration * 0.034 / 2;
  return distance;
}

void setup() {
  lcd.begin(16, 2);
  bariera.attach(SERVO_PIN);
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(TRIG3, OUTPUT);
  pinMode(ECHO3, INPUT);
  lcd.print("Sistem parcare");
  delay(2000);
  lcd.clear();
}

void loop() {
  long distIntrare = readDistanceCM(TRIG1, ECHO1);
  long distSenzor2 = readDistanceCM(TRIG2, ECHO2); // Loc 1 & 2
  long distSenzor3 = readDistanceCM(TRIG3, ECHO3); // Loc 2 & 3

  // Ridica bariera la intrare
  if (distIntrare > 0 && distIntrare <= 9) {
    lcd.clear();
    lcd.print("Masina detectata");
    bariera.write(0);
    delay(4000);
    bariera.write(90);
  }

  bool loc1 = false, loc2 = false, loc3 = false;

  if (distSenzor2 > 0 && distSenzor2 <= 7) loc1 = true;

  if ((distSenzor2 > 7.1 && distSenzor2 <= 16) || 
      (distSenzor3 > 11.1 && distSenzor3 <= 20)) loc2 = true;

  if (distSenzor3 > 0 && distSenzor3 <= 10) loc3 = true;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Locuri ocupate:");

  lcd.setCursor(0, 1);
  if (loc1) lcd.print("1 ");
  if (loc2) lcd.print("2 ");
  if (loc3) lcd.print("3 ");
  if (!loc1 && !loc2 && !loc3) lcd.print("niciunul");

  delay(1000);

  if ((lastLoc1 && !loc1) || (lastLoc2 && !loc2) || (lastLoc3 && !loc3)) {
    lcd.clear();
    lcd.print("Masina pleaca...");
    bariera.write(0); // ridica bariera
    delay(4000);
    bariera.write(90); // coboara bariera
  }

  lastLoc1 = loc1;
  lastLoc2 = loc2;
  lastLoc3 = loc3;

  delay(500);
}
