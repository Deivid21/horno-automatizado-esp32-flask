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

// Registro: tablas de ciclos de secado y eventos de estado.
let ciclos = [];
let eventos = [];

const COLOR_RESULTADO = { completo: "ok", interrumpido: "warn" };
const COLOR_TIPO = { emergencia: "off", falla: "warn", completo: "ok", info: "plain" };

function pintarCiclos() {
  const cuerpo = $("#t-ciclos");
  $("#v-ciclos").hidden = ciclos.length > 0;
  cuerpo.innerHTML = ciclos.map((c) => {
    const resultado = c.resultado || "en_curso";
    const duracion = c.fin ? fmtTiempo(c.fin - c.inicio) : "--";
    return `<tr>
      <td>${fmtFecha(c.inicio)}</td>
      <td class="mono">${duracion}</td>
      <td>${c.modo} ${t("min")}</td>
      <td><span class="${claseTag(COLOR_RESULTADO[resultado])}">${escaparHTML(t(resultado))}</span></td>
      <td>${c.temp_prom === null ? "--" : `${fmtNum(c.temp_prom)} °C`}</td>
      <td>${c.hum_final === null ? "--" : `${fmtNum(c.hum_final)} %`}</td>
    </tr>`;
  }).join("");
}

function pintarEventos() {
  const cuerpo = $("#t-eventos");
  $("#v-eventos").hidden = eventos.length > 0;
  cuerpo.innerHTML = eventos.map((e) => `<tr>
      <td>${fmtFecha(e.ts)}</td>
      <td>${escaparHTML(t(e.anterior || "--"))} → <strong>${escaparHTML(t(e.nuevo))}</strong></td>
      <td><span class="${claseTag(COLOR_TIPO[e.tipo])}">${escaparHTML(t(e.tipo))}</span></td>
    </tr>`).join("");
}

function pintar() {
  pintarCiclos();
  pintarEventos();
}

async function cargar() {
  try {
    [ciclos, eventos] = await Promise.all([pedirJSON("/api/ciclos"), pedirJSON("/api/eventos")]);
  } catch (e) {
    /* conserva los datos anteriores */
  }
  pintar();
}

document.addEventListener("DOMContentLoaded", () => {
  repetir(cargar, 10000);
});
document.addEventListener("idioma", pintar);
