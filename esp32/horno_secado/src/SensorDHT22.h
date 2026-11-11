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
 * SensorDHT22 - Temperatura y humedad de la cámara
 *
 * Es el sensor de control: sus datos se muestran en la pantalla y
 * regulan el calefactor hacia la temperatura objetivo.
 */
#pragma once

#include <Arduino.h>
#include <DHT.h>

class SensorDHT22 {
public:
  explicit SensorDHT22(uint8_t pin);

  void iniciar();
  void actualizar(unsigned long ahora);
  void imprimir() const;

  float temperatura() const { return _temperatura; }
  float humedad() const { return _humedad; }
  bool  tieneDatos() const { return !isnan(_temperatura); }
  bool  enFalla() const;

private:
  DHT           _dht;
  float         _temperatura = NAN;
  float         _humedad     = NAN;
  uint8_t       _fallas      = 0;
  unsigned long _ultimaLectura = 0;
};
