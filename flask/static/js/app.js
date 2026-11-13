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

// Utilidades comunes de la interfaz: peticiones, formatos y estados.

const ESTADOS = {
  "STAND-BY":         { color: "plain", desc: "desc_standby" },
  "SECANDO":          { color: "accent", desc: "desc_secando" },
  "SECADO COMPLETO":  { color: "ok", desc: "desc_completo" },
  "FALLA DHT22":      { color: "warn", desc: "desc_pausa", aviso: "alerta_falla" },
  "FALLA HC-SR04":    { color: "warn", desc: "desc_pausa", aviso: "alerta_falla_presencia" },
  "FALLA SEGURIDAD":  { color: "off", desc: "desc_pausa", emergencia: true },
  "FALLA GAS":        { color: "off", desc: "desc_pausa", emergencia: true },
  "SOBRETEMPERATURA": { color: "off", desc: "desc_pausa", emergencia: true },
  "ALERTA GAS":       { color: "off", desc: "desc_pausa", emergencia: true },
};

async function pedirJSON(url, opciones) {
  const resp = await fetch(url, opciones);
  const datos = await resp.json().catch(() => ({}));
  if (!resp.ok) throw Object.assign(new Error(datos.error || resp.statusText), { datos });
  return datos;
}

function $(selector) { return document.querySelector(selector); }

/**
 * Ejecuta `tarea` (async) ahora y luego cada `ms`, esperando a que termine antes de
 * programar la siguiente. Se pausa con la pestaña oculta o el celular bloqueado y se
 * reanuda al volver, con una consulta inmediata.
 */
function repetir(tarea, ms) {
  let temporizador = null;
  let enCurso = false;
  let repetirAlTerminar = false;

  async function ciclo() {
    clearTimeout(temporizador);
    temporizador = null;
    if (document.hidden) return;
    if (enCurso) { repetirAlTerminar = true; return; }  // nunca dos consultas a la vez
    enCurso = true;
    try { await tarea(); } finally {
      enCurso = false;
      if (repetirAlTerminar) { repetirAlTerminar = false; ciclo(); }
      else if (!document.hidden) temporizador = setTimeout(ciclo, ms);
    }
  }

  document.addEventListener("visibilitychange", () => {
    if (document.hidden) { clearTimeout(temporizador); temporizador = null; }
    else ciclo();
  });
  ciclo();
  return ciclo;  // llamar para forzar una consulta inmediata
}

/** Agrupa eventos seguidos (como resize) en una sola llamada por cuadro. */
function porCuadro(funcion) {
  let pendiente = false;
  return () => {
    if (pendiente) return;
    pendiente = true;
    requestAnimationFrame(() => { pendiente = false; funcion(); });
  };
}

function fmtNum(valor, decimales = 1) {
  return valor === null || valor === undefined ? "--" : Number(valor).toFixed(decimales);
}

function fmtTiempo(segundos) {
  if (segundos === null || segundos === undefined || segundos < 0) return "--:--";
  const h = Math.floor(segundos / 3600);
  const m = Math.floor((segundos % 3600) / 60);
  const s = Math.floor(segundos % 60);
  const mmss = `${String(m).padStart(2, "0")}:${String(s).padStart(2, "0")}`;
  return h > 0 ? `${h}:${mmss}` : mmss;
}

function locale() { return IDIOMA === "en" ? "en-US" : "es-CL"; }

function fmtFecha(ts) {
  return new Date(ts * 1000).toLocaleString(locale(), {
    day: "2-digit", month: "2-digit", hour: "2-digit", minute: "2-digit", second: "2-digit",
  });
}

function fmtHora(ts) {
  return new Date(ts * 1000).toLocaleTimeString(locale(), { hour: "2-digit", minute: "2-digit" });
}

function claseTag(color) {
  return { ok: "tag tag-ok", warn: "tag tag-warn", off: "tag tag-off", plain: "tag tag-plain" }[color] || "tag";
}

function escaparHTML(texto) {
  return String(texto).replace(/[&<>"']/g, (c) => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;" }[c]));
}
