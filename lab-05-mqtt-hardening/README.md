# Lab 5: MQTT Hardening with TLS and Client Authentication

## Overview

In **Lab 5**, I secured MQTT communication between an ESP32 and a Mosquitto broker using **TLS** with **mutual authentication**. The lab demonstrates how to generate and apply self-signed certificates, configure Mosquitto for secure connections, and set up the ESP32 to authenticate using its own client certificate.

This setup prevents unauthorized access and ensures encrypted communication between IoT devices and the broker.

## Key Features

- Uses TLS (MQTTS) with port `8884`
- Employs **client certificate authentication** for the ESP32 device
- Uses **self-signed certificates** generated manually via OpenSSL
- Secures the Mosquitto broker running in a Docker container on a Raspberry Pi
- Demonstrates successful encrypted communication from ESP32 to broker

## Task Description

The task description is available in the [task-description.md](task-description.md) file.

## Solution

The solution consists of the following:

- `ssl_mutual_auth/`: ESP-IDF project with mutual TLS configuration
  - `main/app_main.c`: Code that connects to `mqtts://192.168.1.13:8884` using TLS
  - `main/ca.crt`, `main/client.crt`, `main/client.key`: Client credentials copied into the project
- `certs/`: Certificate generation files (both server and client certificates)
- `tls-connection-log.md`: Output confirming successful secure communication

## How to Build and Run

1. Set up the ESP-IDF environment:

   ```sh
   . $HOME/esp/esp-idf/export.sh
   ```

2. Build the project:

   ```sh
   cd lab-05-mqtt-hardening/ssl_mutual_auth
   idf.py build
   ```

3. Flash the firmware:

   ```sh
   idf.py flash
   ```

4. Monitor the output:

   ```sh
   idf.py monitor
   ```

## Testing the Secure Connection

- On the Raspberry Pi, Mosquitto is configured to listen on port `8884` with TLS and require client certificates.
- The ESP32 successfully connects using `client.crt` and `client.key`, verified by logs and confirmed in [tls-connection-log.md](tls-connection-log.md).

## Notes

- Certificates were generated manually using OpenSSL on the Raspberry Pi.
- Mosquitto container mounts `ca.crt`, `server.crt`, and `server.key` to enable TLS.
- The ESP32 connects to the broker over mqtts://<broker_ip>:8884, with require_certificate true enabled on the server side.
