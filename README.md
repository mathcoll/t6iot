# t6 iot
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/5b1eb9f55e6e462b86eb731fc8b29489)](https://www.codacy.com/app/internetcollaboratif/t6iot?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=mathcoll/t6iot&amp;utm_campaign=Badge_Grade)

##  t6 iot Arduino library
This library aims to connect Arduino Sensors to t6 framework Saas or On Premise.

## Compatibility
t6iot library works on both ESP32 & ESP8266 devboards architectures with some limitations.
Ths following board have been tested:
- ESP32 v2.0.11/WROOM-DA module / 
- ESP32 v2.0.11/WEMOS D1 R32
- ESP8266 v3.1.2/Lolin(WEMOS) D1 Mini Pro
- ESP8266 v3.1.2/NodeMCU 1.0 (ESP-12E) Module

| Arch | Board | Tests | IDE Config |
| ------ | ------ | ------ | ------ |
|ESP8266 v3.1.2|NodeMCU 1.0 (ESP-12E) Module|✔http ✔https ✔createDatapoint ✔Ssdp ✔Mdns ✔Websockets ❌Audio ❌OTA|Do not activate all features together as is can turn to oom|
|ESP8266 v3.1.2|Lolin(WEMOS) D1 Mini Pro|✔http ✔https ✔createDatapoint ⁉️Ssdp ⁉️Mdns ⁉️Websockets ⁉️Audio ❌OTA||
|ESP32 v2.0.11|WEMOS D1 R32|❌http ✔https ✔createDatapoint ⁉️Ssdp ⁉️Mdns ⁉️Websockets ⁉️Audio ❌OTA||
|ESP32 v2.0.11|WROOM-DA module|❌http ✔https ✔createDatapoint ✔Ssdp ✔Mdns ✔Websockets ⁉️Audio ❌OTA|Minimal SPIFFS 1.9MB APP with OTA/190KB SPIFFS|
