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
 * Horno de Secado Automatizado para Endoscopios - ESP32
 * Taller de Diseño de Proyectos Electrónicos - INACAP
 *
 * Programa principal: lee cada módulo, decide el estado del horno y
 * controla las salidas. Las lecturas y reglas de cada sensor viven en src/.
 *
 * Estados (de mayor a menor prioridad):
 *   ALERTA GAS        MQ-2 sobre el umbral           -> sistema apagado
 *   SOBRETEMPERATURA  DS18B20 sobre 70 °C            -> sistema apagado
 *   FALLA SEGURIDAD   DS18B20 sin lecturas           -> sistema apagado
 *   FALLA DHT22       DHT22 sin lecturas             -> calefactor apagado
 *   STAND-BY          HC-SR04 no detecta objeto      -> espera
 *   SECADO COMPLETO   terminó el tiempo del modo     -> espera retirar el objeto
 *   SECANDO           objeto dentro, ciclo en curso  -> control a 30 °C
 *
 * Las emergencias se rearman solas cuando los valores vuelven a lo normal.
 * Durante una emergencia o falla el tiempo de secado queda en pausa.
 *
 * Comandos por monitor serial (115200 baudios, con salto de línea):
 *   5 | 15 | 30 | 45 | 60   modos de secado (minutos)
 *   1 a 180                  tiempo personalizado (minutos)
 *   ?                        muestra la ayuda
 */
#include "config.h"
#include "src/SensorDHT22.h"
#include "src/SensorDS18B20.h"
#include "src/SensorPresencia.h"
#include "src/SensorGas.h"
#include "src/Buzzer.h"
#include "src/Actuadores.h"
#include "src/Pantalla.h"

// ============================================================
// Módulos
// ============================================================
SensorDHT22     dht22(PIN_DHT22);
SensorDS18B20   ds18b20(PIN_DS18B20);
SensorPresencia presencia(PIN_TRIG, PIN_ECHO);
SensorGas       gas(PIN_MQ2);
Buzzer          buzzer(PIN_BUZZER);
Actuadores      actuadores(PIN_VENTILADOR, PIN_CALEFACTOR);
Pantalla        pantalla(LCD_DIRECCION);

// ============================================================
// Estado del horno
// ============================================================
enum class Estado : uint8_t {
  STAND_BY,
  SECANDO,
  SECADO_COMPLETO,
  FALLA_DHT22,
  FALLA_SEGURIDAD,
  SOBRETEMPERATURA,
  ALERTA_GAS
};

Estado        estado         = Estado::STAND_BY;
uint8_t       modoMinutos    = MODO_POR_DEFECTO;
unsigned long transcurridoMs = 0;  // tiempo de secado acumulado
unsigned long ultimoLoop     = 0;
unsigned long ultimaPantalla = 0;
unsigned long ultimoReporte  = 0;

const char* nombreEstado(Estado e) {
  switch (e) {
    case Estado::STAND_BY:         return "STAND-BY";
    case Estado::SECANDO:          return "SECANDO";
    case Estado::SECADO_COMPLETO:  return "SECADO COMPLETO";
    case Estado::FALLA_DHT22:      return "FALLA DHT22";
    case Estado::FALLA_SEGURIDAD:  return "FALLA SEGURIDAD";
    case Estado::SOBRETEMPERATURA: return "SOBRETEMPERATURA";
    case Estado::ALERTA_GAS:       return "ALERTA GAS";
  }
  return "?";
}

bool esEmergencia(Estado e) {
  return e == Estado::ALERTA_GAS || e == Estado::SOBRETEMPERATURA ||
         e == Estado::FALLA_SEGURIDAD;
}

unsigned long duracionCicloMs() {
  return (unsigned long)modoMinutos * 60000UL;
}

long restanteSeg() {
  if (transcurridoMs >= duracionCicloMs()) return 0;
  return (long)((duracionCicloMs() - transcurridoMs) / 1000);
}

// ============================================================
// Reglas: la primera condición verdadera decide el estado
// ============================================================
Estado evaluarEstado() {
  if (gas.enEmergencia())              return Estado::ALERTA_GAS;
  if (ds18b20.enEmergencia())          return Estado::SOBRETEMPERATURA;
  if (ds18b20.enFalla())               return Estado::FALLA_SEGURIDAD;
  if (dht22.enFalla())                 return Estado::FALLA_DHT22;
  if (!presencia.hayObjeto())          return Estado::STAND_BY;
  if (transcurridoMs >= duracionCicloMs()) return Estado::SECADO_COMPLETO;
  return Estado::SECANDO;
}

