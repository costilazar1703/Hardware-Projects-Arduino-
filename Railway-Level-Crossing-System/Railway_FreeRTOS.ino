#include <Arduino_FreeRTOS.h>   // biblioteca FreeRTOS pentru Arduino
#include <queue.h>
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

// ms -> tick-uri FreeRTOS (un tick are ~15 ms pe Uno)
#define MS(x) ((x) / portTICK_PERIOD_MS)

// evenimente trimise de la detecție către barieră
const uint8_t EVT_LOWER = 1;   // senzorul 2 -> coboară bariera
const uint8_t EVT_RAISE = 2;   // senzorul 1 -> ridică bariera

Servo barrier;
QueueHandle_t eventQueue;             // coada Detect -> Barrier
volatile bool trainPresent = false;   // true cât timp trenul e în zonă; citit de Alarm
int barrierAngle = ANGLE_CLOSED;      // unghiul curent

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

// Mută bariera câte 1 grad, fără delay blocant. Ridicarea se oprește dacă vine
// un eveniment nou, coborârea nu se oprește niciodată.
bool moveBarrier(int target) {
  while (barrierAngle != target) {
    if (target == ANGLE_OPEN && uxQueueMessagesWaiting(eventQueue) > 0) {
      return false;
    }
    barrierAngle += (target > barrierAngle) ? 1 : -1;
    barrier.write(barrierAngle);
    vTaskDelay(MS(15));
  }
  return true;
}

// TASK 1 (prioritate 2): citește senzorii și trimite evenimente
void TaskTrainDetect(void *pvParameters) {
  uint8_t last = 0;                      // ultima cerere văzută: 0 = nimic, 1 = coboară, 2 = ridică
  for (;;) {
    long dInchide = readDistance(trigInchide, echoInchide);
    long dDeschide = readDistance(trigDeschide, echoDeschide);

    // Dacă ambii senzori văd ceva, coborârea are prioritate (siguranță)
    uint8_t wanted = (dInchide < DIST_LIMIT) ? 1 : ((dDeschide < DIST_LIMIT) ? 2 : 0);

    // Trimitem evenimentul doar când cererea se schimbă, nu la fiecare citire
    if (wanted != 0 && wanted != last) {
      uint8_t evt = (wanted == 1) ? EVT_LOWER : EVT_RAISE;
      xQueueSend(eventQueue, &evt, 0);
    }
    last = wanted;

    vTaskDelay(MS(60));                  // cedează procesorul celorlalte taskuri
  }
}

// TASK 2 (prioritate 3, cea mai mare): comandă bariera și LED-urile
void TaskBarrier(void *pvParameters) {
  uint8_t evt;
  for (;;) {
    // Taskul doarme aici până vine un eveniment (nu consumă procesor)
    if (xQueueReceive(eventQueue, &evt, portMAX_DELAY) == pdPASS) {
      if (evt == EVT_LOWER) {
        trainPresent = true;
        digitalWrite(ledRosu, HIGH);
        digitalWrite(ledVerde, LOW);
        moveBarrier(ANGLE_CLOSED);
      } else {
        // LED-ul roșu rămâne aprins până bariera e complet ridicată
        if (moveBarrier(ANGLE_OPEN)) {
          digitalWrite(ledRosu, LOW);
          digitalWrite(ledVerde, HIGH);
          trainPresent = false;
        }
      }
    }
  }
}

// TASK 3 (prioritate 1, cea mai mică): semnal sonor cât timp trenul e prezent
void TaskAlarm(void *pvParameters) {
  for (;;) {
    if (trainPresent) {
      tone(buzzerPin, 1000, 200);        // bip scurt
      vTaskDelay(MS(400));
    } else {
      noTone(buzzerPin);
      vTaskDelay(MS(100));
    }
  }
}

void setup() {
  pinMode(trigDeschide, OUTPUT); pinMode(echoDeschide, INPUT);
  pinMode(trigInchide, OUTPUT);  pinMode(echoInchide, INPUT);
  pinMode(ledVerde, OUTPUT);     pinMode(ledRosu, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  barrier.attach(servoPin);

  // --- CONFIGURARE INIȚIALĂ: BARIERA JOS ---
  barrier.write(ANGLE_CLOSED);
  digitalWrite(ledRosu, HIGH); // Becul roșu aprins la pornire
  digitalWrite(ledVerde, LOW); // Becul verde stins

  eventQueue = xQueueCreate(4, sizeof(uint8_t));   // coadă de 4 evenimente

  // xTaskCreate(funcție, nume, stivă în octeți, parametru, prioritate, handle)
  xTaskCreate(TaskBarrier,     "Barrier", 192, NULL, 3, NULL);
  xTaskCreate(TaskTrainDetect, "Detect",  192, NULL, 2, NULL);
  xTaskCreate(TaskAlarm,       "Alarm",   128, NULL, 1, NULL);
}

void loop() {
  // Gol: planificatorul FreeRTOS rulează taskurile
}
