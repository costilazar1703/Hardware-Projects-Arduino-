#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 2, 3, 4, 5);

// Pini
const int ledGreen = 9;      // pinul 1 e TX serial și încurcă uploadul
const int ledRed = 6;
const int buzzer = 7;
const int pinFlacara = 8;    // senzor de flacără, HIGH = flacără
const int pinTemp = A1;      // LM35, 10 mV / °C

// Praguri
const float PRAG_TEMP = 50.0f;      // temperatura care confirmă flacăra
const float PRAG_CRITIC = 70.0f;    // peste asta e alarmă și fără flacără
const int NR_ESANTIOANE = 8;        // media citirilor ADC
const unsigned long INTERVAL_CITIRE = 300;   // ms
const byte CONFIRMARI_NECESARE = 3;          // citiri la rând înainte de alarmă

// Stările sistemului
enum Stare { NORMAL, VERIFICARE, PERICOL };

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
    void afisare(const char *mesaj, float temperatura) {
        lcd.setCursor(0, 0);
        lcd.print(mesaj);
        lcd.setCursor(0, 1);
        lcd.print("Temp: ");
        lcd.print(temperatura, 1);
        lcd.print(" C   ");
    }
};

Senzor senzorFlacara(A0, pinFlacara);
Senzor senzorTemp(pinTemp, PRAG_TEMP);
DisplayManager display;

byte confirmari = 0;
unsigned long ultimaCitire = 0;
bool ledAprins = false;

// Verificarea încrucișată: un singur senzor nu e suficient pentru alarmă
Stare evalueaza(bool flacara, float temperatura) {
    bool tempMare = senzorTemp.depasestePrag(temperatura);
    if ((flacara && tempMare) || temperatura > PRAG_CRITIC) return PERICOL;
    if (flacara || tempMare) return VERIFICARE;
    return NORMAL;
}

void setup() {
    pinMode(ledGreen, OUTPUT);
    pinMode(ledRed, OUTPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(pinFlacara, INPUT);

    display.initialize();
}

void loop() {
    unsigned long acum = millis();
    if (acum - ultimaCitire < INTERVAL_CITIRE) return;
    ultimaCitire = acum;

    float temperatura = senzorTemp.citesteTemperatura();
    bool flacara = senzorFlacara.detecteazaFlacara();
    Stare stare = evalueaza(flacara, temperatura);

    // pericolul trebuie să se repete 3 citiri la rând
    if (stare == PERICOL) {
        if (confirmari < CONFIRMARI_NECESARE) confirmari++;
        if (confirmari < CONFIRMARI_NECESARE) stare = VERIFICARE;
    } else {
        confirmari = 0;
    }

    switch (stare) {
        case PERICOL:
            digitalWrite(ledRed, HIGH);
            digitalWrite(ledGreen, LOW);
            digitalWrite(buzzer, HIGH);
            display.afisare("PERICOL! FOC!   ", temperatura);
            break;

        case VERIFICARE:
            // un singur senzor a reacționat: LED roșu clipește, fără sirenă
            ledAprins = !ledAprins;
            digitalWrite(ledRed, ledAprins);
            digitalWrite(ledGreen, LOW);
            digitalWrite(buzzer, LOW);
            display.afisare(flacara ? "Flacara? Verific" : "Temp mare? Verif", temperatura);
            break;

        default:
            digitalWrite(ledRed, LOW);
            digitalWrite(ledGreen, HIGH);
            digitalWrite(buzzer, LOW);
            display.afisare("Sistem normal   ", temperatura);
            break;
    }
}
