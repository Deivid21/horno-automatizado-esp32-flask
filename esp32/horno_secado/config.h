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
 * config.h - Configuración general del Horno de Secado para Endoscopios
 *
 * Todos los pines, umbrales y tiempos del sistema están aquí.
 * Para ajustar el comportamiento en pruebas, modifica solo este archivo.
 */
#pragma once

#include <Arduino.h>

// ============================================================
// Pines del ESP32
// ============================================================
constexpr uint8_t PIN_DHT22      = 27;  // DHT22 DATA
constexpr uint8_t PIN_DS18B20    = 13;  // DS18B20 DATA (pull-up 4.7k a 3.3 V)
constexpr uint8_t PIN_MQ2        = 26;  // MQ-2 A0 (ADC2, divisor de voltaje 5 V -> 3.3 V)
constexpr uint8_t PIN_TRIG       = 14;  // HC-SR04 TRIG
constexpr uint8_t PIN_ECHO       = 34;  // HC-SR04 ECHO (divisor de voltaje 5 V -> 3.3 V)
constexpr uint8_t PIN_SDA        = 21;  // LCD I2C SDA
constexpr uint8_t PIN_SCL        = 22;  // LCD I2C SCL
constexpr uint8_t PIN_BUZZER     = 32;  // Buzzer activo
constexpr uint8_t PIN_VENTILADOR = 25;  // Gate del MOSFET del ventilador 12 V
constexpr uint8_t PIN_CALEFACTOR = 33;  // Reservado: resistencia (optoacoplador + transistor)

// ============================================================
// Control de temperatura (DHT22)
// ============================================================
constexpr float TEMP_OBJETIVO   = 30.0f;  // °C, temperatura ideal de secado
constexpr float TEMP_HISTERESIS = 1.0f;   // °C, el calefactor enciende bajo (objetivo - histéresis)

// La resistencia aún no está conectada; mientras sea false su pin queda siempre en LOW
constexpr bool CALEFACTOR_INSTALADO = false;

// En una emergencia el sistema se apaga; true = el ventilador sigue encendido
constexpr bool VENTILADOR_EN_EMERGENCIA = false;

// ============================================================
// Modos de secado (minutos). Se cambian escribiendo 15, 30 o 60 en el monitor serial
// ============================================================
constexpr uint8_t MODO_CORTO      = 15;
constexpr uint8_t MODO_NORMAL     = 30;
constexpr uint8_t MODO_LARGO      = 60;
constexpr uint8_t MODO_POR_DEFECTO = MODO_NORMAL;

// ============================================================
// Seguridad: DS18B20
// ============================================================
constexpr float DS18B20_TEMP_EMERGENCIA = 70.0f;  // °C, apaga el sistema
constexpr float DS18B20_TEMP_REARME     = 65.0f;  // °C, el sistema vuelve a operar bajo este valor

// ============================================================
// Seguridad: MQ-2
// ============================================================
constexpr int           GAS_UMBRAL              = 1500;    // ADC 0-4095, calibrar en aire limpio
constexpr int           GAS_HISTERESIS          = 100;     // la alarma se apaga bajo (umbral - histéresis)
constexpr unsigned long GAS_PRECALENTAMIENTO_MS = 60000UL; // el MQ-2 no es confiable el primer minuto

// ============================================================
// Presencia: HC-SR04
// ============================================================
constexpr float   PRESENCIA_DISTANCIA_CM = 10.0f;  // menos que esto = hay un objeto en la cámara
constexpr uint8_t PRESENCIA_LECTURAS     = 3;      // lecturas iguales seguidas para confirmar el cambio

// ============================================================
// Fallas y pantalla
// ============================================================
constexpr uint8_t FALLAS_MAX    = 5;     // lecturas fallidas seguidas para declarar falla de sensor
constexpr uint8_t LCD_DIRECCION = 0x27;  // si la pantalla no muestra nada, prueba 0x3F

// ============================================================
// Intervalos (ms)
// ============================================================
constexpr unsigned long INTERVALO_DHT22_MS     = 2000;  // el DHT22 no lee más rápido
constexpr unsigned long INTERVALO_DS18B20_MS   = 1000;  // conversión de 12 bits tarda 750 ms
constexpr unsigned long INTERVALO_PRESENCIA_MS = 300;
constexpr unsigned long INTERVALO_GAS_MS       = 500;
constexpr unsigned long INTERVALO_PANTALLA_MS  = 1000;
constexpr unsigned long INTERVALO_REPORTE_MS   = 2000;  // reporte en el monitor serial
