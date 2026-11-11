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

#include "SensorPresencia.h"
#include "../config.h"

// Tiempo máximo de espera del eco (~2 m); limita cuánto puede bloquear pulseIn
static constexpr unsigned long TIMEOUT_ECO_US = 12000;

SensorPresencia::SensorPresencia(uint8_t pinTrig, uint8_t pinEcho)
    : _pinTrig(pinTrig), _pinEcho(pinEcho) {}

void SensorPresencia::iniciar() {
  pinMode(_pinTrig, OUTPUT);
  pinMode(_pinEcho, INPUT);
  digitalWrite(_pinTrig, LOW);
}

float SensorPresencia::medir() {
  digitalWrite(_pinTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(_pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(_pinTrig, LOW);

  unsigned long duracion = pulseIn(_pinEcho, HIGH, TIMEOUT_ECO_US);
  if (duracion == 0) return -1;
  return duracion * 0.0343f / 2.0f;  // velocidad del sonido: 343 m/s
}

void SensorPresencia::actualizar(unsigned long ahora) {
  if (ahora - _ultimaLectura < INTERVALO_PRESENCIA_MS) return;
  _ultimaLectura = ahora;

  _distanciaCm = medir();
  bool lectura = (_distanciaCm > 0) && (_distanciaCm < PRESENCIA_DISTANCIA_CM);

  // Filtro: el estado cambia solo con varias lecturas iguales seguidas
  if (lectura != _hayObjeto) {
    if (++_contador >= PRESENCIA_LECTURAS) {
      _hayObjeto = lectura;
      _contador  = 0;
    }
  } else {
    _contador = 0;
  }
}

void SensorPresencia::imprimir() const {
  if (_distanciaCm < 0) {
    Serial.printf("[HC-SR04] Dist: fuera de rango | %s\n",
                  _hayObjeto ? "Objeto dentro" : "Camara vacia");
    return;
  }
  Serial.printf("[HC-SR04] Dist: %.1f cm | Rango: %.0f cm | %s\n",
                _distanciaCm, PRESENCIA_DISTANCIA_CM,
                _hayObjeto ? "Objeto dentro" : "Camara vacia");
}
