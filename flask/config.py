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

"""Configuración del servidor Flask del Horno de Secado.

Cada valor se puede sobrescribir con una variable de entorno, así no hace
falta editar el código en la Raspberry Pi.
"""
import os
from pathlib import Path

BASE_DIR = Path(__file__).resolve().parent

# Conexión con el ESP32 por USB
PUERTO_SERIAL = os.environ.get("HORNO_PUERTO", "/dev/ttyUSB0")
BAUDIOS = int(os.environ.get("HORNO_BAUDIOS", "115200"))

# Modo demo: genera datos simulados sin ESP32 conectado (HORNO_DEMO=1)
MODO_DEMO = os.environ.get("HORNO_DEMO", "0") == "1"

# Servidor web
HOST = os.environ.get("HORNO_HOST", "0.0.0.0")
PUERTO_WEB = int(os.environ.get("HORNO_PUERTO_WEB", "5000"))

# Base de datos
BASE_DATOS = Path(os.environ.get("HORNO_DB", BASE_DIR / "datos" / "horno.db"))
INTERVALO_GUARDADO_S = 10   # cada cuánto se guarda una lectura en el historial
DIAS_RETENCION = 30         # las lecturas más antiguas se borran una vez al día

# Sin datos del ESP32 durante este tiempo = desconectado
DATOS_VENCIDOS_S = 10

# Límites del horno: el ESP32 envía los suyos (config.h) al encender y cada 30 s,
# y el servidor los usa en vez de estos. Estos valores solo se usan mientras no
# llegan (y en el modo demo), así que no hace falta mantenerlos sincronizados.
LIMITES_POR_DEFECTO = {
    "modos": [5, 15, 30, 45, 60],
    "modo_min": 1,        # tiempo personalizado mínimo (min)
    "modo_max": 180,      # tiempo personalizado máximo (min)
    "temp_objetivo": 30.0,
    "ds18b20_emergencia": 70.0,
    "gas_umbral": 1500,
}
