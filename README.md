# ESP32 - Sistema de alarma y servidor local

Firmware para ESP32 desarrollado con ESP-IDF. El proyecto integra una alarma
de intrusión, indicadores LED, control remoto infrarrojo y un servidor HTTP
local para consultar el estado del sistema.

## Arquitectura y modularización

El firmware se organizó por responsabilidades para que la lógica de la
aplicación no quede mezclada con los detalles de GPIO, Wi-Fi, almacenamiento o
HTTP. `main.c` coordina los módulos; cada módulo ofrece una interfaz pública
en su archivo `.h` y mantiene su implementación en el `.c` correspondiente.
Así es posible cambiar o ampliar una parte (por ejemplo, el sensor) sin
trasladar su implementación al bucle principal.

```mermaid
flowchart TB
    subgraph Inputs["Sensores"]
        Intrusion["intrusion_sensor.c"]
        Remote["ir_remote.c"]
    end
    State["Estado global<br/>app_state.c / app_state.h"]
    Main["Coordinación y lógica<br/>main.c / alarm.c"]
    HTTP["HTTP Server<br/>http_server.c"]
    Dashboard["Dashboard<br/>HTML / CSS / JS<br/>(previsto)"]
    subgraph Outputs["Actuadores"]
        LEDs["led_status.c"]
    end
    WiFi["Conectividad<br/>wifi_manager.c"]
    Storage["Archivos estáticos<br/>spiffs_manager.c / SPIFFS"]

    Intrusion -->|Lecturas| Main
    Remote -->|Comandos| Main
    Main -->|Actualiza| State
    Main --> LEDs
    State -->|Estado JSON| HTTP
    HTTP --> Dashboard
    Dashboard -. "Comandos de control pendientes" .-> HTTP
    HTTP -. "Control por API pendiente" .-> Main
    HTTP -->|Sirve recursos| Storage
    WiFi -->|Al conectarse inicia| HTTP
```

El flujo principal sigue el esquema sensores → estado global → dashboard:
`main.c` procesa las entradas y mantiene el estado mediante `app_state`; el
servidor HTTP lo expone al dashboard. El flujo dashboard → actuadores representa
una extensión prevista: hoy no hay interfaz web en el repositorio y la API solo
implementa `GET /api/state`, sin recibir comandos para controlar la alarma o
los LEDs.

### Responsabilidades

| Módulo | Responsabilidad |
|---|---|
| `main.c` | Inicializa los módulos y coordina las entradas IR y del sensor con la alarma y sus LEDs. |
| `app_state` | Mantiene los datos compartidos de alarma, Wi-Fi y los campos previstos para sensores y actuadores. |
| `alarm` | Implementa las transiciones de estado de la alarma y su representación textual. |
| `ir_remote` | Captura cambios en GPIO 23, decodifica comandos NEC y los expone a la aplicación. |
| `intrusion_sensor` | Lee GPIO 36 y detecta el flanco configurado como intrusión. |
| `led_status` | Refleja los estados de alarma en los LEDs. |
| `wifi_manager` | Configura la conexión Wi-Fi STA y arranca el servidor cuando obtiene una IP. |
| `http_server` | Expone `GET /api/state` y atiende rutas de archivos estáticos desde SPIFFS. |
| `spiffs_manager` | Monta SPIFFS y consulta su capacidad y uso. |

En el flujo de control actual, `main.c` recibe los comandos IR y consulta el
sensor de intrusión; según esos eventos, coordina las transiciones de `alarm`
y actualiza `led_status`. `app_state` conserva el estado que consulta la API.
Cuando Wi-Fi obtiene una IP, `wifi_manager` inicia el servidor, que puede
responder con el estado JSON y servir los recursos presentes en SPIFFS.

## Avance actual

### Implementado

- Conexión Wi-Fi en modo estación (STA) y registro del estado de conexión.
- Máquina de estados de alarma: desactivada, activada y disparada.
- Control de alarma con el botón `2` de un control remoto IR NEC.
- Detección de flanco del sensor de intrusión mientras la alarma está activada.
- Indicadores LED según el estado de alarma:
  - Verde: desactivada.
  - Amarillo: activada.
  - Rojo: disparada.
- Montaje de SPIFFS.
- Servidor HTTP con `GET /api/state` para consultar el estado en JSON y una
  ruta para servir archivos estáticos desde SPIFFS.

### Pendiente de integrar o validar

- Agregar los archivos de la interfaz web a la imagen SPIFFS; actualmente no
  hay archivos web en el repositorio.
- Configurar y comprobar la partición SPIFFS en la tabla de particiones antes
  de probar su montaje.
- Implementar sensores de temperatura/humedad y control de iluminación,
  aire acondicionado y tomacorrientes. Esos campos aparecen en el JSON, pero
  todavía no tienen módulos que actualicen sus valores.
- Compilar y probar el firmware en una placa ESP32 y validar el hardware
  conectado.

## Hardware configurado

| Componente | GPIO |
|---|---:|
| Receptor IR | 23 |
| Sensor de intrusión | 36 |
| LED verde | 25 |
| LED rojo | 26 |
| LED amarillo | 27 |

Verifica el voltaje, el tipo de sensor y el cableado de tu placa antes de
conectar los componentes. El GPIO 36 es solo de entrada y no tiene resistencias
internas pull-up/pull-down.

## Configuración

1. Instala ESP-IDF y abre una terminal con su entorno configurado.
2. Configura `WIFI_SSID` y `WIFI_PASSWORD` en `main/wifi_manager.c` con los
   datos de tu red. No guardes credenciales reales en el repositorio.
3. Revisa la configuración de particiones para confirmar que incluye SPIFFS.

## Compilar y flashear

Desde la raíz del proyecto, en la terminal de ESP-IDF:

```shell
idf.py set-target esp32
idf.py build
idf.py -p <PUERTO> flash monitor
```

Reemplaza `<PUERTO>` por el puerto serie de la placa. Cuando el ESP32 se conecte
a Wi-Fi, el monitor serial mostrará la dirección IP. Desde un dispositivo en
la misma red se puede consultar:

```text
http://<IP-DEL-ESP32>/api/state
```

La raíz del servidor (`/`) intenta servir `/spiffs/index.html`; para usarla,
primero hay que agregar la interfaz web a SPIFFS.
