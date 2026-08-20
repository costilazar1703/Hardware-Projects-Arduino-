#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 2, 3, 4, 5);

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
    }

    float citesteTemperatura() {
        int valoare = analogRead(pinAnalog);
        return (valoare * 5.0 / 1023.0) * 100.0; 
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
    void afisareNormal(float temperatura) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Sistem normal  ");
        lcd.setCursor(0, 1);
        lcd.print("Temp: ");
        lcd.print(temperatura);
        lcd.print(" C");
    }

    void afisarePericol(float temperatura) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("PERICOL! FOC!");
        lcd.setCursor(0, 1);
        lcd.print("Temp: ");
        lcd.print(temperatura);
        lcd.print(" C");
    }
};

Senzor senzorFum(A0, 8);
Senzor senzorTemp(A1, 100.0f); 
DisplayManager display;

int ledGreen = 1;
int ledRed = 6;
int buzzer = 7;

void setup() {
    pinMode(ledGreen, OUTPUT);
    pinMode(ledRed, OUTPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(8, INPUT);

    display.initialize();
}

void loop() {
    float temperatura = senzorTemp.citesteTemperatura();
    bool fumDetectat = senzorFum.detecteazaFum();

    if (fumDetectat || temperatura > 100.0) {
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

    delay(500); 
}
