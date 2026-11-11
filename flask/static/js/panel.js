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

// Panel en vivo: consulta /api/estado cada 2 s y permite cambiar el modo.
let ultimo = null;
let campoPrellenado = false;

function medidor(id, porcentaje, color) {
  const el = document.getElementById(id);
  el.style.width = `${Math.max(0, Math.min(100, porcentaje))}%`;
  el.style.background = `var(--${color})`;
}

function tagSalida(id, encendido, instalado = true) {
  const el = document.getElementById(id);
  if (!instalado) { el.className = "tag tag-plain"; el.textContent = t("no_instalado"); return; }
  el.className = encendido ? "tag tag-ok" : "tag tag-plain";
  el.textContent = t(encendido ? "encendido" : "apagado");
}

function pintarConexion(estado) {
  const tag = $("#tag-conexion");
  tag.className = estado.conectado ? "tag tag-ok" : "tag tag-off";
  tag.lastElementChild.textContent = t(estado.conectado ? "conectado" : "desconectado");
  $("#tag-demo").hidden = !estado.demo;
}

function pintarCiclo(d) {
  const info = ESTADOS[d.estado] || { color: "plain", desc: "sin_datos" };
  const tag = $("#estado-tag");
  tag.className = claseTag(info.color);
  tag.textContent = t(d.estado);

  const desc = $("#estado-desc");
  desc.dataset.i18n = info.desc;
  desc.textContent = t(info.desc);

  const total = (d.modo || 0) * 60;
  const enCiclo = d.restante !== undefined && d.restante >= 0 && total > 0;
  $("#cuenta").textContent = d.estado === "SECADO COMPLETO" ? "00:00" : fmtTiempo(enCiclo ? d.restante : -1);
  const avance = d.estado === "SECADO COMPLETO" ? 100 : enCiclo ? ((total - d.restante) / total) * 100 : 0;
  const barra = $("#progreso");
  barra.style.width = `${avance}%`;
  barra.style.background = info.emergencia ? "var(--off)" : d.estado === "SECADO COMPLETO" ? "var(--ok)" : "var(--accent)";

  const esFijo = window.LIMITES.modos.includes(d.modo);
  document.querySelectorAll("#modos .btn").forEach((b) => {
    b.setAttribute("aria-pressed", String(Number(b.dataset.modo) === d.modo));
  });
  $("#btn-personalizado").setAttribute("aria-pressed", String(!esFijo && Boolean(d.modo)));
  // Al abrir la página, si el horno ya tiene un tiempo personalizado, se muestra en el campo
  if (!campoPrellenado) {
    campoPrellenado = true;
    if (!esFijo && d.modo) $("#minutos-personalizado").value = d.modo;
  }

  const alerta = $("#alerta");
  if (info.emergencia || d.estado === "FALLA DHT22") {
    alerta.hidden = false;
    alerta.classList.toggle("alerta-warn", !info.emergencia);
    $("#alerta-texto").textContent = `${t(d.estado)}: ${t(info.emergencia ? "alerta_emergencia" : "alerta_falla")}`;
  } else {
    alerta.hidden = true;
  }
}

function pintarSensores(d) {
  const L = window.LIMITES;

  $("#v-temp").textContent = fmtNum(d.temp);
  medidor("m-temp", (d.temp / (L.temp_objetivo * 1.5)) * 100,
          d.temp >= L.temp_objetivo + 5 ? "warn" : "accent");

  $("#v-hum").textContent = fmtNum(d.hum);
  medidor("m-hum", d.hum, "accent");

  $("#v-ds").textContent = fmtNum(d.temp_ds);
  const pDs = (d.temp_ds / L.ds18b20_emergencia) * 100;
  medidor("m-ds", pDs, pDs >= 100 ? "off" : pDs >= 80 ? "warn" : "ok");
  $("#e-ds").textContent = d.estado === "SOBRETEMPERATURA" ? t("SOBRETEMPERATURA") : "";

  $("#v-gas").textContent = d.gas ?? "--";
  const pGas = (d.gas / L.gas_umbral) * 100;
  medidor("m-gas", pGas, pGas >= 100 ? "off" : pGas >= 80 ? "warn" : "ok");
  $("#e-gas").textContent = d.gas_calentando ? t("calentando") : d.estado === "ALERTA GAS" ? t("ALERTA GAS") : t("ok");

  $("#v-objeto").textContent = t(d.objeto ? "objeto_dentro" : "camara_vacia");
  $("#v-dist").textContent = d.dist === null || d.dist === undefined ? t("fuera_rango") : `${fmtNum(d.dist)} cm`;

  tagSalida("s-vent", d.ventilador);
  tagSalida("s-calef", d.calefactor, d.calefactor_instalado);
  $("#s-uptime").textContent = fmtTiempo(d.uptime);
}

function pintar() {
  if (!ultimo) return;
  pintarConexion(ultimo);
  if (ultimo.datos && ultimo.datos.estado) {
    pintarCiclo(ultimo.datos);
    pintarSensores(ultimo.datos);
  }
}

async function refrescar() {
  try {
    ultimo = await pedirJSON("/api/estado");
  } catch (e) {
    ultimo = { conectado: false, datos: ultimo ? ultimo.datos : {} };
  }
  pintar();
}

async function cambiarModo(minutos) {
  const msg = $("#modo-msg");
  const botones = document.querySelectorAll("#modos .btn, #btn-personalizado");
  botones.forEach((b) => { b.disabled = true; });
  try {
    await pedirJSON("/api/modo", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ minutos }),
    });
    msg.textContent = `${t("modo_enviado")}: ${minutos} ${t("min")}`;
  } catch (e) {
    msg.textContent = t("modo_error");
  } finally {
    botones.forEach((b) => { b.disabled = false; });
    setTimeout(refrescar, 600);
  }
}

document.addEventListener("DOMContentLoaded", () => {
  document.querySelectorAll("#modos .btn").forEach((b) => {
    b.addEventListener("click", () => {
      $("#minutos-personalizado").value = "";
      cambiarModo(Number(b.dataset.modo));
    });
  });

  // Tiempo personalizado: entero dentro del rango permitido
  $("#modo-personalizado").addEventListener("submit", (e) => {
    e.preventDefault();
    const { modo_min: min, modo_max: max } = window.LIMITES;
    const minutos = Number($("#minutos-personalizado").value);
    if (!Number.isInteger(minutos) || minutos < min || minutos > max) {
      $("#modo-msg").textContent = `${t("rango_invalido")} ${min} – ${max} ${t("min")}`;
      return;
    }
    cambiarModo(minutos);
  });
  refrescar();
  setInterval(refrescar, 2000);
});
document.addEventListener("idioma", pintar);
