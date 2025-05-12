# 1. Overview

The lab setup consists of an isolated Wi-Fi network with a Raspberry Pi acting as an access point. An ESP32 device, the sole client in this Wi-Fi network, transmits MQTT messages to a broker hosted within Docker containers on the Raspberry Pi. Containers for MQTT broker, Grafana, InfluxDB, and Node-RED each have distinct IP addresses on an external LAN network.

## 1.1 Current Security Vulnerabilities and Mitigation Options:

| #   | Component/Aspect             | Vulnerability Description                            | Potential Impact                                      | Mitigation Options                                                            |
| --- | ---------------------------- | ---------------------------------------------------- | ----------------------------------------------------- | ----------------------------------------------------------------------------- |
| 1   | Wi-Fi Network Security       | WPA2 with weak password                              | Unauthorized Wi-Fi access, network eavesdropping      | Strong password policies, WPA3 upgrade, MAC filtering                         |
| 2   | MQTT Communication           | Plain text (unencrypted) MQTT messages               | Message interception, unauthorized data modification  | Use MQTT over TLS, encrypted payload                                          |
| 3   | Device Identity              | ESP32 lacks unique identity/authentication           | Device impersonation, unauthorized message injection  | Implement client authentication (MQTT username/password, client certificates) |
| 4   | External LAN Network         | Susceptible to ARP spoofing                          | Man-in-the-middle (MITM) attacks, network disruptions | Static ARP tables, ARP inspection, network segmentation                       |
| 5   | Broker and Docker Containers | Publicly exposed broker without additional hardening | Unauthorized access to MQTT broker, data leaks        | Implement broker access control, firewall rules, isolate containers           |
| 6   | Data Integrity               | MQTT data modified on-the-fly due to MITM attacks    | Compromised data integrity, incorrect data analytics  | End-to-end message encryption, integrity checks (e.g., digital signatures)    |

## 1.2 Next Steps for Lab Exercise

The lab activities will involve systematically addressing each identified vulnerability by applying recommended mitigations. Exercises will include:

- Strengthening Wi-Fi security.
- Securing MQTT communications with TLS, implementing proper device authentication, ensuring data integrity
- Applying network-level protections against ARP spoofing.
- Securing containerized infrastructure to limit exposure.

## 2. WiFi security

**Vulnerability**

The current setup uses WPA2 security protocol, that has the feature that the shared network key hash is sent over WiFi when a new connection is negotiated. A common attack is to send a fake termination packet forcing the device to renegotiate the connection again, so that network key hash can be captured.

Unfortunately, it is not easy to get Raspberry Pi working in the WPA3 security protocol that would be preferred. For ESP32 that doesn't seem to be a problem.

The plan B is to keep WPA2 protocol and change your network password to be hard enough so that it's not easily cracked:

| Requirement      | Recommendation                                                            |
| ---------------- | ------------------------------------------------------------------------- |
| **Length**       | At least **16 characters** (more is better)                               |
| **Characters**   | Mix of **uppercase**, **lowercase**, **numbers**, and **symbols**         |
| **Avoid**        | Common words, names, patterns (e.g., `12345678`, `qwerty`, `password123`) |
| **Entropy goal** | ~80–100 bits (roughly 15+ truly random characters)                        |

## 3. MQTT over TLS

The ESP32 code was based on the MQTT TCP example that doesn't have any encryption. More proper code would be based on the mqtt `ssl-mutual-auth` example.

### 3.1 Test run

Create an example project based on that SSL mutual auth example. Do the following steps

