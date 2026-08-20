#include <Servo.h>

// Definire pini senzori conform montajului tău
const int trigDeschide = 2; // Senzor 1 (Ridică)
const int echoDeschide = 3;
const int trigInchide = 6;  // Senzor 2 (Coboară)
const int echoInchide = 7;

// Pini Actuatori
const int servoPin = 9;
const int ledVerde = 10;
const int ledRosu = 11;
const int buzzerPin = 12;

// Setări unghiuri și distanță
const long DIST_LIMIT = 20; 
const int ANGLE_OPEN = 0;   
const int ANGLE_CLOSED = 90; 

Servo barrier;

void setup() {
  pinMode(trigDeschide, OUTPUT); pinMode(echoDeschide, INPUT);
  pinMode(trigInchide, OUTPUT);  pinMode(echoInchide, INPUT);
  pinMode(ledVerde, OUTPUT);     pinMode(ledRosu, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  barrier.attach(servoPin);
  
  // --- CONFIGURARE INIȚIALĂ: BARIERA JOS ---
  barrier.write(ANGLE_CLOSED); // 
  digitalWrite(ledRosu, HIGH); // Becul roșu aprins la pornire
  digitalWrite(ledVerde, LOW); // Becul verde stins
}

long readDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH, 20000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

void loop() {
  long dInchide = readDistance(trigInchide, echoInchide);
  long dDeschide = readDistance(trigDeschide, echoDeschide);

  // LOGICA 1: Dacă trenul atinge Senzorul 2 (Pinii 6/7) -> COBOARĂ
  if (dInchide < DIST_LIMIT) {
    barrier.write(ANGLE_CLOSED); // 
    digitalWrite(ledRosu, HIGH);
    digitalWrite(ledVerde, LOW);
    tone(buzzerPin, 1000, 200); // Sunet scurt de confirmare
  }

  // LOGICA 2: Dacă trenul atinge Senzorul 1 (Pinii 2/3) -> RIDICĂ
  if (dDeschide < DIST_LIMIT) {
    barrier.write(ANGLE_OPEN);   // 
    digitalWrite(ledRosu, LOW);
    digitalWrite(ledVerde, HIGH);
    noTone(buzzerPin);
  }

  delay(100); 
}