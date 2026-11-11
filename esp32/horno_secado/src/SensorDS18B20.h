/*
 * SensorDS18B20 - Sensor de seguridad térmica
 *
 * Vigila la temperatura de la cámara de forma independiente al DHT22.
 * Sobre DS18B20_TEMP_EMERGENCIA declara emergencia y el sistema se apaga;
 * bajo DS18B20_TEMP_REARME el sistema vuelve a operar solo.
 * Sus datos se muestran solo en el monitor serial.
 */
#pragma once

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class SensorDS18B20 {
public:
  explicit SensorDS18B20(uint8_t pin);

  void iniciar();
  void actualizar(unsigned long ahora);
  void imprimir() const;

  float temperatura() const { return _temperatura; }
  bool  encontrado() const { return _encontrado; }
  bool  enEmergencia() const { return _emergencia; }
  bool  enFalla() const;

private:
  OneWire           _bus;
  DallasTemperature _sensor;
  float             _temperatura = NAN;
  bool              _encontrado  = false;
  bool              _emergencia  = false;
  uint8_t           _fallas      = 0;
  unsigned long     _ultimaLectura = 0;
};
