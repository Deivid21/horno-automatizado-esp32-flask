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

// Historial: gráficos de temperatura, humedad y gas desde /api/historial.
let horas = 1;
let lecturas = [];

function color(nombre) {
  return getComputedStyle(document.documentElement).getPropertyValue(`--${nombre}`).trim();
}

function pintarGraficos() {
  const L = window.LIMITES;
  const x = lecturas.map((l) => l.ts);
  const vacio = t("sin_historial");

  dibujarGrafico($("#g-temp"), {
    x, unidad: "°", textoVacio: vacio,
    series: [
      { valores: lecturas.map((l) => l.temp), color: color("accent") },
      { valores: lecturas.map((l) => l.temp_ds), color: color("ds") },
    ],
    limites: [{ valor: L.temp_objetivo, color: color("warn") }],
  });

  dibujarGrafico($("#g-hum"), {
    x, unidad: "%", min: 0, max: 100, textoVacio: vacio,
    series: [{ valores: lecturas.map((l) => l.hum), color: color("ok") }],
  });

  dibujarGrafico($("#g-gas"), {
    x, min: 0, textoVacio: vacio,
    series: [{ valores: lecturas.map((l) => l.gas), color: color("accent") }],
    limites: [{ valor: L.gas_umbral, color: color("off") }],
  });
}

async function cargar() {
  try {
    const resp = await pedirJSON(`/api/historial?horas=${horas}`);
    lecturas = resp.lecturas;
  } catch (e) {
    lecturas = [];
  }
  pintarGraficos();
}

document.addEventListener("DOMContentLoaded", () => {
  document.querySelectorAll("#rangos .btn").forEach((b) => {
    b.addEventListener("click", () => {
      horas = Number(b.dataset.horas);
      document.querySelectorAll("#rangos .btn").forEach((o) => o.setAttribute("aria-pressed", String(o === b)));
      cargar();
    });
  });
  cargar();
  setInterval(cargar, 15000);
});
document.addEventListener("idioma", pintarGraficos);
document.addEventListener("tema", pintarGraficos);  // los colores salen de las variables CSS
window.addEventListener("resize", pintarGraficos);
