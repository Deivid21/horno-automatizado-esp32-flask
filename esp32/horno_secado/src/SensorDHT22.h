/*
 * SensorDHT22 - Temperatura y humedad de la cámara
 *
 * Es el sensor de control: sus datos se muestran en la pantalla y
 * regulan el calefactor hacia la temperatura objetivo.
 */
#pragma once

#include <Arduino.h>
#include <DHT.h>

class SensorDHT22 {
public:
  explicit SensorDHT22(uint8_t pin);

  void iniciar();
  void actualizar(unsigned long ahora);
  void imprimir() const;

  float temperatura() const { return _temperatura; }
  float humedad() const { return _humedad; }
  bool  tieneDatos() const { return !isnan(_temperatura); }
  bool  enFalla() const;

private:
  DHT           _dht;
  float         _temperatura = NAN;
  float         _humedad     = NAN;
  uint8_t       _fallas      = 0;
  unsigned long _ultimaLectura = 0;
};
