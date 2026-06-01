#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

const char* ssid     = "3Wheel Remote Controller";
const char* password = "";

// Commands sent to robot over UART : w / s / a / d / x (stop)
// Telemetry received from robot    : $speed,roll,pitch,counter\n
String webpage = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'><title>3Wheel Remote</title><style>body{font-family:Arial,sans-serif;background:#1a1a2e;color:#eee;display:flex;flex-direction:column;align-items:center;padding:20px;gap:15px;}h1{font-size:1.4rem;color:#00d4ff;}#status{font-size:.85rem;padding:4px 14px;border-radius:20px;background:#333;}#status.on{background:#1a6b1a;color:#7fff7f;}.tele{display:grid;grid-template-columns:1fr 1fr;gap:10px;width:100%;max-width:300px;}.box{background:#16213e;border:1px solid #0f3460;border-radius:10px;padding:12px;text-align:center;}.lbl{font-size:.7rem;color:#888;text-transform:uppercase;letter-spacing:1px;}.val{font-size:1.6rem;font-weight:bold;color:#00d4ff;margin-top:4px;}.dpad{display:grid;grid-template-columns:repeat(3,80px);grid-template-rows:repeat(3,80px);gap:8px;}.btn{background:#16213e;border:2px solid #0f3460;border-radius:12px;color:#00d4ff;font-size:1.8rem;cursor:pointer;user-select:none;touch-action:none;display:flex;align-items:center;justify-content:center;}.btn.pressed{background:#0a4a8a;border-color:#00d4ff;}.blank{visibility:hidden;}.btn.stop{background:#3e1616;border-color:#8a0000;color:#ff4444;font-size:1.2rem;}.btn.stop.pressed{background:#8a0000;border-color:#ff4444;}</style></head><body><h1>3Wheel Remote Controller</h1><div id='status'>Disconnected</div><div class='tele'><div class='box'><div class='lbl'>Speed</div><div class='val' id='vs'>--</div></div><div class='box'><div class='lbl'>Roll</div><div class='val' id='vr'>--</div></div><div class='box'><div class='lbl'>Pitch</div><div class='val' id='vp'>--</div></div><div class='box'><div class='lbl'>Counter</div><div class='val' id='vc'>--</div></div></div><div class='dpad'><div class='blank'></div><div class='btn' id='btn_w'>&#9650;</div><div class='blank'></div><div class='btn' id='btn_a'>&#9664;</div><div class='btn stop' id='btn_stop'>&#9632;</div><div class='btn' id='btn_d'>&#9654;</div><div class='blank'></div><div class='btn' id='btn_s'>&#9660;</div><div class='blank'></div></div></body><script> var Socket; var st = document.getElementById('status'); function init() { Socket = new WebSocket('ws://' + window.location.hostname + ':81/'); Socket.onopen = function() { st.textContent = 'Connected'; st.className = 'on'; }; Socket.onclose = function() { st.textContent = 'Disconnected'; st.className = ''; setTimeout(init, 3000); }; Socket.onerror = function() { st.textContent = 'Error'; }; Socket.onmessage = function(event) { processCommand(event); }; } function processCommand(event) { var obj = JSON.parse(event.data); if (obj.type === 'telemetry') { document.getElementById('vs').innerHTML = parseFloat(obj.speed).toFixed(1); document.getElementById('vr').innerHTML = parseFloat(obj.roll).toFixed(1); document.getElementById('vp').innerHTML = parseFloat(obj.pitch).toFixed(1); document.getElementById('vc').innerHTML = parseFloat(obj.counter).toFixed(1); } } var stopEl = document.getElementById('btn_stop'); stopEl.addEventListener('mousedown', function() { press('btn_stop', stopEl); }); stopEl.addEventListener('touchstart', function(e) { e.preventDefault(); press('btn_stop', stopEl); }, { passive: false }); ['btn_w', 'btn_s', 'btn_a', 'btn_d'].forEach(function(id) { var el = document.getElementById(id); el.addEventListener('mousedown', function() { press(id, el); }); el.addEventListener('touchstart', function(e) { e.preventDefault(); press(id, el); }, { passive: false }); }); window.addEventListener('mouseup', release); window.addEventListener('touchend', release); function press(type, el) { el.classList.add('pressed'); Socket.send(JSON.stringify({ type: type, state: 'pressed' })); } function release() { document.querySelectorAll('.btn').forEach(function(b) { b.classList.remove('pressed'); }); Socket.send(JSON.stringify({ type: 'btn_stop', state: 'pressed' })); } window.onload = function() { init(); } </script></html>";

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(115200);

  WiFi.softAP(ssid, password);

  server.on("/", []() {
    server.send(200, "text/html", webpage);
  });
  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  server.handleClient();
  webSocket.loop();

  // Read telemetry lines pushed by the robot MCU
  static String incoming = "";
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      processTelemetry(incoming);
      incoming = "";
    } else if (c != '\r') {
      incoming += c;
    }
  }
}

// Parse "$speed,roll,pitch,counter" and broadcast JSON to all clients
void processTelemetry(String msg) {
  if (!msg.startsWith("$")) return;

  String data = msg.substring(1);
  int c1 = data.indexOf(',');
  int c2 = data.indexOf(',', c1 + 1);
  int c3 = data.indexOf(',', c2 + 1);
  if (c1 < 0 || c2 < 0 || c3 < 0) return;

  StaticJsonDocument<200> doc;
  JsonObject obj = doc.to<JsonObject>();
  obj["type"]  = "telemetry";
  obj["speed"] = data.substring(0, c1).toFloat();
  obj["roll"]  = data.substring(c1 + 1, c2).toFloat();
  obj["pitch"] = data.substring(c2 + 1, c3).toFloat();
  obj["counter"]   = data.substring(c3 + 1).toFloat();

  String json;
  serializeJson(doc, json);
  webSocket.broadcastTXT(json);
}

void webSocketEvent(byte num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      break;
    case WStype_CONNECTED:
      break;
    case WStype_TEXT: {
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, payload);
      if (error) return;

      const char* g_type = doc["type"];

      if      (strcmp(g_type, "btn_w")    == 0) Serial.print("w\n");
      else if (strcmp(g_type, "btn_s")    == 0) Serial.print("s\n");
      else if (strcmp(g_type, "btn_a")    == 0) Serial.print("a\n");
      else if (strcmp(g_type, "btn_d")    == 0) Serial.print("d\n");
      else if (strcmp(g_type, "btn_stop") == 0) Serial.print("x\n");

      digitalWrite(LED_BUILTIN, LOW);
      delay(50);
      digitalWrite(LED_BUILTIN, HIGH);
      break;
    }
  }
}
