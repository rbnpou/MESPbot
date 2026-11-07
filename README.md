# 🤖 MESPbot Meshtastic Bot (Modular & Captive Portal)

### Aviso: el proyecto todavía está muy en alpha. Muchas funciones aún no se han probado y he introducido algunos cambios que deberían funcionar. Lo hice público para tener un repositorio que pudiera levantar desde cualquier lugar y para forzarme a seguir con el desarrollo.

Firmware modular y autónomo para microcontroladores de la familia ESP32 diseñado para automatizar y controlar nodos **Meshtastic**. Incluye un servidor web integrado con **Portal Cautivo**, configuración persistente, sistema de respuestas automáticas con comandos personalizados y telemetría en tiempo real.

---

## 🚀 Características Principales

* **Arquitectura Modular:** Código estructurado limpiamente en componentes independientes (`config`, `storage`, `usb_host_mesh`, `bot_logic`, `web_manager`) para facilitar el mantenimiento y escalabilidad.
* **Soporte Dual de Hardware:** Compatible con conexión directa por USB Host o mediante comunicación serie física (UART / TX-RX).
* **Portal Cautivo Inalámbrico:** Al encenderse sin red configurada, levanta un punto de acceso Wi-Fi (`ESP32-MeshBot-Setup`) y redirige cualquier navegador automáticamente para configurar el dispositivo.
* **Almacenamiento Persistente:** Utiliza la memoria no volátil (NVS/Preferences) del ESP32 para guardar los ajustes sin perderlos al reiniciar.
* **Hasta 10 Comandos Personalizados:** Configura palabras o frases clave y sus respectivas respuestas directamente desde la interfaz web.
* **Telemetría Dinámica:** Inyecta variables del sistema en tiempo real dentro de las respuestas usando etiquetas como `{heap}`, `{uptime}`, `{name}` y `{power}`.

---

## 🔌 Guía de Interconexión y Compatibilidad de Hardware

Dependiendo del modelo de ESP32 que elijas utilizar como controlador y del nodo Meshtastic que poseas (Heltec T114, Faketec V3/V4, etc.), el método de conexión física y la configuración en el software varían:

| Modelo ESP32 Controlador | Tipo de Conexión Física | Hardware Compatible | Configuración en el Portal Web | Explicación y Cableado |
| :--- | :--- | :--- | :--- | :--- |
| **ESP32-S3 Mini** | **USB a USB** *(Recomendado para Heltec T114)* | Heltec T114 (USB Nativo) | *Heltec T114 (USB Nativo)* | El ESP32-S3 cuenta con soporte hardware USB Host nativo. Permite conectar un cable USB-C a USB-C directamente para alimentar el nodo y transmitir datos por CDC-ACM. |
| **ESP32-S3 Mini** | **Serial UART** | Faketec V3 / V4 / Otros | *Faketec V3 / V4 (UART Serial)* | Conexión por pines físicos. Une el `TX` del ESP32 al `RX` del nodo, el `RX` al `TX`, y comparte la tierra (`GND`). |
| **ESP32-C3 Mini** | **Serial UART** | Cualquier nodo Meshtastic | *Faketec V3 / V4 (UART Serial)* | El ESP32-C3 no posee USB Host nativo. Se interconecta estrictamente por cables físicos UART (ej. Pines GPIO 4 y 5 por defecto) configurando el puerto serie. |
| **ESP32-C5 Mini** | **Serial UART** (Wi-Fi 6 de Doble Banda) | Cualquier nodo Meshtastic | *Faketec V3 / V4 (UART Serial)* | Arquitectura RISC-V con conectividad de doble banda mejorada. Se comunica mediante interconexión física UART (`TX`, `RX`, `GND`) con el nodo de radio. |

---

## 🛠️ Estructura del Proyecto (Arduino IDE)

Organiza todos los archivos dentro de una única carpeta llamada `MeshBot/`:

```text
MeshBot/
├── MeshBot.ino        # Archivo orquestador principal (setup y loop)
├── config.h           # Estructuras globales y variables del sistema
├── storage.h          # Cabecera de persistencia NVS
├── storage.cpp        # Lógica de almacenamiento y recuperación
├── usb_host_mesh.h    # Cabecera de comunicación con nodos
├── usb_host_mesh.cpp  # Manejo dual USB Host / UART
├── bot_logic.h        # Cabecera del motor de reglas del bot
├── bot_logic.cpp      # Procesamiento de comandos y telemetría
├── web_manager.h      # Cabecera del servidor web asíncrono
└── web_manager.cpp    # Portal cautivo e interfaz web de configuración
```
# 📦 Dependencias y Librerías
Para compilar este proyecto en el IDE de Arduino, instala las siguientes librerías desde el Gestor de Librerías:

- `ESPAsyncWebServer` (de me-no-dev / ESPAsyncWebServer)
- `AsyncTCP` (requerida por el servidor web asíncrono en ESP32)

# 🎛 Detalles prácticos y dependencias físicas

### La librería del driver USB Host (`cdcacm_ftdi.h`):
- En el módulo `usb_host_mesh.cpp` incluimos `#include "cdcacm_ftdi.h"`. Asegúrate de tener instalada y disponible en tu entorno la librería oficial de soporte USB Host de Espressif (como `Espressif USB Host` o la implementación de CDC-ACM para ESP32-S3) si planeas usar el modo USB nativo con la Heltec T114. Si vas a usar exclusivamente los modelos C3/C5 o S3 por UART, esa inclusión de cabecera no se usará.

