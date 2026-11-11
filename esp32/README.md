# Firmware ESP32

Firmware del horno para Arduino IDE. Cada sensor y actuador vive en su propio
módulo dentro de `src/`; el sketch principal solo coordina el estado del horno.

## Estructura

```
horno_secado/
├── horno_secado.ino          Estados, modos de secado y coordinación
├── config.h                  Pines, umbrales y tiempos
└── src/
    ├── SensorDHT22.h/.cpp      Temperatura y humedad (control y pantalla)
    ├── SensorDS18B20.h/.cpp    Seguridad térmica
    ├── SensorPresencia.h/.cpp  HC-SR04: objeto dentro / stand-by
    ├── SensorGas.h/.cpp        MQ-2: precalentamiento y umbral
    ├── Buzzer.h/.cpp           Patrones de alarma sin bloquear
    ├── Actuadores.h/.cpp       Ventilador y calefactor
    └── Pantalla.h/.cpp         LCD 20x4 I2C
```

Para ajustar el comportamiento en pruebas basta con editar `config.h`.

## Librerías

Instalar desde el Gestor de librerías del Arduino IDE:

- **DHT sensor library** (Adafruit) + **Adafruit Unified Sensor**
- **OneWire** (Paul Stoffregen)
- **DallasTemperature** (Miles Burton)
- **LiquidCrystal I2C** (Frank de Brabander)

Placa: **ESP32 Dev Module**. Abrir `horno_secado/horno_secado.ino`; el IDE
compila automáticamente los módulos de `src/`.

## Conexiones

| Componente        | Pin ESP32 | Notas                                        |
|-------------------|-----------|----------------------------------------------|
| DHT22 DATA        | GPIO 27   | Pull-up 10k si el módulo no la trae          |
| DS18B20 DATA      | GPIO 13   | Pull-up **4.7k** a 3.3 V, alimentar a 3.3 V  |
| MQ-2 A0           | GPIO 26   | **Divisor de voltaje** 5 V → 3.3 V           |
| HC-SR04 TRIG      | GPIO 14   |                                              |
| HC-SR04 ECHO      | GPIO 34   | **Divisor de voltaje** 5 V → 3.3 V           |
| LCD SDA           | GPIO 21   | Dirección I2C 0x27 (o 0x3F)                  |
| LCD SCL           | GPIO 22   |                                              |
| Buzzer            | GPIO 32   | Buzzer activo                                |
| Ventilador 12 V   | GPIO 25   | MOSFET de nivel lógico, diodo en paralelo    |
| Calefactor        | GPIO 33   | Reservado: optoacoplador + transistor        |

> El GPIO 26 es ADC2 y deja de leer si se activa el WiFi del ESP32. La
> comunicación con la Raspberry Pi debe ser por Serial/USB, o mover el MQ-2 a
> un pin ADC1 (32–39).

Todos los módulos de 5 V y el ESP32 deben compartir GND, igual que la fuente
de 12 V del ventilador.

## Estados y reglas

La primera condición verdadera decide el estado:

| Prioridad | Estado            | Condición                          | Salidas                          | Buzzer               |
|-----------|-------------------|------------------------------------|----------------------------------|----------------------|
| 1         | ALERTA GAS        | MQ-2 > 1500 (tras 60 s)            | Sistema apagado                  | Continuo             |
| 2         | SOBRETEMPERATURA  | DS18B20 ≥ 70 °C                    | Sistema apagado                  | Intermitente rápido  |
| 3         | FALLA SEGURIDAD   | DS18B20 sin lecturas válidas       | Sistema apagado                  | Doble pitido         |
| 4         | FALLA DHT22       | DHT22 sin lecturas válidas         | Calefactor apagado               | Pitido cada 2 s      |
| 5         | STAND-BY          | HC-SR04 no detecta objeto (10 cm)  | Ventilador encendido             | —                    |
| 6         | SECADO COMPLETO   | Terminó el tiempo del modo         | Calefactor apagado               | 3 pitidos            |
| 7         | SECANDO           | Objeto dentro, ciclo en curso      | Control a 30 °C, ventilador ON   | 1 pitido al iniciar  |

- **Rearme automático:** las emergencias se liberan solas cuando los valores
  vuelven a la normalidad (DS18B20 < 65 °C, gas < 1400).
- **Pausa:** durante una falla o emergencia el tiempo de secado queda en pausa
  y continúa al volver a la normalidad.
- **Reinicio:** al retirar el objeto de la cámara el ciclo vuelve a cero.
- **Ventilador:** siempre encendido salvo en emergencias
  (`VENTILADOR_EN_EMERGENCIA` en `config.h`).
- **Calefactor:** control on/off a 30 °C con histéresis de 1 °C. Desactivado
  hasta conectar la resistencia (`CALEFACTOR_INSTALADO` en `config.h`).

## Pantalla

```
T: 30.1C  H: 45.2%
Modo: 30 min
SECANDO 12:34
Vent:ON   Calef:N/C
```

## Monitor serial

115200 baudios, con salto de línea. Cada 2 s se imprime un reporte:

```
============ HORNO DE SECADO ============
[DHT22]   Temp: 30.1 C | Hum: 45.2 % | Objetivo: 30.0 C
[DS18B20] Temp: 30.6 C | Limite: 70.0 C | OK
[HC-SR04] Dist: 6.3 cm | Rango: 10 cm | Objeto dentro
[MQ-2]    ADC: 850 | Umbral: 1500 | OK
[SISTEMA] Estado: SECANDO | Modo: 30 min | Restante: 12:34
[SALIDAS] Ventilador: ON | Calefactor: no instalado
```

Los cambios de estado se registran con `[EVENTO]`.

Después de cada reporte se envía una línea JSON para el servidor Flask de la
Raspberry Pi. Las líneas que no empiezan con `{` son solo para lectura humana:

```json
{"temp":30.1,"hum":45.2,"temp_ds":30.6,"dist":6.3,"objeto":true,"gas":850,
 "gas_calentando":false,"estado":"SECANDO","modo":30,"restante":754,
 "ventilador":true,"calefactor":false,"calefactor_instalado":false,"uptime":1234}
```

Los valores sin lectura llegan como `null`; `restante` vale `-1` cuando no hay
ciclo en curso.

| Comando        | Acción                               |
|----------------|--------------------------------------|
| `5`, `15`, `30`, `45`, `60` | Cambia el modo de secado (minutos) |
| `?`            | Muestra la ayuda                     |
