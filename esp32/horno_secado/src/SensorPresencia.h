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
 * SensorPresencia - HC-SR04 dentro de la cámara
 *
 * Detecta si hay un objeto (endoscopio) a menos de PRESENCIA_DISTANCIA_CM.
 * Si no hay nada, el horno pasa a stand-by.
 *
 * Falla: con la cámara cerrada siempre debería haber eco, del objeto o de la
 * pared del fondo. Si no llega eco durante PRESENCIA_LECTURAS_FALLA lecturas
 * seguidas, el sensor está desconectado o dañado. Mientras falla se conserva
 * el último estado conocido para no dar por retirado el endoscopio.
 */
#pragma once

#include <Arduino.h>

class SensorPresencia {
public:
  SensorPresencia(uint8_t pinTrig, uint8_t pinEcho);

  void iniciar();
  void actualizar(unsigned long ahora);
  void imprimir() const;

  float distancia() const { return _distanciaCm; }  // -1 = fuera de rango
  bool  hayObjeto() const { return _hayObjeto; }
  bool  enFalla() const;

private:
  float medir();

  uint8_t       _pinTrig;
  uint8_t       _pinEcho;
  float         _distanciaCm = -1;
  bool          _hayObjeto   = false;
  uint8_t       _contador    = 0;
  uint8_t       _sinEco      = 0;
  unsigned long _ultimaLectura = 0;
};
