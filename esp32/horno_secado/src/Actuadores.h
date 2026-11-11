/*
 * Actuadores - Ventilador 12 V (MOSFET) y resistencia calefactora
 *
 * La resistencia se conectará a futuro con optoacoplador + transistor.
 * Mientras CALEFACTOR_INSTALADO sea false, su pin queda siempre en LOW.
 */
#pragma once

#include <Arduino.h>

class Actuadores {
public:
  Actuadores(uint8_t pinVentilador, uint8_t pinCalefactor);

  void iniciar();
  void ventilador(bool encender);
  void calefactor(bool encender);
  void regularTemperatura(float temperatura);  // control on/off con histéresis
  void apagarTodo();
  void imprimir() const;

  bool ventiladorEncendido() const { return _ventilador; }
  bool calefactorEncendido() const { return _calefactor; }

private:
  uint8_t _pinVentilador;
  uint8_t _pinCalefactor;
  bool    _ventilador = false;
  bool    _calefactor = false;
};
