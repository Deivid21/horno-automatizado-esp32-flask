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

#include "Actuadores.h"
#include "../config.h"

Actuadores::Actuadores(uint8_t pinVentilador, uint8_t pinCalefactor)
    : _pinVentilador(pinVentilador), _pinCalefactor(pinCalefactor) {}

void Actuadores::iniciar() {
  pinMode(_pinVentilador, OUTPUT);
  pinMode(_pinCalefactor, OUTPUT);
  apagarTodo();
}

void Actuadores::ventilador(bool encender) {
  _ventilador = encender;
  digitalWrite(_pinVentilador, encender ? HIGH : LOW);
}

void Actuadores::calefactor(bool encender) {
  _calefactor = encender && CALEFACTOR_INSTALADO;
  digitalWrite(_pinCalefactor, _calefactor ? HIGH : LOW);
}

// Enciende bajo (objetivo - histéresis) y apaga al llegar al objetivo
void Actuadores::regularTemperatura(float temperatura) {
  if (isnan(temperatura))                                  calefactor(false);
  else if (temperatura <= TEMP_OBJETIVO - TEMP_HISTERESIS) calefactor(true);
  else if (temperatura >= TEMP_OBJETIVO)                   calefactor(false);
  // entre ambos valores mantiene el estado actual
}

void Actuadores::apagarTodo() {
  ventilador(false);
  calefactor(false);
}

void Actuadores::imprimir() const {
  Serial.printf("[SALIDAS] Ventilador: %s | Calefactor: %s\n",
                _ventilador ? "ON" : "OFF",
                CALEFACTOR_INSTALADO ? (_calefactor ? "ON" : "OFF") : "no instalado");
}
