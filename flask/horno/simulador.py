#
# For more Support see: https://t.me/Deivid21Hub
#
# Copyright (C) 1996 - 2026 INACAP
# Copyright (C) 2017 - 2026 Deivid Ignacio Parra (Deivid21)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

"""Simulador del ESP32 para probar la interfaz sin hardware (HORNO_DEMO=1).

Genera los mismos datos JSON que el ESP32 y recorre un ciclo completo:
stand-by, secado, secado completo y retiro del objeto. El tiempo de secado
corre acelerado para no esperar 15 minutos en cada prueba.
"""
import logging
import random
import threading
import time

import config

from .monitor import Monitor

log = logging.getLogger(__name__)

INTERVALO_S = 2
ACELERACION = 30            # 1 s real = 30 s de secado
STAND_BY_S = 20             # tiempo con la cámara vacía
ESPERA_RETIRO_S = 15        # tiempo antes de retirar el objeto al terminar


class Simulador(threading.Thread):
    def __init__(self, monitor: Monitor):
        super().__init__(name="simulador", daemon=True)
        self.monitor = monitor
        self.modo = 30
        self.temp = 22.0
        self.hum = 65.0
        self.inicio = time.time()

    @property
    def conectado(self) -> bool:
        return True

    def enviar(self, comando: str) -> bool:
        if comando.isdigit() and int(comando) in config.MODOS_MINUTOS:
            self.modo = int(comando)
            log.info("[demo] Modo de secado: %s min", self.modo)
        return True

    def run(self) -> None:
        log.info("Modo demo activo: datos simulados")
        fase, desde, transcurrido = "vacio", time.time(), 0.0
        while True:
            ahora = time.time()
            en_fase = ahora - desde

            if fase == "vacio" and en_fase > STAND_BY_S:
                fase, desde, transcurrido = "secando", ahora, 0.0
            elif fase == "secando":
                transcurrido += INTERVALO_S * ACELERACION
                if transcurrido >= self.modo * 60:
                    fase, desde = "completo", ahora
            elif fase == "completo" and en_fase > ESPERA_RETIRO_S:
                fase, desde = "vacio", ahora

            objeto = fase != "vacio"

            # La temperatura sube hacia 30 °C y la humedad baja mientras seca
            meta_temp, meta_hum = (30.0, 22.0) if fase == "secando" else (24.0, 60.0)
            self.temp += (meta_temp - self.temp) * 0.08 + random.uniform(-0.15, 0.15)
            self.hum += (meta_hum - self.hum) * 0.05 + random.uniform(-0.4, 0.4)

            estado = {"vacio": "STAND-BY", "secando": "SECANDO", "completo": "SECADO COMPLETO"}[fase]
            restante = int(self.modo * 60 - transcurrido) if fase == "secando" else -1

            self.monitor.actualizar({
                "temp": round(self.temp, 1),
                "hum": round(self.hum, 1),
                "temp_ds": round(self.temp + random.uniform(0.2, 0.8), 1),
                "dist": round(random.uniform(4, 7), 1) if objeto else None,
                "objeto": objeto,
                "gas": random.randint(700, 900),
                "gas_calentando": ahora - self.inicio < 60,
                "estado": estado,
                "modo": self.modo,
                "restante": restante,
                "ventilador": True,
                "calefactor": False,
                "calefactor_instalado": False,
                "uptime": int(ahora - self.inicio),
            })
            time.sleep(INTERVALO_S)
