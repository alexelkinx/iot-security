# IoT Security

<p align="center">
  <img src="https://img.shields.io/badge/Platform-ESP32-blue" alt="ESP32"/>
  <img src="https://img.shields.io/badge/Framework-ESP--IDF-orange" alt="ESP-IDF"/>
  <img src="https://img.shields.io/badge/Build-CMake-informational" alt="CMake"/>
  <img src="https://img.shields.io/badge/Protocol-MQTT-yellow" alt="MQTT"/>
  <img src="https://img.shields.io/badge/Security-TLS%20%2F%20X.509-blue" alt="TLS/X.509"/>
  <img src="https://img.shields.io/badge/Attack-MITM--%26--Buffer%20Overflow-red" alt="MITM & Buffer Overflow"/>
  <img src="https://img.shields.io/badge/Scanner-OpenVAS-lightgrey" alt="OpenVAS"/>
  <img src="https://img.shields.io/badge/Tool-Wireshark-007ACC" alt="Wireshark"/>
  <img src="https://img.shields.io/badge/Tool-Ettercap-green" alt="Ettercap"/>
  <img src="https://img.shields.io/badge/Tool-Bettercap-purple" alt="Bettercap"/>
  <img src="https://img.shields.io/badge/Tool-Docker-blue" alt="Docker"/>
  <img src="https://img.shields.io/badge/Tool-Node--RED-red" alt="Node-RED"/>
  <img src="https://img.shields.io/badge/Monitoring-Grafana-orange" alt="Grafana"/>
  <img src="https://img.shields.io/badge/Database-InfluxDB-lightblue" alt="InfluxDB"/>
  <img src="https://img.shields.io/badge/Language-C%20%7C%20Python-blue" alt="C Python"/>
</p>

## Overview

This repository contains practical labs and configuration files for the **IoT Security** course. The course focuses on understanding and analyzing security aspects of embedded and IoT systems through hands-on exercises involving real hardware, network setups, and common security tools.

## Topics Covered

**Foundations**

- Ethical hacking principles and legal boundaries
- Secure network setup for IoT experimentation

**Infrastructure**

- EdgeRouter X configuration and firewall rules
- Raspberry Pi setup as a Wi-Fi access point and Docker host
- Full IoT stack deployment using MQTT, InfluxDB, Node-RED, and Grafana

**Vulnerabilities & Attacks**

- Vulnerability scanning with Kali Linux and OpenVAS
- Attacks on IoT protocols: MITM on MQTT
- Exploiting memory vulnerabilities: buffer overflow on embedded systems

**Defensive Techniques**

- Secure firmware development for ESP32 devices
- Securing MQTT communication with TLS and client authentication

## Labs

1. [Lab 1: Environment Setup](lab-01-environment-setup/README.md)  
   Set up the secure testing environment including EdgeRouter X, Raspberry Pi with Docker-based IoT stack, and ESP32 MQTT example.

2. [Lab 2: Network Scanning and Recon](lab-02-network-scanning-and-recon/README.md)  
   Capture ARP and TCP traffic, scan the local network with Nmap, and identify weaknesses using OpenVAS and other Kali Linux tools.

3. [Lab 3: MQTT MITM and Packet Modification](lab-03-mqtt-mitm/README.md)  
   Perform a Man-in-the-Middle (MITM) attack on MQTT traffic, analyze message flows, and modify published data using ettercap and bettercap tools.

4. [Lab 4: Buffer Overflow Attack](lab-04-buffer-overflow-attack/README.md)  
   Perform a buffer overflow attack against a vulnerable C program running on ESP32. Craft and inject a payload that overwrites the return address and activates hidden admin mode.

5. [Lab 5: MQTT Hardening](lab-05-mqtt-hardening/README.md)  
   Establish a secure TLS channel for MQTT communication with mutual certificate authentication. Test the connection with ESP32 and Mosquitto using manually generated certificates.

## Requirements

- **Raspberry Pi 4 (64-bit OS)** — hosts the IoT stack (MQTT, Node-RED, InfluxDB, Grafana)
- **ESP32-C6** — used to simulate embedded devices in attack and defense labs
- **EdgeRouter X** — provides network segmentation and firewalling
- **Kali Linux VM** — used as the attacker machine (Nmap, OpenVAS, Bettercap, etc.)

---

Each lab folder includes its own `README.md` and task description with detailed setup instructions and code.
