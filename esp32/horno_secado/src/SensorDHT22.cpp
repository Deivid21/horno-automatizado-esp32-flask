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

#include "SensorDHT22.h"
#include "../config.h"

SensorDHT22::SensorDHT22(uint8_t pin) : _dht(pin, DHT22) {}

void SensorDHT22::iniciar() {
  _dht.begin();
  _ultimaLectura = millis();
}

void SensorDHT22::actualizar(unsigned long ahora) {
  if (ahora - _ultimaLectura < INTERVALO_DHT22_MS) return;
  _ultimaLectura = ahora;

  float t = _dht.readTemperature();
  float h = _dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    if (_fallas < 255) _fallas++;
    return;  // conserva el último valor válido
  }

  _temperatura = t;
  _humedad     = h;
  _fallas      = 0;
}

bool SensorDHT22::enFalla() const {
  return _fallas >= FALLAS_MAX;
}

void SensorDHT22::imprimir() const {
  if (!tieneDatos()) {
    Serial.println("[DHT22]   Sin datos (revisa la conexión)");
    return;
  }
  Serial.printf("[DHT22]   Temp: %.1f C | Hum: %.1f %% | Objetivo: %.1f C%s\n",
                _temperatura, _humedad, TEMP_OBJETIVO,
                enFalla() ? " | FALLA" : "");
}
