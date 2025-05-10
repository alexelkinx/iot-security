# IoT Security Lab 2b: System reconnaissance

Your goal is **collect all available information of the target system** (using Kali linux tools): network topology, host IP addresses, services and ports. Additionally search for weaknesses and vulnerabilities.

A **weakness** is a **flaw or deficiency** in a system, configuration, process, or design that **could** lead to a security issue if exploited.

- **Not always exploitable** on its own.
- **May or may not** lead to a vulnerability.
- **Can exist without active threats.**
- Examples of Weaknesses:
  - **Using weak encryption algorithms** (e.g., MD5, SHA-1).
  - **Poorly written code** (e.g., missing input validation).
  - **Allowing default credentials** (e.g., `admin/admin`).
  - **Running outdated software** (without known exploits yet).

A **vulnerability** is a **specific weakness** that has been **proven to be exploitable** by an attacker to compromise a system.

- A vulnerability **requires a weakness**.
- It **can be actively exploited**.
- Typically has a **CVE (Common Vulnerabilities and Exposures) ID**.
- Often **rated using CVSS (Common Vulnerability Scoring System)**.
- Examples of Vulnerabilities:
  - **CVE-2024-1001**: Buffer overflow in OpenSSH (can lead to remote code execution).
  - **SQL Injection**: Poor input validation allowing database manipulation.
  - **Weak MAC Algorithm in SSH**: Allows downgrade attacks (e.g., OpenVAS finding in your report).
  - **Unpatched OS vulnerabilities**: Like EternalBlue (CVE-2017-0144) used in WannaCry ransomware.

## 1. Setup

A) Need to do slight modifications to lab 1 IOTstack setup:

- The containers seem to get different IPs on each and restart. Need to change the config so that mosquitto broker (and other services) have always the same IP. This can be set in `docker-compose.yml`.
- Raspi host to containers traffic is routed in asymmetrical way: packets from host to containers are routed via erx router 192.168.1.1, but replies come directly from macvlan. That setup works OK, but for later steps we need to have all traffic routed symmetrically via erx **so that the setup resembles a normal LAN with MQTT services**. The solution is trick the containers so that they think host is in another network and only reachable via the router at 192.168.1.1. This can be done by defining `docker-compose.yml` macvlan network so that container IPs are close to router in smaller network 192.168.1.0/28 and raspi host is outside that network but within the actual network 192.168.1.0/24 (note the different network prefixes). In raspi add new path `sudo ip route add 192.168.1.0/28 via 192.168.1.1 dev eth0`

You find the updated version of `docker-compose.yml` in this repo. Replace the existing contents in raspi `/IOTstack/docker-compose.yml` and restart the containers

```
docker-compose down
docker-compose up -d
```

B) Change MQTT broker address in your ESP32 according to above `docker-compose.yml`. Consider changing the ESP32 code so that it sends variable MQTT data like in this snippet

```
    int count=0;
    while(1)
    {
        vTaskDelay(100);
        char buf[10];
        sprintf(buf, "data%d", count);
        count++;
        count = count%10;
        esp_mqtt_client_publish(client, "/topic/qos0", buf, 0, 0, 0);
    }
```

## 2. Network scanning

### 2.1. Procedure

Run the first network scan with default scan settings:

```
kali@kalivm:~ $ nmap 192.168.1.0/24
```

This will find you the list of detected hosts. Using the repo file `iotsec-network.plantuml` as template, document your findings as network diagram `reconnassaince-topology.plantuml` (and keep collecting data to that over the whole lab assignment). Use live plantuml server for editing.

As you see not all open ports are detected, because basic scan only checks 1000 most common port numbers. You want to take your time and make a more thorough scanning

```
kali@kalivm:~ $ nmap -p- -sV --script vuln 192.168.1.0/24
```

**Explanation of Options:**

- `-p-`: Scans all 65,535 TCP ports on the target hosts.​
- `-sV`: Enables service version detection to identify the specific services running on open ports.​
- `--script vuln`: Utilizes Nmap's scripting engine to run vulnerability detection scripts against the identified services.

### 2.2 Analysis

1. Update your system network document to show open ports at each IP address
2. Save the output of thorough nmap scan to repo to file `nmap-output.txt`
3. Feed the output of thorough nmap scan to AI and ask for analysis. Report three most promising weaknesses to this repo in md document `nmap-weakness-analysis.md`

## 3. Service scans

### 3.1 Procedure

Run openVAS scans. Check that you have the database feeds downloaded

![alt](<assets/20250313161338.png>)

Then in `Scans/Tasks` tab choose the magic wand icon `Task Wizard`. Just choose the basic test for all target devices in your system LAN

![alt](<assets/20250313161245.png>)

### 3.2 Analysis

For each report,

1. download it in csv format
2. save it to repo as openvas-report-ipaddress.csv
3. feed the csv content to AI for further explanation and analysis.
   Based on those, choose three most promising weaknesses and report those to this repo as md file: `openvas-weakness-analysis.md`