void aplicarSalidas() {
  switch (estado) {
    case Estado::ALERTA_GAS:
      actuadores.calefactor(false);
      actuadores.ventilador(VENTILADOR_EN_EMERGENCIA);
      buzzer.patron(PatronBuzzer::CONTINUO);
      break;

    case Estado::SOBRETEMPERATURA:
      actuadores.calefactor(false);
      actuadores.ventilador(VENTILADOR_EN_EMERGENCIA);
      buzzer.patron(PatronBuzzer::RAPIDO);
      break;

    case Estado::FALLA_SEGURIDAD:
      actuadores.calefactor(false);
      actuadores.ventilador(VENTILADOR_EN_EMERGENCIA);
      buzzer.patron(PatronBuzzer::DOBLE);
      break;

    case Estado::FALLA_DHT22:  // sin dato de control no se calienta
      actuadores.calefactor(false);
      actuadores.ventilador(true);
      buzzer.patron(PatronBuzzer::LENTO);
      break;

    case Estado::STAND_BY:
      actuadores.calefactor(false);
      actuadores.ventilador(true);
      buzzer.patron(PatronBuzzer::SILENCIO);
      break;

    case Estado::SECADO_COMPLETO:
      actuadores.calefactor(false);
      actuadores.ventilador(true);
      buzzer.patron(PatronBuzzer::TRES_PITIDOS);
      break;

    case Estado::SECANDO:
      actuadores.regularTemperatura(dht22.temperatura());
      actuadores.ventilador(true);
      buzzer.patron(PatronBuzzer::PITIDO_CORTO);
      break;
  }
}

// ============================================================
// Tiempo de secado
// ============================================================
void actualizarTiempo(unsigned long delta) {
  if (!presencia.hayObjeto()) {
    transcurridoMs = 0;  // sin objeto, el próximo ciclo parte de cero
  } else if (estado == Estado::SECANDO) {
    transcurridoMs += delta;  // en fallas y emergencias queda en pausa
  }
}

// ============================================================
// Monitor serial
// ============================================================
// Acepta los modos fijos y cualquier tiempo personalizado dentro del rango
bool esModoValido(int minutos) {
  return minutos >= MODO_MIN_MINUTOS && minutos <= MODO_MAX_MINUTOS;
}

void mostrarAyuda() {
  Serial.println("Comandos:");
  Serial.print("  ");
  for (uint8_t i = 0; i < CANTIDAD_MODOS; i++) {
    Serial.printf(i == 0 ? "%u" : " | %u", MODOS_MINUTOS[i]);
  }
  Serial.println("  -> modos de secado (minutos)");
  Serial.printf("  %u a %u  -> tiempo personalizado (minutos)\n", MODO_MIN_MINUTOS, MODO_MAX_MINUTOS);
  Serial.println("  ?  -> esta ayuda");
}

void procesarComando(String cmd) {
  cmd.trim();
  if (cmd.length() == 0) return;

  if (cmd == "?") {
    mostrarAyuda();
    return;
  }

  int minutos = cmd.toInt();
  if (esModoValido(minutos)) {
    modoMinutos = minutos;
    Serial.printf("[COMANDO] Modo de secado: %u min\n", modoMinutos);
  } else {
    Serial.printf("[COMANDO] '%s' no reconocido. Escribe ? para ver la ayuda\n", cmd.c_str());
  }
}

void leerComandos() {
  static String buffer;
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      procesarComando(buffer);
      buffer = "";
    } else if (buffer.length() < 16) {
      buffer += c;
    }
  }
}

void registrarCambio(Estado anterior, Estado nuevo) {
  Serial.printf("[EVENTO]  %s -> %s\n", nombreEstado(anterior), nombreEstado(nuevo));
  if (esEmergencia(nuevo))        Serial.println("[EVENTO]  Sistema apagado por seguridad");
  else if (esEmergencia(anterior)) Serial.println("[EVENTO]  Valores normales, sistema rearmado");
}