- Configure is the embedded LAB Wi-Fi SSID and password.
- Generate a key pair for your device. When working on Windows command prompt / PowerShell, the instructions given in the example project info don't work as they are. But if you open up a git bash prompt in your project folder, then you can use the OpenSSL tools. Follow the instructions given in the examples (i.e. on that ESP `Show Examples` page) to generate the client key and the certificate. Copy the contents of the generated files into the placeholders in your project, as instructed.
- When properly configured, the example code should have connection to the test.mosquitto.org and you should be able to test it having a connection from your mobile phone (MyMQTT app or similar) to the unencrypted endpoint in that server. Both endpoints are going to the same broker, so you can publish or subscribe unencrypted messages with phone, and you can still see the messages coming from the TLS secured connection.

### 3.2 Create demo configuration

Next step is to migrate this solution into your existing Raspberry Pi / ESP32 setup.

#### 3.2.1 Create your own self-signed certificates

Certificate creation has 3 steps:

1. As in the role of the system admin, create system root key and certificate -> ca.key, ca.crt
2. As in the role of measly developer, kindly request that your device gets a certificate == Create certificate signing request for a device (you need to do this for broker and ESP, we'll use terms "server" and "client" respectively).
3. As in the role of the system admin, create a device certificate for the requested device and sign it using system ca.key

```
jpp@raspberrypi:~ $ mkdir -p my-certs
jpp@raspberrypi:~ $ cd my-certs/
jpp@raspberrypi:~/my-certs $ openssl genrsa -out ca.key 2048
jpp@raspberrypi:~/my-certs $ openssl req -x509 -new -nodes -key ca.key -sha256 -days 3650 -out ca.crt -subj "/C=FI/ST=Varsinais-Suomi/L=Turku/O=IoTCA/OU=Dev/CN=IoTdemo"
jpp@raspberrypi:~/my-certs $ openssl genrsa -out server.key 2048
jpp@raspberrypi:~/my-certs $ openssl req -new -key server.key -out server.csr -subj "/C=FI/ST=Varsinais-Suomi/L=Turku/O=IoTServer/OU=MQTT/CN=192.168.1.13"
jpp@raspberrypi:~/my-certs $ openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 365 -sha256
Certificate request self-signature ok
subject=C = FI, ST = Varsinais-Suomi, L = Turku, O = IoTServer, OU = MQTT, CN = 192.168.1.13
jpp@raspberrypi:~/my-certs $ ls
ca.crt  ca.key  ca.srl  server.crt  server.csr  server.key
jpp@raspberrypi:~/my-certs $
```

**Repeat these steps to create client.key, client.csr and client.crt to be used for ESP32!!!**
As for step 1 above you dont want to create another ca.key and ca.crt, they exist already.
You need to replicate the server certificate request and certificate signing parts. In the certificate request part `"/C=FI/ST=Varsinais-Suomi/L=Turku/O=IoTServer/OU=MQTT/CN=192.168.1.13"` the CN (Common Name) can be edited to give your device a friendly name. For the server side it must be the IP address.
Replace parts referring to "server" with "client".

| File         | Description                                                                                                                      | Who Uses It                                                                 |
| ------------ | -------------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------- |
| `ca.crt`     | **CA certificate** — the public part of your Certificate Authority. Used to verify signatures of server and client certificates. | Required by **Mosquitto**, <br>Required by **clients (e.g., Raspberry Pi)** |
| `ca.key`     | **CA private key** — used to sign other certificates (e.g., server/client). Keep this **safe and secret**.                       | Used **only during certificate generation** (never distributed)             |
| `ca.srl`     | **Serial number file** — tracks the last serial number used by your CA when signing certs.                                       | Auto-generated by OpenSSL, not directly used by Mosquitto or clients        |
| `server.crt` | **Server certificate** — proves the identity of the broker. Signed by your CA.                                                   | Used by **Mosquitto**                                                       |
| `server.key` | **Server private key** — used to encrypt communication from the broker. Must match `server.crt`.                                 | Used by **Mosquitto**                                                       |
| `server.csr` | **Certificate Signing Request** — created during server cert generation. It contains the public key and identifying info.        | Used **only when creating `server.crt`**                                    |

You can check the validity of your certificates

```
openssl rsa -in client.key -check
openssl verify -CAfile ca.crt client.crt
openssl verify -CAfile ca.crt server.crt

These two below must match:
openssl x509 -in client.crt -noout -modulus | openssl md5
openssl rsa -in client.key -noout -modulus | openssl md5
If they don’t, the cert and key are not a valid pair.
```

Copy the certificates required by mosquitto broker into folder that will be mounted to broker container:

```
jpp@raspberrypi:~/my-certs $ sudo mkdir ~/IOTstack/volumes/mosquitto/certs
jpp@raspberrypi:~/my-certs $ sudo cp ca.crt ~/IOTstack/volumes/mosquitto/certs
jpp@raspberrypi:~/my-certs $ sudo cp server.crt ~/IOTstack/volumes/mosquitto/certs
jpp@raspberrypi:~/my-certs $ sudo cp server.key ~/IOTstack/volumes/mosquitto/certs
```

#### 3.2.2 Configure Mosquitto broker

Current configuration file is defined in `docker-compose.yml` file

```
jpp@raspberrypi:~/IOTstack $ cat docker-compose.yml
..
mosquitto:
    volumes:
    - ./volumes/mosquitto/config:/mosquitto/config
..
```

To have the certificates available for mosquitto container, add file mount for that in the same `volumes` section

```
    - ./volumes/mosquitto/certs:/mosquitto/certs
```

To have the new service port open, you need to replace port 1883 with 8884

```
    ports:
    - "8884:8884"
```

Next you modify the `~/IOTStack/volumes/mosquitto/config/mosquitto.conf` file:

Our running ESP32 example had the connection configuration to service defined as in `https://test.mosquitto.org/config/` and ESP32 example code was connecting to ` .broker.address.uri = "mqtts://test.mosquitto.org:8884",` so you need to set up a similar configuration for your broker. Add links to the certs that you just mounted to container:

```
listener 8884
protocol mqtt

cafile /mosquitto/certs/ca.crt
certfile /mosquitto/certs/server.crt
keyfile /mosquitto/certs/server.key

require_certificate true
use_identity_as_username true
tls_version tlsv1.2
```

Restart the container stack

```
jpp@raspberrypi:~/IOTstack $ docker-compose up -d
```

#### 3.2.3 Testing the broker with client certificates

From Raspberry pi, open two terminal sessions: one for subscriber, one for publisher:

```
jpp@raspberrypi:~/my-certs $ mosquitto_sub -h 192.168.1.13 -p 8884 -t "test/topic" --cafile ca.crt --cert client.crt --key client.key
```

```
jpp@raspberrypi:~/my-certs $ mosquitto_pub -h 192.168.1.13 -p 8884 -t "test/topic" -m "Hello mTLS" --cafile ca.crt --cert client.crt --key client.key
```

You should be able to connect and get data published.

#### 3.2.4 ESP32 configuration

Copy the client certificate contents to ESP32 project certificate files (you need to copy all three: ca.crt to be able to verify broker identity, and client.key and client.crt for client identity). Set the broker url to `mqtts://192.168.1.13:8884`

Test and enjoy.

## 4. Protection against ARP spoofing

The most efficient way would be to upgrade the network switch so that it supports DAI and DHCP spoofing. That solution is not in our budget...

To mitigate the problem, it is possible to set up static ARP addresses at Raspi and containers, so that they wouldn't listen to gratituous ARP messages.

\*\*In this lab setup we will skip that mitigation as well, because then it's very very difficult to change anything. It always requires manual entry to set up the new MAC addresses.

## 5. Additional access control

It would be possible to set up specific incoming traffic firewall rules for Raspberry Pi and the containers. It is also possible to set up an access control list for the MQTT broker so that the specific user only has access to specific topics. These steps are skipped for now.

## 6. To return

- ESP32 repo containing secure setup
- a log of successful messaging from ESP32 `tls-connection-log.md`
