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
 * Actuadores - Ventilador 12 V (MOSFET) y resistencia calefactora
 *
 * La resistencia se conectará a futuro con optoacoplador + transistor.
 * Mientras CALEFACTOR_INSTALADO sea false, su pin queda siempre en LOW.
 */
#pragma once

#include <Arduino.h>

class Actuadores {
public:
  Actuadores(uint8_t pinVentilador, uint8_t pinCalefactor);

  void iniciar();
  void ventilador(bool encender);
  void calefactor(bool encender);
  void regularTemperatura(float temperatura);  // control on/off con histéresis
  void apagarTodo();
  void imprimir() const;

  bool ventiladorEncendido() const { return _ventilador; }
  bool calefactorEncendido() const { return _calefactor; }

private:
  uint8_t _pinVentilador;
  uint8_t _pinCalefactor;
  bool    _ventilador = false;
  bool    _calefactor = false;
};
