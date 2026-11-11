/*
 * Buzzer - Patrones de alarma sin bloquear el loop
 *
 * El programa principal elige un patrón según el estado del horno y
 * llama a actualizar() en cada vuelta del loop.
 */
#pragma once

#include <Arduino.h>

enum class PatronBuzzer : uint8_t {
  SILENCIO,
  PITIDO_CORTO,   // una vez: objeto detectado, inicia el secado
  TRES_PITIDOS,   // una vez: secado completo
  LENTO,          // pitido cada 2 s: falla del DHT22
  DOBLE,          // doble pitido cada 1 s: falla del sensor de seguridad
  RAPIDO,         // intermitente rápido: sobretemperatura
  CONTINUO        // sin pausa: gas detectado
};

class Buzzer {
public:
  explicit Buzzer(uint8_t pin);

  void iniciar();
  void patron(PatronBuzzer nuevo);
  void actualizar(unsigned long ahora);
  void pitidoPrueba();  // bloqueante, solo para setup()

  bool sonando() const { return _sonando; }

private:
  uint8_t       _pin;
  PatronBuzzer  _patron  = PatronBuzzer::SILENCIO;
  unsigned long _inicio  = 0;
  bool          _sonando = false;
};
