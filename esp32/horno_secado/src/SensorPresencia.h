/*
 * SensorPresencia - HC-SR04 dentro de la cámara
 *
 * Detecta si hay un objeto (endoscopio) a menos de PRESENCIA_DISTANCIA_CM.
 * Si no hay nada, el horno pasa a stand-by.
 */
#pragma once

#include <Arduino.h>

class SensorPresencia {
public:
  SensorPresencia(uint8_t pinTrig, uint8_t pinEcho);

  void iniciar();
  void actualizar(unsigned long ahora);
  void imprimir() const;

  float distancia() const { return _distanciaCm; }  // -1 = fuera de rango
  bool  hayObjeto() const { return _hayObjeto; }

private:
  float medir();

  uint8_t       _pinTrig;
  uint8_t       _pinEcho;
  float         _distanciaCm = -1;
  bool          _hayObjeto   = false;
  uint8_t       _contador    = 0;
  unsigned long _ultimaLectura = 0;
};
