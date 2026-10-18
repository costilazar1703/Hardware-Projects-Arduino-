#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 2, 3, 4, 5);

// Pini actuatori. ATENȚIE: LED-ul verde era pe pinul 1 (TX al portului serial,
// intră în conflict cu USB/upload). Mută firul LED-ului verde de pe pinul 1 pe pinul 9.
const int ledGreen = 9;
const int ledRed = 6;
const int buzzer = 7;
const int pinFum = 8;        // senzor digital (fum/flacără, HIGH = detectat)
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

    // Folosește pragul salvat în constructor (înainte era scris direct 100.0 în loop)
    bool depasestePrag(float temperatura) {
        return temperatura > pragTemp;
    }

    bool detecteazaFum() {
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

    // Rescriem liniile complete (cu spații la final) în loc de lcd.clear(): fără clipire
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

Senzor senzorFum(A0, pinFum);
Senzor senzorTemp(pinTemp, PRAG_TEMPERATURA);
DisplayManager display;

byte confirmari = 0;
unsigned long ultimaCitire = 0;

void setup() {
    pinMode(ledGreen, OUTPUT);
    pinMode(ledRed, OUTPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(pinFum, INPUT);

    display.initialize();
}

void loop() {
    // Măsurăm periodic, fără delay(): loop-ul rămâne liber pentru alte sarcini
    unsigned long acum = millis();
    if (acum - ultimaCitire < INTERVAL_CITIRE) return;
    ultimaCitire = acum;

    float temperatura = senzorTemp.citesteTemperatura();
    bool fumDetectat = senzorFum.detecteazaFum();

    // OR din motive de siguranță: oricare senzor poate declanșa alarma.
    // Alarmele false se reduc prin confirmare: trebuie CONFIRMARI_NECESARE citiri la rând.
    bool pericol = fumDetectat || senzorTemp.depasestePrag(temperatura);
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
