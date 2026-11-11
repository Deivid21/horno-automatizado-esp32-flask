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
