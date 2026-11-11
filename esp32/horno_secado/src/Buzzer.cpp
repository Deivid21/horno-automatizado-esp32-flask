#include "Buzzer.h"

Buzzer::Buzzer(uint8_t pin) : _pin(pin) {}

void Buzzer::iniciar() {
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
}

// El patrón reinicia su tiempo solo cuando cambia
void Buzzer::patron(PatronBuzzer nuevo) {
  if (nuevo == _patron) return;
  _patron = nuevo;
  _inicio = millis();
}

void Buzzer::actualizar(unsigned long ahora) {
  unsigned long t = ahora - _inicio;
  bool on = false;

  switch (_patron) {
    case PatronBuzzer::SILENCIO:     on = false;                                   break;
    case PatronBuzzer::PITIDO_CORTO: on = t < 150;                                 break;
    case PatronBuzzer::TRES_PITIDOS: on = (t < 1500) && ((t % 500) < 250);         break;
    case PatronBuzzer::LENTO:        on = (t % 2000) < 150;                        break;
    case PatronBuzzer::DOBLE:        on = ((t % 1000) < 100) ||
                                          ((t % 1000) >= 200 && (t % 1000) < 300); break;
    case PatronBuzzer::RAPIDO:       on = (t % 400) < 200;                         break;
    case PatronBuzzer::CONTINUO:     on = true;                                    break;
  }

  _sonando = on;
  digitalWrite(_pin, on ? HIGH : LOW);
}

void Buzzer::pitidoPrueba() {
  digitalWrite(_pin, HIGH);
  delay(200);
  digitalWrite(_pin, LOW);
}
