# Horno Automatizado de Secado para Endoscopios

Proyecto del ramo **Taller de Diseño de Proyectos Electrónicos** (INACAP).

Horno automatizado para el secado de endoscopios, en particular de la goma distal.
Un **ESP32** lee los sensores, aplica las reglas de seguridad y controla el
ventilador, mientras que una **Raspberry Pi** aloja un servidor **Flask** local
que muestra los datos y el estado del horno.

## Estructura

```
.
├── esp32/
│   └── horno_secado/      Firmware del ESP32 (Arduino IDE)
└── flask/                 Servidor local en la Raspberry Pi (próximamente)
```

## Hardware

| Componente   | Función                                        |
|--------------|------------------------------------------------|
| ESP32        | Lectura de sensores, reglas y control          |
| DHT22        | Temperatura y humedad de la cámara (control)   |
| DS18B20      | Sensor de seguridad térmica                    |
| HC-SR04      | Detección del endoscopio dentro de la cámara   |
| MQ-2         | Detección de gas                               |
| LCD 20x4 I2C | Datos y estado del horno                       |
| Buzzer       | Alarmas                                        |
| Ventilador   | Ventilador 12 V con MOSFET                     |
| Raspberry Pi | Servidor Flask local                           |

La documentación del firmware, conexiones y reglas está en
[`esp32/README.md`](esp32/README.md).

## Licencia

Distribuido bajo la [Licencia Apache 2.0](LICENSE).

Copyright (C) 1996 - 2026 INACAP  
Copyright (C) 2017 - 2026 Deivid Ignacio Parra (Deivid21)

Soporte: [Deivid21Hub](https://t.me/Deivid21Hub)

## Autor

Deivid Ignacio Parra ([Deivid21](https://github.com/Deivid21))
