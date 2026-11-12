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

#include "SensorDS18B20.h"
#include "../config.h"

SensorDS18B20::SensorDS18B20(uint8_t pin) : _bus(pin), _sensor(&_bus) {}

void SensorDS18B20::iniciar() {
  _sensor.begin();
  _encontrado = _sensor.getDeviceCount() > 0;
  _sensor.setResolution(12);
  _sensor.setWaitForConversion(false);  // no bloquea el loop
  _sensor.requestTemperatures();
  _ultimaLectura = millis();
}

// Lee la conversión pedida en la vuelta anterior y pide la siguiente
void SensorDS18B20::actualizar(unsigned long ahora) {
  if (ahora - _ultimaLectura < INTERVALO_DS18B20_MS) return;
  _ultimaLectura = ahora;

  float t = _sensor.getTempCByIndex(0);
  _sensor.requestTemperatures();

  // -127 = desconectado; 85 = valor de encendido, aún sin medición real
  if (t == DEVICE_DISCONNECTED_C || t == 85.0f) {
    if (_fallas < 255) _fallas++;
    // En falla se borra el valor; _emergencia se conserva para no rearmar a ciegas
    if (enFalla()) _temperatura = NAN;
    return;
  }

  _temperatura = t;
  _fallas      = 0;
  _encontrado  = true;

  // Emergencia con histéresis: se rearma solo cuando baja lo suficiente
  if (t >= DS18B20_TEMP_EMERGENCIA)   _emergencia = true;
  else if (t < DS18B20_TEMP_REARME)   _emergencia = false;
}

bool SensorDS18B20::enFalla() const {
  return _fallas >= FALLAS_MAX;
}

void SensorDS18B20::imprimir() const {
  if (enFalla()) {
    Serial.println("[DS18B20] Temp: --- | FALLA: sin lecturas (revisa la conexión y la pull-up de 4.7k)");
    return;
  }
  if (isnan(_temperatura)) {
    Serial.println("[DS18B20] Esperando la primera lectura...");
    return;
  }
  Serial.printf("[DS18B20] Temp: %.1f C | Limite: %.1f C | %s\n",
                _temperatura, DS18B20_TEMP_EMERGENCIA, _emergencia ? "EMERGENCIA" : "OK");
}
