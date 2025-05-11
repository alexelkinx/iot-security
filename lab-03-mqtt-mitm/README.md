# Lab 3: MQTT MITM and Packet Modification

## Overview

In **Lab 3**, I performed a **Man-in-the-Middle (MITM) attack** on MQTT traffic within a LAN and successfully modified the contents of MQTT packets in transit. The lab demonstrates advanced network-level attacks and traffic manipulation in IoT systems using tools like **Ettercap** and **Bettercap**.

## Key Features:

- Captures and analyzes normal MQTT traffic flow using tcpdump
- Performs MITM attack using ARP poisoning to discover MQTT clients
- Visualizes traffic flow with and without MITM using sequence diagrams
- Modifies live MQTT packet payloads using Bettercap and a custom JavaScript filter
- Built on a real IoT setup with Raspberry Pi, ESP32, Mosquitto broker, and Kali VM

## Task Description

The task description can be found in the [task-description.md](task-description.md) file.

## Solution

The lab includes:

- `normal_mqtt_flow.md`: Sequence diagram of the normal MQTT message path
- `mitm_mqtt_flow.md`: Modified diagram showing MITM interception and message routing
- `replace.js`: JavaScript filter used by Bettercap to modify MQTT payload content
- `mqtt_tcp/`: ESP-IDF project with MQTT publisher setup for use in the attack

## Tools Used

- **tcpdump**: For capturing MQTT packets at different points
- **Wireshark**: For analyzing MQTT message contents
- **Ettercap**: For initial ARP spoofing MITM setup
- **Bettercap**: For more advanced MITM with packet modification
- **ESP32**: Publishes MQTT messages over Wi-Fi
- **Kali Linux**: Used as the attacker machine

## How to Run

1. Set up the IoT network as described in the course materials (EdgeRouterX, Raspberry Pi with Mosquitto, ESP32 publisher, Kali VM).
2. Use the ESP32 to publish messages using the provided `mqtt_tcp/` project.
3. Perform ARP spoofing from Kali using Ettercap or Bettercap.
4. Analyze and modify traffic as instructed in the [task-description.md](task-description.md).
