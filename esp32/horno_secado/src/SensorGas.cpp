#include "SensorGas.h"
#include "../config.h"

static constexpr uint8_t MUESTRAS = 10;  // promedio para bajar el ruido del ADC

SensorGas::SensorGas(uint8_t pin) : _pin(pin) {}

void SensorGas::iniciar() {
  analogReadResolution(12);                  // 0-4095
  analogSetPinAttenuation(_pin, ADC_11db);   // rango ~0-3.1 V
  _inicio = millis();
  _ultimaLectura = _inicio;
}

bool SensorGas::calentando() const {
  return millis() - _inicio < GAS_PRECALENTAMIENTO_MS;
}

void SensorGas::actualizar(unsigned long ahora) {
  if (ahora - _ultimaLectura < INTERVALO_GAS_MS) return;
  _ultimaLectura = ahora;

  long suma = 0;
  for (uint8_t i = 0; i < MUESTRAS; i++) {
    suma += analogRead(_pin);
    delayMicroseconds(200);
  }
  _valor = suma / MUESTRAS;

  if (calentando()) {
    _emergencia = false;
    return;
  }

  if (_valor > GAS_UMBRAL)                        _emergencia = true;
  else if (_valor < GAS_UMBRAL - GAS_HISTERESIS)  _emergencia = false;
}

void SensorGas::imprimir() const {
  if (calentando()) {
    unsigned long restante = (GAS_PRECALENTAMIENTO_MS - (millis() - _inicio)) / 1000;
    Serial.printf("[MQ-2]    ADC: %d | Precalentando (%lu s)\n", _valor, restante);
    return;
  }
  Serial.printf("[MQ-2]    ADC: %d | Umbral: %d | %s\n",
                _valor, GAS_UMBRAL, _emergencia ? "GAS DETECTADO" : "OK");
}
