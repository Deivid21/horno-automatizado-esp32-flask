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
 * SensorGas - MQ-2 (salida analógica)
 *
 * Tras el precalentamiento, si la lectura supera GAS_UMBRAL declara
 * emergencia y el sistema se apaga; se rearma solo cuando baja de
 * (GAS_UMBRAL - GAS_HISTERESIS).
 */
#pragma once

#include <Arduino.h>

class SensorGas {
public:
  explicit SensorGas(uint8_t pin);

  void iniciar();
  void actualizar(unsigned long ahora);
  void imprimir() const;

  int  valor() const { return _valor; }
  bool calentando() const;
  bool enEmergencia() const { return _emergencia; }

private:
  uint8_t       _pin;
  int           _valor      = 0;
  bool          _emergencia = false;
  unsigned long _inicio     = 0;
  unsigned long _ultimaLectura = 0;
};
