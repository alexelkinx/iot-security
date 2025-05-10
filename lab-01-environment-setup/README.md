# Lab 1: Environment Setup

## Project Overview

In **Lab 1**, I configured the complete environment for testing IoT security scenarios. This includes setting up network infrastructure with EdgeRouter X, a Raspberry Pi running a full IoT Docker stack, a Kali Linux VM for security testing, and an ESP32 device publishing MQTT data. This setup provides the foundation for all future labs in the course.

## Key Features

- Configures EdgeRouter X for secure LAN and firewall rules
- Sets up Raspberry Pi as Wi-Fi access point and IoT stack host
- Deploys MQTT, InfluxDB, Grafana, and Node-RED in Docker containers
- Enables container isolation via macvlan network
- Prepares a Kali Linux VM for scanning and monitoring
- Connects ESP32 to publish test data to the broker

## Task Description

The task description can be found in the [task-description.md](task-description.md) file.

## Additional Files

- `firewall-rules.md`: Notes on firewall configuration
- `gvm-update-fix.md`: Instructions for fixing GVM update issues in Kali

## Solution

The ESP32 test project is located in the `mqtt_tcp/` directory:

- `main/app_main.c`: Modified example that publishes MQTT data in a loop
- `sdkconfig`: Configuration file for the ESP-IDF project
- `CMakeLists.txt`: Project build configuration

## How to Build and Run ESP32 Project

1. Set up the ESP-IDF environment:

   ```sh
   . $HOME/esp/esp-idf/export.sh
   ```

2. Build the project:

   ```sh
   cd mqtt_tcp
   idf.py build
   ```

3. Flash the firmware:

   ```sh
   idf.py flash
   ```

4. Monitor the serial output:

   ```sh
   idf.py monitor
   ```

## Network Testing

To verify that the ESP32 is successfully sending MQTT messages, you can subscribe to all topics from the Kali VM.

1. First, find the IP address of the MQTT broker (Mosquitto) running on your Raspberry Pi:

   ```sh
   docker exec -it mosquitto ip a
   ```

   Look for an IP address in the `192.168.1.x` range.

2. Then use the following command to subscribe to all topics:

   ```sh
   mosquitto_sub -h <broker_ip_address> -t "#" -v
   ```

   Replace `<broker_ip_address>` with the actual IP you found in step 1.

This will print all incoming MQTT messages from the ESP32 in real time.
