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

"""Servidor Flask del Horno de Secado para Endoscopios.

Lee el estado del ESP32 por USB, lo guarda en SQLite y lo muestra en una
interfaz web local. Ejecutar con:  python app.py
"""
import logging

from flask import Flask, jsonify, render_template, request

import config
from horno.database import BaseDatos
from horno.monitor import Monitor

HISTORIAL_HORAS = (1, 6, 24)


def crear_app() -> Flask:
    app = Flask(__name__)

    db = BaseDatos(config.BASE_DATOS)
    db.limpiar(config.DIAS_RETENCION)
    monitor = Monitor(db, config.INTERVALO_GUARDADO_S)

    if config.MODO_DEMO:
        from horno.simulador import Simulador
        fuente = Simulador(monitor)
    else:
        from horno.conexion import ConexionSerial
        fuente = ConexionSerial(config.PUERTO_SERIAL, config.BAUDIOS, monitor)
    fuente.start()

    limites = {
        "temp_objetivo": config.TEMP_OBJETIVO,
        "ds18b20_emergencia": config.DS18B20_TEMP_EMERGENCIA,
        "gas_umbral": config.GAS_UMBRAL,
        "modos": config.MODOS_MINUTOS,
    }

    # ---------- Páginas ----------
    @app.get("/")
    def panel():
        return render_template("panel.html", pagina="panel", limites=limites)

    @app.get("/historial")
    def historial():
        return render_template("historial.html", pagina="historial", limites=limites)

    @app.get("/registro")
    def registro():
        return render_template("registro.html", pagina="registro", limites=limites)

    # ---------- API ----------
    @app.get("/api/estado")
    def api_estado():
        estado = monitor.estado(config.DATOS_VENCIDOS_S)
        estado["demo"] = config.MODO_DEMO
        return jsonify(estado)

    @app.post("/api/modo")
    def api_modo():
        minutos = (request.get_json(silent=True) or {}).get("minutos")
        if minutos not in config.MODOS_MINUTOS:
            return jsonify(ok=False, error="modo_invalido"), 400
        if not fuente.enviar(str(minutos)):
            return jsonify(ok=False, error="sin_conexion"), 503
        return jsonify(ok=True, minutos=minutos)

    @app.get("/api/historial")
    def api_historial():
        horas = request.args.get("horas", 1, type=int)
        if horas not in HISTORIAL_HORAS:
            horas = 1
        return jsonify(horas=horas, lecturas=db.lecturas_desde(horas * 3600))

    @app.get("/api/eventos")
    def api_eventos():
        limite = min(request.args.get("limite", 50, type=int), 500)
        return jsonify(db.ultimos_eventos(limite))

    @app.get("/api/ciclos")
    def api_ciclos():
        limite = min(request.args.get("limite", 50, type=int), 500)
        return jsonify(db.ultimos_ciclos(limite))

    return app


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format="%(asctime)s [%(levelname)s] %(name)s: %(message)s")
    # use_reloader=False: el recargador de Flask abriría el puerto serial dos veces
    crear_app().run(host=config.HOST, port=config.PUERTO_WEB, debug=False, use_reloader=False)
