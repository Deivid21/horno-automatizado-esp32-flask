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

"""Base de datos SQLite: lecturas de sensores, eventos de estado y ciclos de secado."""
import sqlite3
import threading
import time
from contextlib import closing
from pathlib import Path

ESQUEMA = """
CREATE TABLE IF NOT EXISTS lecturas (
    id      INTEGER PRIMARY KEY AUTOINCREMENT,
    ts      REAL    NOT NULL,
    temp    REAL,
    hum     REAL,
    temp_ds REAL,
    gas     INTEGER,
    dist    REAL,
    estado  TEXT
);
CREATE INDEX IF NOT EXISTS idx_lecturas_ts ON lecturas (ts);

CREATE TABLE IF NOT EXISTS eventos (
    id       INTEGER PRIMARY KEY AUTOINCREMENT,
    ts       REAL NOT NULL,
    anterior TEXT,
    nuevo    TEXT NOT NULL,
    tipo     TEXT NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_eventos_ts ON eventos (ts);

CREATE TABLE IF NOT EXISTS ciclos (
    id        INTEGER PRIMARY KEY AUTOINCREMENT,
    inicio    REAL NOT NULL,
    fin       REAL,
    modo      INTEGER NOT NULL,
    resultado TEXT,
    temp_prom REAL,
    hum_final REAL
);
"""

MAX_PUNTOS_HISTORIAL = 600  # el historial se reduce a este máximo de puntos


class BaseDatos:
    def __init__(self, ruta: Path):
        self.ruta = Path(ruta)
        self.ruta.parent.mkdir(parents=True, exist_ok=True)
        self._lock = threading.Lock()
        with closing(self._conexion()) as con:
            # WAL: las lecturas del panel no esperan a las escrituras del hilo serial
            con.execute("PRAGMA journal_mode=WAL")
            con.executescript(ESQUEMA)

    def _conexion(self):
        con = sqlite3.connect(self.ruta, timeout=10)
        con.row_factory = sqlite3.Row
        con.execute("PRAGMA synchronous=NORMAL")  # seguro con WAL y menos escrituras en la SD
        return con

    def _escribir(self, sql: str, params: tuple = ()) -> int:
        with self._lock, closing(self._conexion()) as con, con:
            return con.execute(sql, params).lastrowid

    def _leer(self, sql: str, params: tuple = ()) -> list[dict]:
        with closing(self._conexion()) as con:
            return [dict(fila) for fila in con.execute(sql, params)]

    # ---------- Lecturas ----------
    def guardar_lectura(self, d: dict) -> None:
        self._escribir(
            "INSERT INTO lecturas (ts, temp, hum, temp_ds, gas, dist, estado) "
            "VALUES (?, ?, ?, ?, ?, ?, ?)",
            (time.time(), d.get("temp"), d.get("hum"), d.get("temp_ds"),
             d.get("gas"), d.get("dist"), d.get("estado")),
        )

    def lecturas_desde(self, segundos: int) -> list[dict]:
        desde = time.time() - segundos
        total = self._leer("SELECT COUNT(*) AS n FROM lecturas WHERE ts >= ?", (desde,))[0]["n"]
        paso = max(1, total // MAX_PUNTOS_HISTORIAL)
        return self._leer(
            "SELECT ts, temp, hum, temp_ds, gas, estado FROM ("
            "  SELECT *, ROW_NUMBER() OVER (ORDER BY ts) AS n FROM lecturas WHERE ts >= ?"
            ") WHERE (n - 1) % ? = 0 ORDER BY ts",
            (desde, paso),
        )

    def limpiar(self, dias: int) -> None:
        """Borra las lecturas más antiguas que `dias`. Eventos y ciclos se conservan."""
        self._escribir("DELETE FROM lecturas WHERE ts < ?", (time.time() - dias * 86400,))

    # ---------- Eventos ----------
    def registrar_evento(self, anterior: str | None, nuevo: str, tipo: str) -> None:
        self._escribir(
            "INSERT INTO eventos (ts, anterior, nuevo, tipo) VALUES (?, ?, ?, ?)",
            (time.time(), anterior, nuevo, tipo),
        )

    def ultimos_eventos(self, limite: int = 50) -> list[dict]:
        return self._leer("SELECT * FROM eventos ORDER BY ts DESC LIMIT ?", (limite,))

    # ---------- Ciclos ----------
    def cerrar_ciclos_abiertos(self) -> int:
        """Cierra los ciclos que quedaron sin terminar (servidor reiniciado o apagado)."""
        with self._lock, closing(self._conexion()) as con, con:
            cursor = con.execute(
                "UPDATE ciclos SET fin = COALESCE("
                "  (SELECT MAX(ts) FROM lecturas WHERE ts >= ciclos.inicio), inicio"
                "), resultado = 'interrumpido' WHERE fin IS NULL"
            )
            return cursor.rowcount

    def iniciar_ciclo(self, modo: int) -> int:
        return self._escribir("INSERT INTO ciclos (inicio, modo) VALUES (?, ?)", (time.time(), modo))

    def actualizar_modo_ciclo(self, ciclo_id: int, modo: int) -> None:
        self._escribir("UPDATE ciclos SET modo = ? WHERE id = ?", (modo, ciclo_id))

    def cerrar_ciclo(self, ciclo_id: int, resultado: str,
                     temp_prom: float | None, hum_final: float | None) -> None:
        self._escribir(
            "UPDATE ciclos SET fin = ?, resultado = ?, temp_prom = ?, hum_final = ? WHERE id = ?",
            (time.time(), resultado, temp_prom, hum_final, ciclo_id),
        )

    def ultimos_ciclos(self, limite: int = 50) -> list[dict]:
        return self._leer("SELECT * FROM ciclos ORDER BY inicio DESC LIMIT ?", (limite,))
