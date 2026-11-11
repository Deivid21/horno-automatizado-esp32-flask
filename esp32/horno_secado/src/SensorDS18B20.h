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

/*
 * SensorDS18B20 - Sensor de seguridad térmica
 *
 * Vigila la temperatura de la cámara de forma independiente al DHT22.
 * Sobre DS18B20_TEMP_EMERGENCIA declara emergencia y el sistema se apaga;
 * bajo DS18B20_TEMP_REARME el sistema vuelve a operar solo.
 * Sus datos se muestran solo en el monitor serial.
 */
#pragma once

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class SensorDS18B20 {
public:
  explicit SensorDS18B20(uint8_t pin);

  void iniciar();
  void actualizar(unsigned long ahora);
  void imprimir() const;

  float temperatura() const { return _temperatura; }
  bool  encontrado() const { return _encontrado; }
  bool  enEmergencia() const { return _emergencia; }
  bool  enFalla() const;

private:
  OneWire           _bus;
  DallasTemperature _sensor;
  float             _temperatura = NAN;
  bool              _encontrado  = false;
  bool              _emergencia  = false;
  uint8_t           _fallas      = 0;
  unsigned long     _ultimaLectura = 0;
};
