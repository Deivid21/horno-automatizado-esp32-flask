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

// Traducciones español / inglés. Los elementos con data-i18n="clave" se
// traducen solos; el código usa t("clave") para los textos dinámicos.
const TEXTOS = {
  es: {
    marca: "Horno de Secado",
    nav_panel: "Panel", nav_historial: "Historial", nav_registro: "Registro",
    pie_ramo: "Taller de Diseño de Proyectos Electrónicos",
    theme_auto: "Tema: sistema", theme_light: "Tema: claro", theme_dark: "Tema: oscuro",
    eyebrow: "INACAP · Secado de endoscopios",
    panel_titulo: "Horno de Secado",
    panel_lead: "Estado en vivo del horno, sensores y salidas.",
    conectado: "Conectado", desconectado: "Sin conexión", demo: "Demo",
    sin_datos: "Esperando datos del ESP32…",
    ciclo: "Ciclo de secado", restante: "Restante", modo: "Modo de secado",
    modo_enviado: "Modo enviado", modo_error: "No se pudo enviar el modo",
    min: "min",
    sensores: "Sensores", salidas: "Salidas",
    dht_temp: "Temperatura (DHT22)", dht_hum: "Humedad (DHT22)",
    ds_temp: "Seguridad (DS18B20)", gas: "Gas (MQ-2)", presencia: "Presencia (HC-SR04)",
    objetivo: "Objetivo", limite: "Límite", umbral: "Umbral",
    calentando: "Precalentando", ok: "OK",
    objeto_dentro: "Objeto dentro", camara_vacia: "Cámara vacía", fuera_rango: "fuera de rango",
    ventilador: "Ventilador", calefactor: "Calefactor", no_instalado: "No instalado",
    encendido: "Encendido", apagado: "Apagado", uptime: "Tiempo encendido del ESP32",
    // Estados
    "STAND-BY": "Stand-by", "SECANDO": "Secando", "SECADO COMPLETO": "Secado completo",
    "FALLA DHT22": "Falla DHT22", "FALLA SEGURIDAD": "Falla sensor de seguridad",
    "SOBRETEMPERATURA": "Sobretemperatura", "ALERTA GAS": "Alerta de gas",
    desc_standby: "No hay objeto en la cámara.",
    desc_secando: "Secando a temperatura controlada.",
    desc_completo: "Ciclo terminado. Retira el endoscopio.",
    desc_pausa: "Ciclo en pausa hasta normalizar.",
    alerta_emergencia: "Sistema apagado por seguridad. Se rearmará solo cuando los valores vuelvan a la normalidad.",
    alerta_falla: "El DHT22 no responde. El calefactor está apagado.",
    // Historial
    historial_titulo: "Historial", historial_lead: "Evolución de los sensores registrada en la Raspberry Pi.",
    hora_1: "1 h", hora_6: "6 h", hora_24: "24 h",
    graf_temp: "Temperatura", graf_hum: "Humedad", graf_gas: "Gas",
    sin_historial: "Aún no hay lecturas en este rango.",
    // Registro
    registro_titulo: "Registro", registro_lead: "Ciclos de secado y cambios de estado del horno.",
    ciclos: "Ciclos de secado", eventos: "Eventos",
    col_inicio: "Inicio", col_duracion: "Duración", col_modo: "Modo", col_resultado: "Resultado",
    col_temp_prom: "Temp. promedio", col_hum_final: "Hum. final",
    col_fecha: "Fecha", col_cambio: "Cambio", col_tipo: "Tipo",
    completo: "Completo", interrumpido: "Interrumpido", en_curso: "En curso",
    emergencia: "Emergencia", falla: "Falla", info: "Info",
    sin_ciclos: "Aún no hay ciclos registrados.", sin_eventos: "Aún no hay eventos registrados.",
  },
  en: {
    marca: "Drying Oven",
    nav_panel: "Dashboard", nav_historial: "History", nav_registro: "Log",
    pie_ramo: "Electronic Project Design Workshop",
    theme_auto: "Theme: system", theme_light: "Theme: light", theme_dark: "Theme: dark",
    eyebrow: "INACAP · Endoscope drying",
    panel_titulo: "Drying Oven",
    panel_lead: "Live status of the oven, sensors and outputs.",
    conectado: "Connected", desconectado: "Disconnected", demo: "Demo",
    sin_datos: "Waiting for ESP32 data…",
    ciclo: "Drying cycle", restante: "Remaining", modo: "Drying mode",
    modo_enviado: "Mode sent", modo_error: "Could not send the mode",
    min: "min",
    sensores: "Sensors", salidas: "Outputs",
    dht_temp: "Temperature (DHT22)", dht_hum: "Humidity (DHT22)",
    ds_temp: "Safety (DS18B20)", gas: "Gas (MQ-2)", presencia: "Presence (HC-SR04)",
    objetivo: "Target", limite: "Limit", umbral: "Threshold",
    calentando: "Warming up", ok: "OK",
    objeto_dentro: "Object inside", camara_vacia: "Chamber empty", fuera_rango: "out of range",
    ventilador: "Fan", calefactor: "Heater", no_instalado: "Not installed",
    encendido: "On", apagado: "Off", uptime: "ESP32 uptime",
    "STAND-BY": "Stand-by", "SECANDO": "Drying", "SECADO COMPLETO": "Drying complete",
    "FALLA DHT22": "DHT22 fault", "FALLA SEGURIDAD": "Safety sensor fault",
    "SOBRETEMPERATURA": "Overtemperature", "ALERTA GAS": "Gas alert",
    desc_standby: "No object in the chamber.",
    desc_secando: "Drying at a controlled temperature.",
    desc_completo: "Cycle finished. Remove the endoscope.",
    desc_pausa: "Cycle paused until values return to normal.",
    alerta_emergencia: "System shut down for safety. It will rearm on its own when values return to normal.",
    alerta_falla: "The DHT22 is not responding. The heater is off.",
    historial_titulo: "History", historial_lead: "Sensor readings recorded on the Raspberry Pi.",
    hora_1: "1 h", hora_6: "6 h", hora_24: "24 h",
    graf_temp: "Temperature", graf_hum: "Humidity", graf_gas: "Gas",
    sin_historial: "No readings in this range yet.",
    registro_titulo: "Log", registro_lead: "Drying cycles and oven state changes.",
    ciclos: "Drying cycles", eventos: "Events",
    col_inicio: "Start", col_duracion: "Duration", col_modo: "Mode", col_resultado: "Result",
    col_temp_prom: "Avg. temp", col_hum_final: "Final hum.",
    col_fecha: "Date", col_cambio: "Change", col_tipo: "Type",
    completo: "Complete", interrumpido: "Interrupted", en_curso: "In progress",
    emergencia: "Emergency", falla: "Fault", info: "Info",
    sin_ciclos: "No cycles recorded yet.", sin_eventos: "No events recorded yet.",
  },
};

function idiomaGuardado() {
  try {
    const guardado = localStorage.getItem("idioma");
    if (guardado === "es" || guardado === "en") return guardado;
  } catch (e) { /* almacenamiento no disponible */ }
  return navigator.language && navigator.language.startsWith("en") ? "en" : "es";
}

let IDIOMA = idiomaGuardado();
document.documentElement.lang = IDIOMA;

function t(clave) {
  return (TEXTOS[IDIOMA] && TEXTOS[IDIOMA][clave]) || TEXTOS.es[clave] || clave;
}

function aplicarIdioma() {
  document.documentElement.lang = IDIOMA;
  document.querySelectorAll("[data-i18n]").forEach((el) => { el.textContent = t(el.dataset.i18n); });
  document.dispatchEvent(new CustomEvent("idioma"));
}

document.addEventListener("DOMContentLoaded", () => {
  aplicarIdioma();
  const boton = document.getElementById("lang-toggle");
  if (boton) {
    boton.addEventListener("click", () => {
      IDIOMA = IDIOMA === "es" ? "en" : "es";
      try { localStorage.setItem("idioma", IDIOMA); } catch (e) { /* sin almacenamiento */ }
      aplicarIdioma();
    });
  }
});
