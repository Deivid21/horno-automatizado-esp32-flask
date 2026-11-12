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
 * Buzzer - Patrones de alarma sin bloquear el loop
 *
 * El programa principal elige un patrón según el estado del horno y
 * llama a actualizar() en cada vuelta del loop.
 */
#pragma once

#include <Arduino.h>

enum class PatronBuzzer : uint8_t {
  SILENCIO,
  PITIDO_CORTO,   // una vez: objeto detectado, inicia el secado
  TRES_PITIDOS,   // una vez: secado completo
  LENTO,          // pitido cada 2 s: falla del DHT22
  LARGO,          // pitido largo cada 3 s: falla del sensor de presencia
  DOBLE,          // doble pitido cada 1 s: falla del sensor de seguridad
  TRIPLE,         // triple pitido cada 1.5 s: falla del sensor de gas
  RAPIDO,         // intermitente rápido: sobretemperatura
  CONTINUO        // sin pausa: gas detectado
};

class Buzzer {
public:
  explicit Buzzer(uint8_t pin);

  void iniciar();
  void patron(PatronBuzzer nuevo);
  void actualizar(unsigned long ahora);
  void pitidoPrueba();  // bloqueante, solo para setup()

  bool sonando() const { return _sonando; }

private:
  uint8_t       _pin;
  PatronBuzzer  _patron  = PatronBuzzer::SILENCIO;
  unsigned long _inicio  = 0;
  bool          _sonando = false;
};