void reporteSerial() {
  Serial.println("============ HORNO DE SECADO ============");
  dht22.imprimir();
  ds18b20.imprimir();
  presencia.imprimir();
  gas.imprimir();

  Serial.printf("[SISTEMA] Estado: %s | Modo: %u min", nombreEstado(estado), modoMinutos);
  if (presencia.hayObjeto() && estado != Estado::SECADO_COMPLETO) {
    long r = restanteSeg();
    Serial.printf(" | Restante: %02ld:%02ld%s", r / 60, r % 60,
                  estado == Estado::SECANDO ? "" : " (en pausa)");
  }
  Serial.println();
  actuadores.imprimir();
}

// Formatea un float para JSON (NaN -> null)
void jsonFloat(char* buf, size_t len, float valor) {
  if (isnan(valor)) snprintf(buf, len, "null");
  else              snprintf(buf, len, "%.1f", valor);
}

// Una línea JSON por reporte: la lee el servidor Flask de la Raspberry Pi.
// Las líneas que no empiezan con '{' son para lectura humana y Flask las ignora.
void enviarJSON() {
  char temp[12], hum[12], tempDs[12], dist[12];
  jsonFloat(temp,   sizeof(temp),   dht22.temperatura());
  jsonFloat(hum,    sizeof(hum),    dht22.humedad());
  jsonFloat(tempDs, sizeof(tempDs), ds18b20.temperatura());
  jsonFloat(dist,   sizeof(dist),   presencia.distancia() < 0 ? NAN : presencia.distancia());

  long restante = (presencia.hayObjeto() && estado != Estado::SECADO_COMPLETO) ? restanteSeg() : -1;

  Serial.printf("{\"temp\":%s,\"hum\":%s,\"temp_ds\":%s,\"dist\":%s,\"objeto\":%s,"
                "\"gas\":%d,\"gas_calentando\":%s,\"estado\":\"%s\",\"modo\":%u,"
                "\"restante\":%ld,\"ventilador\":%s,\"calefactor\":%s,"
                "\"calefactor_instalado\":%s,\"uptime\":%lu}\n",
                temp, hum, tempDs, dist,
                presencia.hayObjeto() ? "true" : "false",
                gas.valor(), gas.calentando() ? "true" : "false",
                nombreEstado(estado), modoMinutos, restante,
                actuadores.ventiladorEncendido() ? "true" : "false",
                actuadores.calefactorEncendido() ? "true" : "false",
                CALEFACTOR_INSTALADO ? "true" : "false",
                millis() / 1000);
}

void actualizarPantalla() {
  DatosPantalla d;
  d.temperatura = dht22.temperatura();
  d.humedad     = dht22.humedad();
  d.dhtConDatos = dht22.tieneDatos();
  d.modoMinutos = modoMinutos;
  d.estado      = nombreEstado(estado);
  d.restanteSeg = (estado == Estado::SECANDO) ? restanteSeg() : -1;
  d.ventilador  = actuadores.ventiladorEncendido();
  d.calefactor  = actuadores.calefactorEncendido();
  pantalla.mostrar(d);
}

// ============================================================
// Programa
// ============================================================
void setup() {
  Serial.begin(115200);

  actuadores.iniciar();
  buzzer.iniciar();
  dht22.iniciar();
  ds18b20.iniciar();
  presencia.iniciar();
  gas.iniciar();
  pantalla.iniciar(PIN_SDA, PIN_SCL);

  Serial.println();
  Serial.println("Horno de Secado para Endoscopios - ESP32");
  if (!ds18b20.encontrado())
    Serial.println("[AVISO]   DS18B20 no encontrado: revisa la conexión y la pull-up de 4.7k");
  mostrarAyuda();

  buzzer.pitidoPrueba();
  pantalla.bienvenida(ds18b20.encontrado());

  ultimoLoop = millis();
}

void loop() {
  unsigned long ahora = millis();
  unsigned long delta = ahora - ultimoLoop;
  ultimoLoop = ahora;

  leerComandos();

  dht22.actualizar(ahora);
  ds18b20.actualizar(ahora);
  presencia.actualizar(ahora);
  gas.actualizar(ahora);

  actualizarTiempo(delta);

  Estado nuevo = evaluarEstado();
  if (nuevo != estado) {
    registrarCambio(estado, nuevo);
    estado = nuevo;
  }

  aplicarSalidas();
  buzzer.actualizar(ahora);

  if (ahora - ultimaPantalla >= INTERVALO_PANTALLA_MS) {
    ultimaPantalla = ahora;
    actualizarPantalla();
  }
  if (ahora - ultimoReporte >= INTERVALO_REPORTE_MS) {
    ultimoReporte = ahora;
    reporteSerial();
    enviarJSON();
  }
}
