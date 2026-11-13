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

// Tema: sistema (por defecto) → claro → oscuro, igual que deivid21.github.io.
// El tema guardado se aplica en el <head> de base.html para evitar el parpadeo.
(() => {
  const ORDEN = ["auto", "light", "dark"];
  const raiz = document.documentElement;
  const actual = () => raiz.dataset.theme || "auto";

  // Barras del navegador y del sistema (theme-color): se mantienen ambas etiquetas
  // y se fuerzan al tema elegido. Colores = superficie de la barra superior.
  const BARRAS = { dark: "#192123", light: "#e5f0f2" };
  function pintarBarras() {
    document.querySelectorAll('meta[name="theme-color"]').forEach((m) => {
      const propio = m.media.includes("dark") ? "dark" : "light";
      m.content = actual() === "auto" ? BARRAS[propio] : BARRAS[actual()];
    });
  }

  function avisar() {
    document.dispatchEvent(new CustomEvent("tema"));
  }

  function etiquetar(boton) {
    const texto = t(`theme_${actual()}`);
    boton.title = texto;
    boton.setAttribute("aria-label", texto);
  }

  document.addEventListener("DOMContentLoaded", () => {
    const boton = document.getElementById("theme-toggle");
    if (!boton) return;

    boton.addEventListener("click", () => {
      const siguiente = ORDEN[(ORDEN.indexOf(actual()) + 1) % ORDEN.length];
      if (siguiente === "auto") delete raiz.dataset.theme;
      else raiz.dataset.theme = siguiente;
      try {
        if (siguiente === "auto") localStorage.removeItem("theme");
        else localStorage.setItem("theme", siguiente);
      } catch (e) { /* almacenamiento no disponible */ }
      etiquetar(boton);
      pintarBarras();
      avisar();
    });

    document.addEventListener("idioma", () => etiquetar(boton));
    etiquetar(boton);
    pintarBarras();
  });

  // En modo sistema, el tema cambia si el sistema cambia
  window.matchMedia("(prefers-color-scheme: light)").addEventListener("change", () => {
    if (actual() === "auto") avisar();
  });
})();
