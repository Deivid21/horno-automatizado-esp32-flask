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
  "FALLA DHT22":      { color: "warn", desc: "desc_pausa" },
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
