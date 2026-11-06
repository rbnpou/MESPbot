# 🤖 ESP32 Meshtastic Bot (Modular & Captive Portal)

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

