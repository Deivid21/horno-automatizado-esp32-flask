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

#include "Pantalla.h"
#include <Wire.h>
#include "../config.h"

static constexpr uint8_t COLUMNAS = 20;
static constexpr uint8_t FILAS    = 4;

Pantalla::Pantalla(uint8_t direccion) : _lcd(direccion, COLUMNAS, FILAS) {}

void Pantalla::iniciar(uint8_t pinSda, uint8_t pinScl) {
  Wire.begin(pinSda, pinScl);
  _lcd.init();
  _lcd.backlight();
  _lcd.clear();
}

// Escribe la línea completa rellenando con espacios hasta 20 columnas
void Pantalla::linea(uint8_t fila, const char* texto) {
  char buf[COLUMNAS + 1];
  snprintf(buf, sizeof(buf), "%-20s", texto);
  _lcd.setCursor(0, fila);
  _lcd.print(buf);
}

void Pantalla::bienvenida(bool ds18b20Encontrado) {
  linea(0, "  HORNO DE SECADO");
  linea(1, "   ENDOSCOPIOS");
  linea(2, ds18b20Encontrado ? "Seguridad: OK" : "Seguridad: SIN DS18B");
  linea(3, "Iniciando...");
  delay(2000);
  _lcd.clear();
}

void Pantalla::mostrar(const DatosPantalla& d) {
  char buf[32];

  // Fila 0: datos del DHT22
  if (d.dhtConDatos)
    snprintf(buf, sizeof(buf), "T:%5.1fC  H:%5.1f%%", d.temperatura, d.humedad);
  else
    snprintf(buf, sizeof(buf), "DHT22: sin datos");
  linea(0, buf);

  // Fila 1: modo de secado
  snprintf(buf, sizeof(buf), "Modo: %u min", d.modoMinutos);
  linea(1, buf);

  // Fila 2: estado y cuenta regresiva
  if (d.restanteSeg >= 0)
    snprintf(buf, sizeof(buf), "%s %02ld:%02ld", d.estado, d.restanteSeg / 60, d.restanteSeg % 60);
  else
    snprintf(buf, sizeof(buf), "%s", d.estado);
  linea(2, buf);

  // Fila 3: salidas
  snprintf(buf, sizeof(buf), "Vent:%s  Calef:%s",
           d.ventilador ? "ON " : "OFF",
           CALEFACTOR_INSTALADO ? (d.calefactor ? "ON" : "OFF") : "N/C");
  linea(3, buf);
}
