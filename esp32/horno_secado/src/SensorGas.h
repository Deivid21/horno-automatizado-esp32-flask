/*
 * SensorGas - MQ-2 (salida analógica)
 *
 * Tras el precalentamiento, si la lectura supera GAS_UMBRAL declara
 * emergencia y el sistema se apaga; se rearma solo cuando baja de
 * (GAS_UMBRAL - GAS_HISTERESIS).
 */
#pragma once

#include <Arduino.h>

class SensorGas {
public:
  explicit SensorGas(uint8_t pin);

  void iniciar();
  void actualizar(unsigned long ahora);
  void imprimir() const;

  int  valor() const { return _valor; }
  bool calentando() const;
  bool enEmergencia() const { return _emergencia; }

private:
  uint8_t       _pin;
  int           _valor      = 0;
  bool          _emergencia = false;
  unsigned long _inicio     = 0;
  unsigned long _ultimaLectura = 0;
};
