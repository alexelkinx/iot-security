### Kali Linux

```
┌──(alex㉿kali)-[~]
└─$ arp -a
? (172.16.132.2) at 00:50:56:f7:fc:53 [ether] on eth0
? (172.16.132.254) at 00:50:56:f7:f8:a8 [ether] on eth0
```

### Raspberry Pi

```
alex@respi-xyz:~ $ docker exec -it mosquitto arp -a
influxdb.iotstack_macvlan_net (192.168.1.131) at 7a:67:5b:28:57:4a [ether] on eth0
? (192.168.1.43) at de:60:ad:e5:c2:59 [ether] on eth0
? (192.168.1.41) at a0:ce:c8:b5:bd:70 [ether] on eth0
? (192.168.1.1) at 70:a7:41:de:33:6b [ether] on eth0
```
