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

INTERVALO_LIMPIEZA_S = 24 * 3600  # las lecturas antiguas se borran una vez al día

# Tipo de cada estado del ESP32 (los que no aparecen son "info").
# Es la única definición: la interfaz la recibe desde aquí (window.TIPOS_ESTADO).
TIPOS_ESTADO = {
    "ALERTA GAS": "emergencia",
    "SOBRETEMPERATURA": "emergencia",
    "FALLA SEGURIDAD": "emergencia",
    "FALLA GAS": "emergencia",
    "FALLA DHT22": "falla",
    "FALLA HC-SR04": "falla",
    "SECADO COMPLETO": "completo",
}


def tipo_evento(estado: str) -> str:
    return TIPOS_ESTADO.get(estado, "info")


def validar_limites(nuevos: object, actuales: dict) -> dict:
    """Toma de `nuevos` solo las claves conocidas con el tipo correcto."""
    if not isinstance(nuevos, dict):
        return actuales
    limites = dict(actuales)
    for clave, valor in nuevos.items():
        if clave not in actuales:
            continue
        if clave == "modos":
            if isinstance(valor, list) and valor and all(isinstance(m, int) and m > 0 for m in valor):
                limites["modos"] = valor
        elif isinstance(valor, (int, float)) and not isinstance(valor, bool):
            limites[clave] = valor
    return limites


class Monitor:
    def __init__(self, db: BaseDatos, intervalo_guardado_s: int, dias_retencion: int,
                 limites: dict):
        self.db = db
        self._limites = dict(limites)
        self.intervalo_guardado_s = intervalo_guardado_s
        self.dias_retencion = dias_retencion
        self._ultima_limpieza = 0.0
        self._lock = threading.Lock()
        self._datos: dict | None = None
        self._recibido = 0.0
        self._ultimo_guardado = 0.0
        self._ciclo: dict | None = None  # {id, modo, suma_temp, n_temp, hum}

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
        if not isinstance(datos, dict):
            return False
        if "limites" in datos:
            self.actualizar_limites(datos["limites"])
        else:
            self.actualizar(datos)
        return True

    def actualizar_limites(self, nuevos: dict) -> None:
        with self._lock:
            anteriores = self._limites
            self._limites = validar_limites(nuevos, anteriores)
            cambiaron = self._limites != anteriores
        if cambiaron:
            log.info("Límites recibidos del ESP32: %s", self._limites)

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

        if ahora - self._ultima_limpieza >= INTERVALO_LIMPIEZA_S:
            self._ultima_limpieza = ahora
            self.db.limpiar(self.dias_retencion)

    # ---------- Ciclos de secado ----------
    def _seguir_ciclo(self, estado: str | None, d: dict) -> None:
        modo = d.get("modo")

        if self._ciclo is None:
            if estado != "SECANDO" or not modo:
                return
            self._ciclo = {"id": self.db.iniciar_ciclo(modo), "modo": modo,
                           "suma_temp": 0.0, "n_temp": 0, "hum": None}

        if modo and modo != self._ciclo["modo"]:
            self._ciclo["modo"] = modo
            self.db.actualizar_modo_ciclo(self._ciclo["id"], modo)

        # Promedio acumulado: suma y cantidad, sin guardar cada lectura en memoria
        if estado == "SECANDO" and d.get("temp") is not None:
            self._ciclo["suma_temp"] += d["temp"]
            self._ciclo["n_temp"] += 1
        if d.get("hum") is not None:
            self._ciclo["hum"] = d["hum"]

        if estado == "SECADO COMPLETO":
            self._cerrar_ciclo("completo")
        elif not d.get("objeto"):
            self._cerrar_ciclo("interrumpido")

    def _cerrar_ciclo(self, resultado: str) -> None:
        n = self._ciclo["n_temp"]
        prom = round(self._ciclo["suma_temp"] / n, 1) if n else None
        self.db.cerrar_ciclo(self._ciclo["id"], resultado, prom, self._ciclo["hum"])
        log.info("Ciclo %s: %s", self._ciclo["id"], resultado)
        self._ciclo = None

    # ---------- Salida ----------
    def limites(self) -> dict:
        with self._lock:
            return dict(self._limites)

    def estado(self, vencido_s: int) -> dict:
        with self._lock:
            datos = dict(self._datos) if self._datos else {}
            antiguedad = time.time() - self._recibido if self._recibido else None
        return {
            "datos": datos,
            "limites": self.limites(),
            "conectado": antiguedad is not None and antiguedad < vencido_s,
            "antiguedad_s": round(antiguedad, 1) if antiguedad is not None else None,
        }
