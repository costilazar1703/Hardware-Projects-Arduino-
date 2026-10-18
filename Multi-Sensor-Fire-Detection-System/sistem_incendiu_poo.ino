#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 2, 3, 4, 5);

// Pini actuatori. LED verde pe 9 (pinul 1 e TX serial și încurcă uploadul)
const int ledGreen = 9;
const int ledRed = 6;
const int buzzer = 7;
const int pinFlacara = 8;    // senzor de flacără digital (HIGH = flacără detectată)
const int pinTemp = A1;      // senzor de temperatură analogic (LM35: 10 mV / °C)

const float PRAG_TEMPERATURA = 100.0f;           // °C
const int NR_ESANTIOANE = 8;                     // media citirilor ADC reduce zgomotul
const unsigned long INTERVAL_CITIRE = 300;       // ms între măsurători (fără delay)
const byte CONFIRMARI_NECESARE = 3;              // citiri consecutive înainte de alarmă

class Senzor {
private:
    int pinAnalog;
    int pinDigital;
    float pragTemp;
public:
    Senzor(int analogPin, float tempThreshold) {
        pinAnalog = analogPin;
        pinDigital = -1;
        pragTemp = tempThreshold;
    }

    Senzor(int analogPin, int digitalPin) {
        pinAnalog = analogPin;
        pinDigital = digitalPin;
        pragTemp = 0;
    }

    float citesteTemperatura() {
        long suma = 0;
        for (int i = 0; i < NR_ESANTIOANE; i++) {
            suma += analogRead(pinAnalog);
        }
        float valoare = suma / (float)NR_ESANTIOANE;
        return (valoare * 5.0 / 1023.0) * 100.0;
    }

    // folosește pragul din constructor
    bool depasestePrag(float temperatura) {
        return temperatura > pragTemp;
    }

    bool detecteazaFlacara() {
        if (pinDigital != -1) {
            return digitalRead(pinDigital) == HIGH;
        }
        return false;
    }
};

class DisplayManager {
public:
    void initialize() {
        lcd.begin(16, 2);
        lcd.print("Sistem activ!");
        delay(2000);
        lcd.clear();
    }

    // rescriu liniile întregi, fără lcd.clear(), ca să nu clipească
    void afisareNormal(float temperatura) {
        lcd.setCursor(0, 0);
        lcd.print("Sistem normal   ");
        lcd.setCursor(0, 1);
        lcd.print("Temp: ");
        lcd.print(temperatura, 1);
        lcd.print(" C   ");
    }

    void afisarePericol(float temperatura) {
        lcd.setCursor(0, 0);
        lcd.print("PERICOL! FOC!   ");
        lcd.setCursor(0, 1);
        lcd.print("Temp: ");
        lcd.print(temperatura, 1);
        lcd.print(" C   ");
    }
};

Senzor senzorFlacara(A0, pinFlacara);
Senzor senzorTemp(pinTemp, PRAG_TEMPERATURA);
DisplayManager display;

byte confirmari = 0;
unsigned long ultimaCitire = 0;

void setup() {
    pinMode(ledGreen, OUTPUT);
    pinMode(ledRed, OUTPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(pinFlacara, INPUT);

    display.initialize();
}

void loop() {
    // măsor la 300 ms, fără delay
    unsigned long acum = millis();
    if (acum - ultimaCitire < INTERVAL_CITIRE) return;
    ultimaCitire = acum;

    float temperatura = senzorTemp.citesteTemperatura();
    bool flacaraDetectata = senzorFlacara.detecteazaFlacara();

    // OR: oricare senzor poate da alarma (siguranță). Pentru mai puține alarme
    // false, trebuie 3 citiri la rând.
    bool pericol = flacaraDetectata || senzorTemp.depasestePrag(temperatura);
    if (pericol) {
        if (confirmari < CONFIRMARI_NECESARE) confirmari++;
    } else {
        confirmari = 0;
    }
    bool alarma = (confirmari >= CONFIRMARI_NECESARE);

    if (alarma) {
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledGreen, LOW);
        digitalWrite(buzzer, HIGH);
        display.afisarePericol(temperatura);
    } else {
        digitalWrite(ledRed, LOW);
        digitalWrite(ledGreen, HIGH);
        digitalWrite(buzzer, LOW);
        display.afisareNormal(temperatura);
    }
}
