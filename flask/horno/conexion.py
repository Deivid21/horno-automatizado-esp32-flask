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

"""Conexión serial con el ESP32: lee las líneas en segundo plano y envía comandos."""
import logging
import threading
import time

from .monitor import Monitor

log = logging.getLogger(__name__)

ESPERA_RECONEXION_S = 3


class ConexionSerial(threading.Thread):
    def __init__(self, puerto: str, baudios: int, monitor: Monitor):
        super().__init__(name="conexion-serial", daemon=True)
        self.puerto = puerto
        self.baudios = baudios
        self.monitor = monitor
        self._serial = None
        self._lock = threading.Lock()

    @property
    def conectado(self) -> bool:
        return self._serial is not None and self._serial.is_open

    def run(self) -> None:
        try:
            import serial  # pyserial
        except ImportError:
            log.error("Falta pyserial: instala las dependencias con 'pip install -r requirements.txt'")
            return

        while True:
            try:
                # Al abrir el puerto el ESP32 se reinicia; es normal
                with serial.Serial(self.puerto, self.baudios, timeout=1) as puerto:
                    self._serial = puerto
                    log.info("Conectado al ESP32 en %s", self.puerto)
                    while True:
                        linea = puerto.readline()
                        if linea:
                            self.monitor.procesar_linea(linea.decode("utf-8", errors="ignore"))
            except (serial.SerialException, OSError) as error:
                log.warning("Sin conexión con %s (%s). Reintentando en %d s",
                            self.puerto, error, ESPERA_RECONEXION_S)
            finally:
                self._serial = None
            time.sleep(ESPERA_RECONEXION_S)

    def enviar(self, comando: str) -> bool:
        with self._lock:
            if not self.conectado:
                return False
            try:
                self._serial.write((comando + "\n").encode())
                return True
            except OSError as error:
                log.warning("No se pudo enviar '%s': %s", comando, error)
                return False
