# Servidor Flask (Raspberry Pi)

Servidor web local que lee el estado del ESP32 por USB, lo guarda en SQLite y
lo muestra en una interfaz bilingüe (español / inglés) con el estilo de
[deivid21.github.io](https://deivid21.github.io).

## Páginas

| Ruta         | Contenido                                                       |
|--------------|-----------------------------------------------------------------|
| `/`          | Panel en vivo: estado, cuenta regresiva, sensores, salidas, modo de secado y tiempo personalizado |
| `/historial` | Gráficos de temperatura, humedad y gas (1 h, 6 h o 24 h)        |
| `/registro`  | Ciclos de secado y eventos de cambio de estado                  |

La interfaz no depende de librerías externas: funciona aunque la Raspberry Pi
no tenga internet (solo cambia la tipografía).

## Estructura

```
flask/
├── app.py              Rutas y API
├── config.py           Puerto serial, base de datos y límites
├── horno/
│   ├── conexion.py     Lectura del ESP32 por USB en segundo plano
│   ├── monitor.py      Último estado, eventos y ciclos de secado
│   ├── database.py     SQLite: lecturas, eventos y ciclos
│   └── simulador.py    Datos simulados para probar sin ESP32
├── templates/          Páginas HTML (Jinja)
├── static/             CSS, JS e ícono
└── horno.service       Servicio systemd
```

## Instalación en la Raspberry Pi

```bash
cd horno-automatizado-esp32-flask/flask
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```

Conecta el ESP32 por USB y revisa su puerto (normalmente `/dev/ttyUSB0` o
`/dev/ttyACM0`):

```bash
ls /dev/ttyUSB* /dev/ttyACM*
sudo usermod -aG dialout $USER   # permiso para el puerto serial; luego reinicia sesión
```

## Uso

```bash
python app.py                           # puerto por defecto /dev/ttyUSB0
HORNO_PUERTO=/dev/ttyACM0 python app.py # otro puerto
HORNO_DEMO=1 python app.py              # datos simulados, sin ESP32
```

Abre `http://<ip-de-la-raspberry>:5000` desde cualquier equipo de la misma red.
Al conectarse, el ESP32 se reinicia; es normal.

No dejes abierto el monitor serial del Arduino IDE al mismo tiempo: el puerto
solo puede usarlo un programa.

### Instalar como app en el celular

El panel incluye un manifest e íconos para instalarlo como aplicación
(pantalla completa, sin barra del navegador).

- **iPhone (Safari):** Compartir → *Agregar a pantalla de inicio*.
- **Android (Chrome):** menú ⋮ → *Agregar a la pantalla principal*.

Chrome solo la instala como app independiente en un contexto seguro (HTTPS o
`localhost`). Por la IP de la red local (`http://192.168.x.x:5000`) crea un
acceso directo que abre el panel dentro del navegador.

### Variables de entorno

| Variable           | Por defecto        | Descripción                     |
|--------------------|--------------------|---------------------------------|
| `HORNO_PUERTO`     | `/dev/ttyUSB0`     | Puerto serial del ESP32         |
| `HORNO_BAUDIOS`    | `115200`           | Velocidad serial                |
| `HORNO_DEMO`       | `0`                | `1` = datos simulados           |
| `HORNO_HOST`       | `0.0.0.0`          | Interfaz de red del servidor    |
| `HORNO_PUERTO_WEB` | `5000`             | Puerto web                      |
| `HORNO_DB`         | `datos/horno.db`   | Archivo de la base de datos     |

### Iniciar con la Raspberry Pi

```bash
sudo cp horno.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable --now horno
journalctl -u horno -f   # ver el registro
```

## API

| Método | Ruta                         | Descripción                                   |
|--------|------------------------------|-----------------------------------------------|
| GET    | `/api/estado`                | Último JSON del ESP32 y estado de la conexión |
| POST   | `/api/modo`                  | `{"minutos": 1–180}` cambia el tiempo de secado (modos fijos: 5, 15, 30, 45, 60) |
| GET    | `/api/historial?horas=1`     | Lecturas guardadas (1, 6 o 24 h)              |
| GET    | `/api/eventos?limite=50`     | Últimos cambios de estado                     |
| GET    | `/api/ciclos?limite=50`      | Últimos ciclos de secado                      |

Se guarda una lectura cada 10 s y se conservan 30 días; la limpieza corre una
vez al día. Al iniciar, los ciclos que quedaron sin terminar (por un reinicio o
un corte de luz) se marcan como interrumpidos.

Los umbrales, la temperatura objetivo y los modos de secado los envía el ESP32
(`config.h`), y `/api/estado` los incluye en `limites`. Los de
`LIMITES_POR_DEFECTO` en `config.py` solo se usan hasta que llegan.
