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
 * Pantalla - LCD 20x4 con módulo I2C (PCF8574)
 *
 * Muestra los datos del DHT22, el modo de secado, el estado del horno
 * y las salidas. Cada línea se reescribe completa para evitar restos.
 */
#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

struct DatosPantalla {
  float       temperatura;
  float       humedad;
  bool        dhtConDatos;
  uint8_t     modoMinutos;
  const char* estado;
  long        restanteSeg;  // -1 = no mostrar cuenta regresiva
  bool        ventilador;
  bool        calefactor;
};

class Pantalla {
public:
  explicit Pantalla(uint8_t direccion);

  void iniciar(uint8_t pinSda, uint8_t pinScl);
  void bienvenida(bool ds18b20Encontrado);  // bloqueante, solo para setup()
  void mostrar(const DatosPantalla& datos);

private:
  void linea(uint8_t fila, const char* texto);

  LiquidCrystal_I2C _lcd;
};
