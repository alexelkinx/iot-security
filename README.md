# IoT Security

## Overview

This repository contains practical labs and configuration files for the **IoT Security** course. The course focuses on understanding and analyzing security aspects of embedded and IoT systems through hands-on exercises involving real hardware, network setups, and common security tools.

## Topics Covered

- Ethical hacking principles and legal boundaries
- Secure network setup for IoT experimentation
- EdgeRouter X configuration and firewall rules
- Raspberry Pi setup as a Wi-Fi access point and Docker host
- Full IoT stack deployment using MQTT, InfluxDB, Node-RED, and Grafana
- Vulnerability scanning with Kali Linux and OpenVAS
- Secure firmware development for ESP32 devices

## Labs

1. [Lab 1: Environment Setup](lab-01-environment-setup/README.md)  
   Set up the secure testing environment including EdgeRouter X, Raspberry Pi with Docker-based IoT stack, and ESP32 MQTT example.

2. [Lab 2: Network Scanning and Recon](lab-02-network-scanning-and-recon/README.md)  
   Capture ARP and TCP traffic, scan the local network with Nmap, and identify weaknesses using OpenVAS and other Kali Linux tools.

3. [Lab 3: MQTT MITM and Packet Modification](lab-03-mqtt-mitm/README.md)  
   Perform a Man-in-the-Middle (MITM) attack on MQTT traffic, analyze message flows, and modify published data using ettercap and bettercap tools.

## Requirements

- Raspberry Pi 4 (with 64-bit OS)
- ESP32-C6 or similar development board
- EdgeRouter X
- Kali Linux VM (VirtualBox or VMware)

---

Each lab folder includes its own `README.md` and task description with detailed setup instructions and code.
