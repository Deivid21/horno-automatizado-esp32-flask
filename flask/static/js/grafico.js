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

// Gráfico de líneas en SVG, sin librerías externas: la Raspberry Pi puede
// estar sin internet y la página debe funcionar igual.
const SVG_NS = "http://www.w3.org/2000/svg";

function nodoSVG(tipo, atributos, padre) {
  const el = document.createElementNS(SVG_NS, tipo);
  Object.entries(atributos).forEach(([k, v]) => el.setAttribute(k, v));
  if (padre) padre.appendChild(el);
  return el;
}

/**
 * Dibuja un gráfico de líneas.
 * @param {SVGElement} svg
 * @param {Object} op  { x: [ts], series: [{valores, color}], limites: [{valor, color}],
 *                       unidad, min, max, textoVacio }
 */
function dibujarGrafico(svg, op) {
  svg.innerHTML = "";
  const ancho = svg.clientWidth || 800;
  const alto = svg.clientHeight || 240;
  svg.setAttribute("viewBox", `0 0 ${ancho} ${alto}`);

  const m = { arriba: 12, derecha: 12, abajo: 26, izquierda: 44 };
  const w = ancho - m.izquierda - m.derecha;
  const h = alto - m.arriba - m.abajo;

  const valores = op.series.flatMap((s) => s.valores).filter((v) => v !== null && v !== undefined);
  if (op.x.length < 2 || valores.length === 0) {
    const txt = nodoSVG("text", { x: ancho / 2, y: alto / 2, "text-anchor": "middle" }, svg);
    txt.textContent = op.textoVacio;
    return;
  }

  // Rango Y con margen, incluyendo las líneas límite
  const extras = (op.limites || []).map((l) => l.valor);
  let minY = op.min ?? Math.min(...valores, ...extras);
  let maxY = op.max ?? Math.max(...valores, ...extras);
  if (minY === maxY) { minY -= 1; maxY += 1; }
  const holgura = (maxY - minY) * 0.1;
  if (op.min === undefined) minY -= holgura;
  if (op.max === undefined) maxY += holgura;

  const x0 = op.x[0];
  const x1 = op.x[op.x.length - 1];
  const px = (ts) => m.izquierda + ((ts - x0) / (x1 - x0 || 1)) * w;
  const py = (v) => m.arriba + h - ((v - minY) / (maxY - minY)) * h;

  // Grilla y eje Y
  for (let i = 0; i <= 4; i++) {
    const v = minY + ((maxY - minY) * i) / 4;
    const y = py(v);
    nodoSVG("line", { x1: m.izquierda, x2: ancho - m.derecha, y1: y, y2: y, class: "eje", opacity: i === 0 ? 1 : 0.5 }, svg);
    const t = nodoSVG("text", { x: m.izquierda - 8, y: y + 4, "text-anchor": "end" }, svg);
    t.textContent = `${maxY - minY < 10 ? v.toFixed(1) : Math.round(v)}${op.unidad || ""}`;
  }

  // Eje X: 5 marcas de hora
  for (let i = 0; i <= 4; i++) {
    const ts = x0 + ((x1 - x0) * i) / 4;
    const anchor = i === 0 ? "start" : i === 4 ? "end" : "middle";
    const t = nodoSVG("text", { x: px(ts), y: alto - 6, "text-anchor": anchor }, svg);
    t.textContent = fmtHora(ts);
  }

  // Líneas límite (objetivo, umbral)
  (op.limites || []).forEach((l) => {
    const y = py(l.valor);
    nodoSVG("line", { x1: m.izquierda, x2: ancho - m.derecha, y1: y, y2: y, stroke: l.color, class: "limite" }, svg);
  });

  // Series: los null cortan la línea
  op.series.forEach((s) => {
    let d = "";
    let abierto = false;
    s.valores.forEach((v, i) => {
      if (v === null || v === undefined) { abierto = false; return; }
      d += `${abierto ? "L" : "M"}${px(op.x[i]).toFixed(1)},${py(v).toFixed(1)}`;
      abierto = true;
    });
    if (d) nodoSVG("path", { d, stroke: s.color, class: "linea" }, svg);
  });
}
