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

#include "SensorGas.h"
#include "../config.h"

static constexpr uint8_t MUESTRAS = 10;  // promedio para bajar el ruido del ADC

SensorGas::SensorGas(uint8_t pin) : _pin(pin) {}

void SensorGas::iniciar() {
  analogReadResolution(12);                  // 0-4095
  analogSetPinAttenuation(_pin, ADC_11db);   // rango ~0-3.1 V
  _inicio = millis();
  _ultimaLectura = _inicio;
}

bool SensorGas::calentando() const {
  return millis() - _inicio < GAS_PRECALENTAMIENTO_MS;
}

void SensorGas::actualizar(unsigned long ahora) {
  if (ahora - _ultimaLectura < INTERVALO_GAS_MS) return;
  _ultimaLectura = ahora;

  long suma = 0;
  for (uint8_t i = 0; i < MUESTRAS; i++) {
    suma += analogRead(_pin);
    delayMicroseconds(200);
  }
  _valor = suma / MUESTRAS;

  if (calentando()) {
    _emergencia = false;
    _sinSenal   = 0;
    return;
  }

  // Falla: lectura pegada en 0 = cable cortado o módulo sin alimentación
  if (_valor < GAS_MIN_VALIDO) {
    if (_sinSenal < 255) _sinSenal++;
  } else {
    _sinSenal = 0;
  }

  if (_valor > GAS_UMBRAL)                        _emergencia = true;
  else if (_valor < GAS_UMBRAL - GAS_HISTERESIS)  _emergencia = false;
}

bool SensorGas::enFalla() const {
  return _sinSenal >= GAS_LECTURAS_FALLA;
}

void SensorGas::imprimir() const {
  if (calentando()) {
    unsigned long restante = (GAS_PRECALENTAMIENTO_MS - (millis() - _inicio)) / 1000;
    Serial.printf("[MQ-2]    ADC: %d | Precalentando (%lu s)\n", _valor, restante);
    return;
  }
  if (enFalla()) {
    Serial.printf("[MQ-2]    ADC: %d | FALLA: sin señal (revisa la conexión de A0 y la alimentación)\n", _valor);
    return;
  }
  Serial.printf("[MQ-2]    ADC: %d | Umbral: %d | %s\n",
                _valor, GAS_UMBRAL, _emergencia ? "GAS DETECTADO" : "OK");
}
