// Definire pini
// pinii 0 și 1 sunt RX/TX și încurcă uploadul, de aceea becurile sunt pe 2-5 și butonul pe 7
const int pinBecuri[] = {2, 3, 4, 5};
const int pinButon = 7;
const int pinVRx = A0;
const int pinVRy = A1;

const unsigned long DEBOUNCE_MS = 50;

// Variabile de stare
bool becuriPornite = true;
int stareButonAnterioara = HIGH;
unsigned long ultimaSchimbare = 0;

// Funcție pentru a controla toate becurile
void toateBecurile(bool stare) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(pinBecuri[i], stare);
  }
}

// Funcție pentru aprinderea unui bec
void aprindeBec(int index) {
  toateBecurile(LOW);
  digitalWrite(pinBecuri[index], HIGH);
}

void setup() {
  // Configurare pini becuri ca ieșire
  for (int i = 0; i < 4; i++) {
    pinMode(pinBecuri[i], OUTPUT);
    digitalWrite(pinBecuri[i], LOW);
  }

  // Configurare pin buton ca intrare
  pinMode(pinButon, INPUT_PULLUP);

  // Configurare pini joystick ca intrări
  pinMode(pinVRx, INPUT);
  pinMode(pinVRy, INPUT);
}

void loop() {
  // Citire stări joystick și buton
  int valoareVRx = analogRead(pinVRx);
  int valoareVRy = analogRead(pinVRy);
  int stareButon = digitalRead(pinButon);

  // Control becuri pe baza joystick-ului
  if (becuriPornite) {
    if (valoareVRx < 400) { // Joystick la stânga
      aprindeBec(0);
    } else if (valoareVRx > 600) { // Joystick la dreapta
      aprindeBec(2);
    } else if (valoareVRy < 400) { // Joystick în jos
      aprindeBec(1);
    } else if (valoareVRy > 600) { // Joystick în sus
      aprindeBec(3);
    } else { // Poziție neutră
      toateBecurile(LOW);
    }
  }

  // buton apăsat: contează doar apăsarea (HIGH -> LOW), debounce cu millis()
  unsigned long acum = millis();
  if (stareButon == LOW && stareButonAnterioara == HIGH &&
      acum - ultimaSchimbare >= DEBOUNCE_MS) {
    ultimaSchimbare = acum;
    becuriPornite = !becuriPornite;
    toateBecurile(becuriPornite ? HIGH : LOW);
  }
  stareButonAnterioara = stareButon;
}
