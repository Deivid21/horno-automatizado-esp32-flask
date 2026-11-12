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

"""Monitor del horno: guarda el último estado recibido, registra eventos y ciclos."""
import json
import logging
import threading
import time

from .database import BaseDatos

log = logging.getLogger(__name__)

EMERGENCIAS = {"ALERTA GAS", "SOBRETEMPERATURA", "FALLA SEGURIDAD", "FALLA GAS"}
FALLAS = {"FALLA DHT22"}


def tipo_evento(estado: str) -> str:
    if estado in EMERGENCIAS:
        return "emergencia"
    if estado in FALLAS:
        return "falla"
    if estado == "SECADO COMPLETO":
        return "completo"
    return "info"


class Monitor:
    def __init__(self, db: BaseDatos, intervalo_guardado_s: int):
        self.db = db
        self.intervalo_guardado_s = intervalo_guardado_s
        self._lock = threading.Lock()
        self._datos: dict | None = None
        self._recibido = 0.0
        self._ultimo_guardado = 0.0
        self._ciclo: dict | None = None  # {id, modo, temps, hum}

    # ---------- Entrada ----------
    def procesar_linea(self, linea: str) -> bool:
        """Procesa una línea del ESP32. Solo las líneas JSON traen datos."""
        linea = linea.strip()
        if not linea.startswith("{"):
            return False
        try:
            datos = json.loads(linea)
        except json.JSONDecodeError:
            log.debug("Línea JSON incompleta: %s", linea)
            return False
        self.actualizar(datos)
        return True

    def actualizar(self, datos: dict) -> None:
        ahora = time.time()
        with self._lock:
            anterior = self._datos.get("estado") if self._datos else None
            self._datos = datos
            self._recibido = ahora

        nuevo = datos.get("estado")
        if nuevo and anterior and nuevo != anterior:
            self.db.registrar_evento(anterior, nuevo, tipo_evento(nuevo))
            log.info("Estado: %s -> %s", anterior, nuevo)

        self._seguir_ciclo(nuevo, datos)

        if ahora - self._ultimo_guardado >= self.intervalo_guardado_s:
            self._ultimo_guardado = ahora
            self.db.guardar_lectura(datos)

    # ---------- Ciclos de secado ----------
    def _seguir_ciclo(self, estado: str | None, d: dict) -> None:
        modo = d.get("modo")

        if self._ciclo is None:
            if estado != "SECANDO" or not modo:
                return
            self._ciclo = {"id": self.db.iniciar_ciclo(modo), "modo": modo, "temps": [], "hum": None}

        if modo and modo != self._ciclo["modo"]:
            self._ciclo["modo"] = modo
            self.db.actualizar_modo_ciclo(self._ciclo["id"], modo)

        if estado == "SECANDO" and d.get("temp") is not None:
            self._ciclo["temps"].append(d["temp"])
        if d.get("hum") is not None:
            self._ciclo["hum"] = d["hum"]

        if estado == "SECADO COMPLETO":
            self._cerrar_ciclo("completo")
        elif not d.get("objeto"):
            self._cerrar_ciclo("interrumpido")

    def _cerrar_ciclo(self, resultado: str) -> None:
        temps = self._ciclo["temps"]
        prom = round(sum(temps) / len(temps), 1) if temps else None
        self.db.cerrar_ciclo(self._ciclo["id"], resultado, prom, self._ciclo["hum"])
        log.info("Ciclo %s: %s", self._ciclo["id"], resultado)
        self._ciclo = None

    # ---------- Salida ----------
    def estado(self, vencido_s: int) -> dict:
        with self._lock:
            datos = dict(self._datos) if self._datos else {}
            antiguedad = time.time() - self._recibido if self._recibido else None
        return {
            "datos": datos,
            "conectado": antiguedad is not None and antiguedad < vencido_s,
            "antiguedad_s": round(antiguedad, 1) if antiguedad is not None else None,
        }
