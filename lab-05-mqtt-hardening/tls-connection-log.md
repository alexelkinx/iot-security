## Log

```
I (23) boot: ESP-IDF v5.4-dirty 2nd stage bootloader
I (23) boot: compile time Apr 11 2025 16:17:44
I (24) boot: chip revision: v0.0
I (24) boot: efuse block revision: v0.2
I (27) boot.esp32c6: SPI Speed : 80MHz
I (31) boot.esp32c6: SPI Mode : DIO
I (34) boot.esp32c6: SPI Flash Size : 2MB
I (38) boot: Enabling RNG early entropy source...
I (43) boot: Partition Table:
I (45) boot: ## Label Usage Type ST Offset Length
I (52) boot: 0 nvs WiFi data 01 02 00009000 00006000
I (58) boot: 1 phy_init RF data 01 01 0000f000 00001000
I (65) boot: 2 factory factory app 00 00 00010000 00100000
I (71) boot: End of partition table
I (75) esp_image: segment 0: paddr=00010020 vaddr=420b0020 size=21fd0h (139216) map
I (108) esp_image: segment 1: paddr=00031ff8 vaddr=40800000 size=06020h ( 24608) load
I (114) esp_image: segment 2: paddr=00038020 vaddr=42000020 size=af614h (718356) map
I (247) esp_image: segment 3: paddr=000e763c vaddr=40806020 size=134b8h ( 79032) load
I (264) esp_image: segment 4: paddr=000faafc vaddr=408194e0 size=03b08h ( 15112) load
I (275) boot: Loaded app from partition at offset 0x10000
I (275) boot: Disabling RNG early entropy source...
I (285) cpu_start: Unicore app
I (293) cpu_start: Pro cpu start user code
I (294) cpu_start: cpu freq: 160000000 Hz
I (294) app_init: Application information:
I (294) app_init: Project name: mqtt_ssl_mutual_auth
I (299) app_init: App version: 2555357
I (302) app_init: Compile time: Apr 11 2025 16:17:37
I (308) app_init: ELF file SHA256: d405f7687...
I (312) app_init: ESP-IDF: v5.4-dirty
I (316) efuse_init: Min chip rev: v0.0
I (320) efuse_init: Max chip rev: v0.99
I (324) efuse_init: Chip rev: v0.0
I (328) heap_init: Initializing. RAM available for dynamic allocation:
I (334) heap_init: At 408229C0 len 00059C50 (359 KiB): RAM
I (339) heap_init: At 4087C610 len 00002F54 (11 KiB): RAM
I (344) heap_init: At 50000000 len 00003FE8 (15 KiB): RTCRAM
I (350) spi_flash: detected chip: generic
I (353) spi_flash: flash io: dio
W (356) spi_flash: Detected size(8192k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (369) sleep_gpio: Configure to isolate all GPIO pins in sleep state
I (375) sleep_gpio: Enable automatic switching of GPIO sleep configuration
I (381) coexist: coex firmware version: 49a8cdc
I (386) coexist: coexist rom version 5b8dcfa
I (390) main_task: Started on CPU0
I (390) main_task: Calling app_main()
I (390) system: Initializing NVS...
I (400) system: Initializing network interface...
I (410) system: Creating default event loop...
I (410) pp: pp rom version: 5b8dcfa
I (410) net80211: net80211 rom version: 5b8dcfa
I (420) wifi_init: rx ba win: 6
I (420) wifi_init: accept mbox: 6
I (420) wifi_init: tcpip mbox: 32
I (420) wifi_init: udp mbox: 6
I (420) wifi_init: tcp mbox: 6
I (430) wifi_init: tcp tx win: 5760
I (430) wifi_init: tcp rx win: 5760
I (430) wifi_init: tcp mss: 1440
I (440) wifi_init: WiFi IRAM OP enabled
I (440) wifi_init: WiFi RX IRAM OP enabled
I (440) wifi_init: WiFi SLP IRAM OP enabled
I (450) phy_init: phy_version 320,348a293,Sep 3 2024,16:33:12
I (500) wi-fi: Attempting to connect to Wi-Fi...
I (3990) wi-fi: Connected. IP: 192.168.99.71, Mask: 255.255.255.0, Gateway: 192.168.99.10
I (3990) mqtt_tls: Preparing to connect to MQTT broker...
I (5010) mqtt_tls: Connected to MQTT broker at URI: mqtts://192.168.1.13:8884
I (5010) mqtt_tls: Subscribed to topic: /topic/qos0
W (5010) mqtt_tls: Unhandled MQTT event ID: 3
I (6510) mqtt_tls: Data packet published: data0
I (6510) mqtt_tls: Received message. Topic: /topic/qos0, Data: data0
I (9510) mqtt_tls: Data packet published: data1
I (9520) mqtt_tls: Received message. Topic: /topic/qos0, Data: data1
I (12510) mqtt_tls: Data packet published: data2
I (12510) mqtt_tls: Received message. Topic: /topic/qos0, Data: data2
I (15510) mqtt_tls: Data packet published: data3
I (15510) mqtt_tls: Received message. Topic: /topic/qos0, Data: data3
I (18510) mqtt_tls: Data packet published: data4
I (18520) mqtt_tls: Received message. Topic: /topic/qos0, Data: data4
I (21510) mqtt_tls: Data packet published: data5
I (21520) mqtt_tls: Received message. Topic: /topic/qos0, Data: data5
I (24510) mqtt_tls: Data packet published: data6
I (24520) mqtt_tls: Received message. Topic: /topic/qos0, Data: data6
I (27510) mqtt_tls: Data packet published: data7
I (27510) mqtt_tls: Received message. Topic: /topic/qos0, Data: data7
I (30510) mqtt_tls: Data packet published: data8
I (30520) mqtt_tls: Received message. Topic: /topic/qos0, Data: data8
I (33510) mqtt_tls: Data packet published: data9
I (33520) mqtt_tls: Received message. Topic: /topic/qos0, Data: data9
I (36510) mqtt_tls: Data packet published: data0
I (36510) mqtt_tls: Received message. Topic: /topic/qos0, Data: data0
```