### Los Pines UART por defecto:
- En `usb_host_mesh.cpp` configuramos por defecto los pines GPIO 4 (RX) y GPIO 5 (TX) para la comunicación serie (UART1). Dependiendo del factor de forma exacto de tu placa (ESP32-S3 Mini, C3 SuperMini, etc.), asegúrate de que esos pines coincidan físicamente con los pines libres de tu placa y los del nodo Meshtastic.

# 🌐 Uso del Portal Cautivo
- Al encender el microcontrolador, busca redes Wi-Fi en tu teléfono o PC y conéctate a `ESP32-MeshBot-Setup`.
- El portal cautivo abrirá automáticamente la interfaz (si no abre, ingresa a `http://192.168.4.1`).
- Configura el nombre del bot, el tipo de hardware adecuado según la tabla anterior, el modo de energía y añade tus frases clave con sus respectivas respuestas utilizando etiquetas (ej: `Estado de red OK. Memoria libre: {heap}`).
- Haz clic en Guardar Configuración y Reiniciar. El bot se reiniciará aplicando los cambios de inmediato.

# 📡 Guía de Configuración del Nodo Meshtastic

## ⚙️ 1. Concepto de Funcionamiento

El **MeshBot** actúa como una interfaz inteligente externa conectada al puerto de comunicaciones serie o USB del nodo Meshtastic.

```text
[ Red LoRa / Usuarios ] 
          │ (Mensajes de texto)
          ▼
   [ Nodo Meshtastic ] ──(Puerto Serie / USB Host 115200 baudios)──► [ MESPbot ]
                                                                             │
   [ Nodo Meshtastic ] ◄──(Envía respuesta procesada por el Bot)──────────────┘
```

Para que esta interacción funcione en tiempo real sin interferir con otras funciones del nodo, debemos ajustar la interfaz de transmisión serie de Meshtastic.

---

## 🛠️ 2. Parámetros Clave en la App de Meshtastic

Abre la aplicación de **Meshtastic** (Android, iOS o cliente Web vía Bluetooth/USB) e ingresa a **Configuración del Dispositivo (Device Settings)**:

### Configuración del Puerto Serie (Serial Module)
Es el módulo principal para habilitar la comunicación entre el microcontrolador y el nodo.

1. Dirígete a **Configuración de Módulos (Module Configuration)** $\rightarrow$ **Serial**.
2. Ajusta los siguientes valores:
   * **Enabled:** `On` (Activado).
   * **Baud Rate:** `115200` (Debe coincidir exactamente con la velocidad del bot).
   * **Serial Mode:** `TEXTMSG` (Permite enviar y recibir texto plano por el puerto serie sin necesidad de procesar los buffer de Protobuf nativos).
   * **TX Pin / RX Pin:** *(Dejar por defecto en 0 si usas USB Host en Heltec T114. En Faketec, asegúrate de que corresponda a los pines TX/RX expuestos)*.

> **Importante:** Al seleccionar `TEXTMSG`, Meshtastic enviará directamente al puerto serie del ESP32 cualquier mensaje recibido en la red LoRa en formato de cadena de texto simple.

---

## 🔌 3. Verificación de la Conexión Física

Dependiendo del modelo que seleccionaste en el panel web del bot:

### Caso A: Heltec T114 (Modo USB Nativo)
1. Conecta un cable USB-C a USB-C entre el **ESP32-S3 Mini** (puerto OTG/Host) y el puerto USB de la **Heltec T114**.
2. En el portal cautivo web del bot (`http://192.168.4.1`), selecciona **Heltec T114 (USB Nativo)**.

### Caso B: Faketec V3/V4 u otros (Modo UART Serial)
1. Conecta los pines de comunicación serie cruzados:
   * **ESP32 TX (GPIO 5)** $\rightarrow$ **Nodo RX**
   * **ESP32 RX (GPIO 4)** $\rightarrow$ **Nodo TX**
   * **GND** $\rightarrow$ **GND**
2. En el portal cautivo web del bot, selecciona **Faketec V3 / V4 (UART Serial)**.

---

## 🧪 4. Prueba de Fuego y Diagnóstico

Una vez todo configurado y encendido:

1. **Prueba Básica (!ping):**
   * Envía un mensaje desde cualquier otro nodo de la red LoRa que diga exactamente: `!ping` o `ping`.
   * El nodo receptor pasará el mensaje al ESP32 por la interfaz serie.
   * El ESP32 generará la respuesta: `pong! [NombreDelBot] - Heap: XXX B` y la enviará de vuelta a la red.

2. **Prueba de Telemetría (!estado):**
   * Envía el mensaje: `!estado`.
   * Deberías recibir de vuelta: `SYS OK | Bot: MeshBot-S3 | Uptime: Xm Ys | Memoria: XXX B`.

---

## ❓ Solución de Problemas Frecuentes

* **El bot no responde pero recibe corriente:**
  * Verifica que en la configuración del Módulo Serial de Meshtastic el modo esté seleccionado en **`TEXTMSG`** y NO en `PROTOBUF`.
  * Asegúrate de que la velocidad de transmisión sea de `115200` baudios.
* **El bot responde en la consola serie pero el nodo no transmite el mensaje por radio:**
  * Comprueba que el rol del nodo en Meshtastic sea `CLIENT` y no `REPEATER`.
  * Asegúrate de que el canal en el que estás escribiendo no esté en modo "Mute" o de solo lectura.

# 📄 Licencia
Este proyecto se distribuye bajo la licencia MIT. Siéntete libre de adaptarlo y mejorarlo para tus redes mesh locales.
