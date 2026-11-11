#include "Actuadores.h"
#include "../config.h"

Actuadores::Actuadores(uint8_t pinVentilador, uint8_t pinCalefactor)
    : _pinVentilador(pinVentilador), _pinCalefactor(pinCalefactor) {}

void Actuadores::iniciar() {
  pinMode(_pinVentilador, OUTPUT);
  pinMode(_pinCalefactor, OUTPUT);
  apagarTodo();
}

void Actuadores::ventilador(bool encender) {
  _ventilador = encender;
  digitalWrite(_pinVentilador, encender ? HIGH : LOW);
}

void Actuadores::calefactor(bool encender) {
  _calefactor = encender && CALEFACTOR_INSTALADO;
  digitalWrite(_pinCalefactor, _calefactor ? HIGH : LOW);
}

// Enciende bajo (objetivo - histéresis) y apaga al llegar al objetivo
void Actuadores::regularTemperatura(float temperatura) {
  if (isnan(temperatura))                                  calefactor(false);
  else if (temperatura <= TEMP_OBJETIVO - TEMP_HISTERESIS) calefactor(true);
  else if (temperatura >= TEMP_OBJETIVO)                   calefactor(false);
  // entre ambos valores mantiene el estado actual
}

void Actuadores::apagarTodo() {
  ventilador(false);
  calefactor(false);
}

void Actuadores::imprimir() const {
  Serial.printf("[SALIDAS] Ventilador: %s | Calefactor: %s\n",
                _ventilador ? "ON" : "OFF",
                CALEFACTOR_INSTALADO ? (_calefactor ? "ON" : "OFF") : "no instalado");
}
