//
// For more Support see: https://t.me/Deivid21Hub
//
// Copyright (C) 1996 - 2026 INACAP
// Copyright (C) 2017 - 2026 Deivid Ignacio Parra (Deivid21)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

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
    case PatronBuzzer::TRIPLE:       on = ((t % 1500) < 500) && ((t % 1500) % 200 < 100);  break;
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
