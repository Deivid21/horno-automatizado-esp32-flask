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
 *
 * Falla: un MQ-2 alimentado nunca marca 0. Si la lectura queda bajo
 * GAS_MIN_VALIDO durante GAS_LECTURAS_FALLA lecturas seguidas, el sensor
 * está desconectado o sin alimentación. La resistencia del divisor de
 * voltaje que va del GPIO a GND lleva el pin a 0 si se corta el cable.
 *
 * Pin flotante: si se suelta el cable justo en el GPIO, el pin queda al
 * aire y el ADC lee ruido que sube y baja. Para detectarlo, cada lectura
 * hace una prueba de carga: carga el pin a 0 y lo lee, luego a 3.3 V y lo
 * lee. Con el divisor conectado ambas lecturas dan el valor real; con el
 * pin al aire conserva la carga y la diferencia es enorme. Una lectura
 * flotante no cuenta para la alarma de gas.
 */
#pragma once

#include <Arduino.h>
#include "../config.h"

class SensorGas {
public:
  explicit SensorGas(uint8_t pin);

  void iniciar();
  void actualizar(unsigned long ahora);
  void imprimir() const;

  int  valor() const { return _valor; }
  bool calentando() const;
  bool enEmergencia() const { return _emergencia; }
  bool enFalla() const;
  bool flotante() const { return _flotante >= GAS_LECTURAS_FALLA; }

private:
  int leerForzado(uint8_t nivel);

  uint8_t       _pin;
  int           _valor      = 0;
  bool          _emergencia = false;
  uint8_t       _sinSenal   = 0;
  uint8_t       _flotante   = 0;
  unsigned long _inicio     = 0;
  unsigned long _ultimaLectura = 0;
};
