// ========================================
// DIWALI LIGHT CONTROLLER - ESP32
// Non-Blocking Web Edition with Serial Monitor
// ========================================

#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include "time.h"

// ========================================
// WIFI CONFIGURATION
// ========================================
const char* ssid = "wh%pnk_cl1xv3";
const char* password = "Y0USHA11NOT(onnect";
const char* ntpServer = "pool.ntp.org";
const char* timeZone = "IST-5:30";

WebServer server(80);
struct tm timeinfo;
bool wifiConnected = false;
unsigned long lastWifiAttempt = 0;
const unsigned long wifiRetryInterval = 30000;

// ========================================
// PIN CONFIGURATION
// ========================================
const int pinkLEDPins[] = {13, 14, 26, 33, 5, 19, 15};
const int whiteLEDPins[] = {12, 27, 25, 32, 18, 21};
const int pnkledno = 7;
const int whtledno = 6;
const int ropeWhitePin = 2;
const int ropePinkPin = 4;

// ========================================
// SERIAL LOG BUFFER
// ========================================
#define LOG_BUFFER_SIZE 2000
char logBuffer[LOG_BUFFER_SIZE];
int logBufferPos = 0;

void addLog(String msg) {
  Serial.println(msg);
  int msgLen = msg.length();
  if (logBufferPos + msgLen + 2 > LOG_BUFFER_SIZE - 1) {
    // Shift buffer left to make room
    int shift = LOG_BUFFER_SIZE / 2;
    memmove(logBuffer, logBuffer + shift, LOG_BUFFER_SIZE - shift);
    logBufferPos -= shift;
  }
  msg.toCharArray(logBuffer + logBufferPos, msgLen + 1);
  logBufferPos += msgLen;
  logBuffer[logBufferPos++] = '\n';
  logBuffer[logBufferPos] = '\0';
}

// ========================================
// EEPROM CONFIGURATION
// ========================================
#define EEPROM_SIZE 256
#define EEPROM_MAGIC 0xDEAD

struct Settings {
  uint16_t magic;
  int startHour;
  int startMinute;
  int endHour;
  int endMinute;
  bool manualOverride;
  bool lightsOn;
  bool lightsEnabled;
  int currentPattern;
};

Settings settings;

const char* patternNames[] = {
  "Wave Chase", "Alternate Flash", "Mirror Converge", "Color Breathing",
  "Random Sparkle", "Triple Wave", "Rising Cascade", "Flash and Hold",
  "Random Blinking", "Pink White Flash", "Staggered Flicker", "Random Glow",
  "Symmetrical Pulse", "Inverse Pulse", "Double Chase", "Bounce Effect",
  "Snake Chase", "Festival Grand", "Heartbeat Pulse", "Border Highlight"
};

const int NUM_PATTERNS = 20;

// ========================================
// NON-BLOCKING PATTERN STATE
// ========================================
unsigned long patternStartTime = 0;
unsigned long lastStepTime = 0;
int patternStep = 0;
bool patternRunning = false;
const unsigned long PATTERN_DURATION = 15000; // 15 seconds per pattern

// ========================================
// DEFAULT SETTINGS
// ========================================
void loadDefaultSettings() {
  settings.magic = EEPROM_MAGIC;
  settings.startHour = 18;
  settings.startMinute = 0;
  settings.endHour = 1;
  settings.endMinute = 0;
  settings.manualOverride = false;
  settings.lightsOn = false;
  settings.lightsEnabled = true;
  settings.currentPattern = 0;
}

void saveSettings() {
  EEPROM.put(0, settings);
  EEPROM.commit();
  addLog("Settings saved to EEPROM");
}

void loadSettings() {
  EEPROM.get(0, settings);
  if (settings.magic != EEPROM_MAGIC) {
    addLog("No valid settings found, loading defaults");
    loadDefaultSettings();
    saveSettings();
  } else {
    addLog("Settings loaded from EEPROM");
  }
}

// ========================================
// UTILITY FUNCTIONS
// ========================================
void turnOffAllLEDs() {
  for (int i = 0; i < pnkledno; i++) digitalWrite(pinkLEDPins[i], HIGH);
  for (int i = 0; i < whtledno; i++) digitalWrite(whiteLEDPins[i], HIGH);
  digitalWrite(ropeWhitePin, LOW);
  digitalWrite(ropePinkPin, LOW);
}

void turnOnAllLEDs() {
  for (int i = 0; i < pnkledno; i++) digitalWrite(pinkLEDPins[i], LOW);
  for (int i = 0; i < whtledno; i++) digitalWrite(whiteLEDPins[i], LOW);
  digitalWrite(ropeWhitePin, HIGH);
  digitalWrite(ropePinkPin, HIGH);
}

// ========================================
// TIME FUNCTIONS
// ========================================
void printLocalTime() {
  if (!getLocalTime(&timeinfo)) {
    return;
  }
  char timeStr[64];
  strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);
  addLog(String(timeStr));
}

bool isLightShowTime() {
  if (!getLocalTime(&timeinfo)) {
    return false;
  }
  int currentHour = timeinfo.tm_hour;
  int currentMinute = timeinfo.tm_min;
  int currentTimeInMinutes = currentHour * 60 + currentMinute;
  int startTime = settings.startHour * 60 + settings.startMinute;
  int endTime = settings.endHour * 60 + settings.endMinute;
  return (currentTimeInMinutes >= startTime || currentTimeInMinutes <= endTime);
}

// ========================================
// WEB SERVER HANDLERS
// ========================================
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Diwali Lights</title>
<style>
* { margin: 0; padding: 0; box-sizing: border-box; }
body { font-family: Arial, sans-serif; background: #1a1a2e; color: #eee; padding: 20px; }
.container { max-width: 1000px; margin: 0 auto; }
h1 { text-align: center; color: #ff6b6b; margin-bottom: 20px; }
.section { background: #16213e; border-radius: 8px; padding: 20px; margin-bottom: 20px; }
.section h2 { color: #ffa500; margin-bottom: 15px; border-bottom: 2px solid #ffa500; padding-bottom: 10px; }
.status-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; margin-bottom: 20px; }
.status-box { background: #0f3460; padding: 15px; border-radius: 8px; text-align: center; }
.status-label { font-size: 0.9em; opacity: 0.7; margin-bottom: 5px; }
.status-value { font-size: 1.5em; font-weight: bold; color: #4ecca3; }
.btn { background: #ff6b6b; color: white; border: none; padding: 12px 25px; border-radius: 6px; cursor: pointer; font-size: 1em; margin: 5px; transition: all 0.3s; }
.btn:hover { background: #ff5252; transform: translateY(-2px); }
.btn-success { background: #4ecca3; }
.btn-success:hover { background: #45b393; }
.btn-danger { background: #e74c3c; }
.btn-danger:hover { background: #c0392b; }
.btn-primary { background: #3498db; }
.btn-primary:hover { background: #2980b9; }
.control-group { margin-bottom: 15px; }
.control-group label { display: block; margin-bottom: 5px; font-weight: bold; }
.control-group input, .control-group select { width: 100%; padding: 10px; border: none; border-radius: 6px; background: #0f3460; color: #eee; font-size: 1em; }
.grid-2 { display: grid; grid-template-columns: 1fr 1fr; gap: 15px; }
.log-box { background: #000; padding: 15px; border-radius: 6px; height: 300px; overflow-y: auto; font-family: monospace; font-size: 0.85em; line-height: 1.4; }
.log-box pre { margin: 0; white-space: pre-wrap; word-wrap: break-word; color: #0f0; }
.toggle { position: relative; display: inline-block; width: 60px; height: 30px; }
.toggle input { opacity: 0; width: 0; height: 0; }
.slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; transition: .4s; border-radius: 30px; }
.slider:before { position: absolute; content: ""; height: 22px; width: 22px; left: 4px; bottom: 4px; background-color: white; transition: .4s; border-radius: 50%; }
input:checked + .slider { background-color: #4ecca3; }
input:checked + .slider:before { transform: translateX(30px); }
@media (max-width: 768px) { .grid-2 { grid-template-columns: 1fr; } }
</style>
</head>
<body>
<div class="container">
<h1>🪔 Diwali Light Controller</h1>

<div class="status-grid">
<div class="status-box">
<div class="status-label">WiFi Status</div>
<div class="status-value" id="wifi">Connected</div>
</div>
<div class="status-box">
<div class="status-label">Mode</div>
<div class="status-value" id="mode">Auto</div>
</div>
<div class="status-box">
<div class="status-label">Lights</div>
<div class="status-value" id="lights">OFF</div>
</div>
<div class="status-box">
<div class="status-label">Pattern</div>
<div class="status-value" id="pattern">1</div>
</div>
</div>

<div class="section">
<h2>⚡ Quick Controls</h2>
<button class="btn btn-success" onclick="cmd('lightsOn')">All Lights ON</button>
<button class="btn btn-danger" onclick="cmd('lightsOff')">All Lights OFF</button>
<button class="btn btn-primary" onclick="cmd('toggleManual')">Toggle Manual Mode</button>
<button class="btn" onclick="cmd('toggleEnabled')">Toggle Lights Enabled</button>
<button class="btn btn-primary" onclick="cmd('save')">💾 Save Settings</button>
</div>

<div class="section">
<h2>⏰ Schedule Settings</h2>
<div class="grid-2">
<div class="control-group">
<label>Start Time</label>
<input type="time" id="startTime" value="18:00">
</div>
<div class="control-group">
<label>End Time</label>
<input type="time" id="endTime" value="01:00">
</div>
</div>
<button class="btn btn-success" onclick="saveSchedule()">Update Schedule</button>
</div>

<div class="section">
<h2>🎨 Pattern Control</h2>
<div class="control-group">
<label>Select Pattern</label>
<select id="patternSelect"></select>
</div>
<button class="btn btn-primary" onclick="jumpPattern()">Jump to Pattern</button>
<button class="btn btn-primary" onclick="cmd('nextPattern')">Next ▶</button>
<button class="btn btn-primary" onclick="cmd('prevPattern')">◀ Prev</button>
</div>

<div class="section">
<h2>📊 Serial Monitor</h2>
<div class="log-box" id="logBox"><pre id="logContent">Loading...</pre></div>
<button class="btn" onclick="clearLogs()">Clear Logs</button>
</div>

</div>

<script>
function updateStatus() {
fetch('/status').then(r => r.json()).then(d => {
document.getElementById('wifi').textContent = d.wifi ? 'Connected' : 'Offline';
document.getElementById('mode').textContent = d.manual ? 'Manual' : 'Auto';
document.getElementById('lights').textContent = d.enabled ? (d.on ? 'ON' : 'OFF') : 'DISABLED';
document.getElementById('pattern').textContent = (d.pattern + 1);
document.getElementById('startTime').value = pad(d.sh) + ':' + pad(d.sm);
document.getElementById('endTime').value = pad(d.eh) + ':' + pad(d.em);
});
}

function loadPatterns() {
fetch('/patterns').then(r => r.json()).then(d => {
let html = '';
d.forEach((p, i) => {
html += '<option value="' + i + '">' + (i+1) + '. ' + p + '</option>';
});
document.getElementById('patternSelect').innerHTML = html;
});
}

function updateLogs() {
fetch('/logs').then(r => r.text()).then(d => {
document.getElementById('logContent').textContent = d;
document.getElementById('logBox').scrollTop = document.getElementById('logBox').scrollHeight;
});
}

function cmd(action) {
fetch('/cmd?action=' + action).then(() => updateStatus());
}

function saveSchedule() {
let st = document.getElementById('startTime').value.split(':');
let et = document.getElementById('endTime').value.split(':');
fetch('/schedule?sh=' + st[0] + '&sm=' + st[1] + '&eh=' + et[0] + '&em=' + et[1])
.then(() => { alert('Schedule updated!'); updateStatus(); });
}

function jumpPattern() {
let idx = document.getElementById('patternSelect').value;
fetch('/cmd?action=jump&pattern=' + idx).then(() => updateStatus());
}

function clearLogs() {
fetch('/clearLogs').then(() => updateLogs());
}

function pad(n) { return n < 10 ? '0' + n : n; }

updateStatus();
loadPatterns();
updateLogs();
setInterval(updateStatus, 2000);
setInterval(updateLogs, 3000);
</script>
</body>
</html>
)rawliteral";
  
  server.send(200, "text/html", html);
}

void handleStatus() {
  String json = "{";
  json += "\"wifi\":" + String(wifiConnected ? "true" : "false") + ",";
  json += "\"manual\":" + String(settings.manualOverride ? "true" : "false") + ",";
  json += "\"on\":" + String(settings.lightsOn ? "true" : "false") + ",";
  json += "\"enabled\":" + String(settings.lightsEnabled ? "true" : "false") + ",";
  json += "\"pattern\":" + String(settings.currentPattern) + ",";
  json += "\"sh\":" + String(settings.startHour) + ",";
  json += "\"sm\":" + String(settings.startMinute) + ",";
  json += "\"eh\":" + String(settings.endHour) + ",";
  json += "\"em\":" + String(settings.endMinute);
  json += "}";
  server.send(200, "application/json", json);
}

void handlePatterns() {
  String json = "[";
  for (int i = 0; i < NUM_PATTERNS; i++) {
    if (i > 0) json += ",";
    json += "\"" + String(patternNames[i]) + "\"";
  }
  json += "]";
  server.send(200, "application/json", json);
}

void handleLogs() {
  server.send(200, "text/plain", String(logBuffer));
}

void handleClearLogs() {
  logBufferPos = 0;
  logBuffer[0] = '\0';
  addLog("Logs cleared");
  server.send(200, "text/plain", "OK");
}

void handleSchedule() {
  if (server.hasArg("sh") && server.hasArg("sm") && server.hasArg("eh") && server.hasArg("em")) {
    settings.startHour = server.arg("sh").toInt();
    settings.startMinute = server.arg("sm").toInt();
    settings.endHour = server.arg("eh").toInt();
    settings.endMinute = server.arg("em").toInt();
    saveSettings();
    addLog("Schedule updated: " + String(settings.startHour) + ":" + String(settings.startMinute) + " - " + String(settings.endHour) + ":" + String(settings.endMinute));
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing parameters");
  }
}

void handleCmd() {
  if (server.hasArg("action")) {
    String action = server.arg("action");
    
    if (action == "lightsOn") {
      settings.lightsOn = true;
      settings.manualOverride = true;
      turnOnAllLEDs();
      addLog("Manual ON");
    } else if (action == "lightsOff") {
      settings.lightsOn = false;
      settings.manualOverride = true;
      turnOffAllLEDs();
      addLog("Manual OFF");
    } else if (action == "toggleManual") {
      settings.manualOverride = !settings.manualOverride;
      addLog("Manual mode: " + String(settings.manualOverride ? "ON" : "OFF"));
    } else if (action == "toggleEnabled") {
      settings.lightsEnabled = !settings.lightsEnabled;
      if (!settings.lightsEnabled) turnOffAllLEDs();
      addLog("Lights enabled: " + String(settings.lightsEnabled ? "YES" : "NO"));
    } else if (action == "nextPattern") {
      settings.currentPattern = (settings.currentPattern + 1) % NUM_PATTERNS;
      patternStep = 0;
      patternStartTime = millis();
      addLog("Next pattern: " + String(patternNames[settings.currentPattern]));
    } else if (action == "prevPattern") {
      settings.currentPattern = (settings.currentPattern - 1 + NUM_PATTERNS) % NUM_PATTERNS;
      patternStep = 0;
      patternStartTime = millis();
      addLog("Previous pattern: " + String(patternNames[settings.currentPattern]));
    } else if (action == "jump" && server.hasArg("pattern")) {
      int idx = server.arg("pattern").toInt();
      if (idx >= 0 && idx < NUM_PATTERNS) {
        settings.currentPattern = idx;
        settings.manualOverride = true;
        patternStep = 0;
        patternStartTime = millis();
        addLog("Jumped to pattern: " + String(patternNames[idx]));
      }
    } else if (action == "save") {
      saveSettings();
    }
    
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing action");
  }
}

void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/patterns", handlePatterns);
  server.on("/logs", handleLogs);
  server.on("/clearLogs", handleClearLogs);
  server.on("/schedule", handleSchedule);
  server.on("/cmd", handleCmd);
  server.begin();
  addLog("Web server started");
}

// ========================================
// NON-BLOCKING PATTERN RUNNER
// ========================================
void runCurrentPattern() {
  unsigned long currentTime = millis();
  
  // Check if pattern duration expired
  if (currentTime - patternStartTime >= PATTERN_DURATION) {
    turnOffAllLEDs();
    settings.currentPattern = (settings.currentPattern + 1) % NUM_PATTERNS;
    patternStartTime = currentTime;
    patternStep = 0;
    addLog("Pattern: " + String(patternNames[settings.currentPattern]));
    delay(500); // Brief pause between patterns
    return;
  }
  
  // Run pattern step based on current pattern
  switch (settings.currentPattern) {
    case 0: pattern_waveChase(currentTime); break;
    case 1: pattern_alternateFlash(currentTime); break;
    case 2: pattern_mirrorConverge(currentTime); break;
    case 3: pattern_colorBreathing(currentTime); break;
    case 4: pattern_randomSparkle(currentTime); break;
    case 5: pattern_tripleWave(currentTime); break;
    case 6: pattern_risingCascade(currentTime); break;
    case 7: pattern_flashAndHold(currentTime); break;
    case 8: pattern_randomBlinking(currentTime); break;
    case 9: pattern_pinkWhiteFlash(currentTime); break;
    case 10: pattern_staggeredFlicker(currentTime); break;
    case 11: pattern_randomGlow(currentTime); break;
    case 12: pattern_symmetricalPulse(currentTime); break;
    case 13: pattern_inversePulse(currentTime); break;
    case 14: pattern_doubleChase(currentTime); break;
    case 15: pattern_bounceEffect(currentTime); break;
    case 16: pattern_snakeChase(currentTime); break;
    case 17: pattern_festivalGrand(currentTime); break;
    case 18: pattern_heartbeatPulse(currentTime); break;
    case 19: pattern_borderHighlight(currentTime); break;
  }
}

// ========================================
// SIMPLIFIED NON-BLOCKING PATTERNS
// ========================================
void pattern_waveChase(unsigned long t) {
  if (t - lastStepTime < 150) return;
  lastStepTime = t;
  
  int maxLeds = max(pnkledno, whtledno);
  int i = patternStep % (maxLeds * 2);
  
  turnOffAllLEDs();
  if (i < maxLeds) {
    if (i < pnkledno) digitalWrite(pinkLEDPins[i], LOW);
    if (i < whtledno) digitalWrite(whiteLEDPins[i], LOW);
    digitalWrite(ropeWhitePin, HIGH);
    digitalWrite(ropePinkPin, HIGH);
  } else {
    int rev = maxLeds * 2 - i - 1;
    if (rev < pnkledno) digitalWrite(pinkLEDPins[rev], LOW);
    if (rev < whtledno) digitalWrite(whiteLEDPins[rev], LOW);
    digitalWrite(ropeWhitePin, HIGH);
    digitalWrite(ropePinkPin, HIGH);
  }
  patternStep++;
}

void pattern_alternateFlash(unsigned long t) {
  if (t - lastStepTime < 800) return;
  lastStepTime = t;
  
  turnOffAllLEDs();
  if (patternStep % 2 == 0) {
    for (int i = 0; i < pnkledno; i++) digitalWrite(pinkLEDPins[i], LOW);
    digitalWrite(ropePinkPin, HIGH);
  } else {
    for (int i = 0; i < whtledno; i++) digitalWrite(whiteLEDPins[i], LOW);
    digitalWrite(ropeWhitePin, HIGH);
  }
  patternStep++;
}

void pattern_mirrorConverge(unsigned long t) {
  if (t - lastStepTime < 300) return;
  lastStepTime = t;
  
  int maxSteps = max(pnkledno / 2, whtledno / 2) + 1;
  int i = patternStep % (maxSteps * 2);
  
  turnOffAllLEDs();
  if (i < maxSteps) {
    if (i < pnkledno / 2 + 1) {
      digitalWrite(pinkLEDPins[i], LOW);
      if (pnkledno - 1 - i >= 0) digitalWrite(pinkLEDPins[pnkledno - 1 - i], LOW);
      digitalWrite(ropePinkPin, HIGH);
    }
    if (i < whtledno / 2 + 1) {
      digitalWrite(whiteLEDPins[i], LOW);
      if (whtledno - 1 - i >= 0) digitalWrite(whiteLEDPins[whtledno - 1 - i], LOW);
      digitalWrite(ropeWhitePin, HIGH);
    }
  }
  patternStep++;
}

void pattern_colorBreathing(unsigned long t) {
  if (t - lastStepTime < 100) return;
  lastStepTime = t;
  
  int maxLeds = max(pnkledno, whtledno);
  int cycle = patternStep % (maxLeds * 3);
  
  if (cycle < pnkledno) {
    digitalWrite(pinkLEDPins[cycle], LOW);
    digitalWrite(ropePinkPin, HIGH);
  } else if (cycle < pnkledno + whtledno) {
    int i = cycle - pnkledno;
    digitalWrite(whiteLEDPins[i], LOW);
    digitalWrite(ropeWhitePin, HIGH);
  } else {
    turnOffAllLEDs();
  }
  patternStep++;
}

void pattern_randomSparkle(unsigned long t) {
  if (t - lastStepTime < 200) return;
  lastStepTime = t;
  
  turnOffAllLEDs();
  digitalWrite(ropeWhitePin, HIGH);
  digitalWrite(ropePinkPin, HIGH);
  
  for (int i = 0; i < 3; i++) {
    if (random(2) == 0 && pnkledno > 0) {
      digitalWrite(pinkLEDPins[random(pnkledno)], LOW);
    } else if (whtledno > 0) {
      digitalWrite(whiteLEDPins[random(whtledno)], LOW);
    }
  }
  patternStep++;
}

void pattern_tripleWave(unsigned long t) {
  if (t - lastStepTime < 400) return;
  lastStepTime = t;
  
  turnOffAllLEDs();
  int wave = patternStep % 3;
  int start = wave * 2;
  int end = min(start + 2, max(pnkledno, whtledno));
  
  for (int i = start; i < end; i++) {
    if (i < pnkledno) digitalWrite(pinkLEDPins[i], LOW);
    if (i < whtledno) digitalWrite(whiteLEDPins[i], LOW);
  }
  digitalWrite(ropeWhitePin, HIGH);
  digitalWrite(ropePinkPin, HIGH);
  patternStep++;
}

void pattern_risingCascade(unsigned long t) {
  if (t - lastStepTime < 200) return;
  lastStepTime = t;
  
  int maxLeds = max(pnkledno, whtledno);
  int i = patternStep % (maxLeds + 5);
  
  if (i < maxLeds) {
    if (i < pnkledno) digitalWrite(pinkLEDPins[i], LOW);
    if (i < whtledno) digitalWrite(whiteLEDPins[i], LOW);
    digitalWrite(ropeWhitePin, HIGH);
    digitalWrite(ropePinkPin, HIGH);
  } else {
    turnOnAllLEDs();
  }
  patternStep++;
}

void pattern_flashAndHold(unsigned long t) {
  if (t - lastStepTime < 150) return;
  lastStepTime = t;
  
  int cycle = patternStep % 20;
  if (cycle < 10) {
    turnOffAllLEDs();
  } else {
    turnOnAllLEDs();
  }
  patternStep++;
}

void pattern_randomBlinking(unsigned long t) {
  if (t - lastStepTime < 200) return;
  lastStepTime = t;
  
  digitalWrite(ropeWhitePin, HIGH);
  digitalWrite(ropePinkPin, HIGH);
  
  if (patternStep % 2 == 0) {
    for (int i = 0; i < whtledno; i++) digitalWrite(whiteLEDPins[i], random(2) ? LOW : HIGH);
  } else {
    for (int i = 0; i < pnkledno; i++) digitalWrite(pinkLEDPins[i], random(2) ? LOW : HIGH);
  }
  patternStep++;
}

void pattern_pinkWhiteFlash(unsigned long t) {
  if (t - lastStepTime < 1000) return;
  lastStepTime = t;
  
  turnOffAllLEDs();
  if (patternStep % 3 == 0) {
    for (int i = 0; i < pnkledno; i++) digitalWrite(pinkLEDPins[i], LOW);
    digitalWrite(ropePinkPin, HIGH);
  } else if (patternStep % 3 == 1) {
    for (int i = 0; i < whtledno; i++) digitalWrite(whiteLEDPins[i], LOW);
    digitalWrite(ropeWhitePin, HIGH);
  } else {
    turnOnAllLEDs();
  }
  patternStep++;
}

void pattern_staggeredFlicker(unsigned long t) {
  if (t - lastStepTime < 100) return;
  lastStepTime = t;
  
  digitalWrite(ropeWhitePin, HIGH);
  digitalWrite(ropePinkPin, HIGH);
  
  for (int i = 0; i < whtledno; i++) {
    if (random(2)) digitalWrite(whiteLEDPins[i], random(2) ? LOW : HIGH);
  }
  for (int i = 0; i < pnkledno; i++) {
    if (random(2)) digitalWrite(pinkLEDPins[i], random(2) ? LOW : HIGH);
  }
  patternStep++;
}

void pattern_randomGlow(unsigned long t) {
  if (t - lastStepTime < 300) return;
  lastStepTime = t;
  
  if (patternStep < pnkledno + whtledno) {
    if (patternStep < pnkledno) {
      digitalWrite(pinkLEDPins[patternStep], LOW);
      digitalWrite(ropePinkPin, HIGH);
    } else {
      digitalWrite(whiteLEDPins[patternStep - pnkledno], LOW);
      digitalWrite(ropeWhitePin, HIGH);
    }
  } else {
    turnOnAllLEDs();
  }
  patternStep++;
  if (patternStep > pnkledno + whtledno + 3) patternStep = 0;
}

void pattern_symmetricalPulse(unsigned long t) {
  if (t - lastStepTime < 300) return;
  lastStepTime = t;
  
  int center = max(pnkledno, whtledno) / 2;
  int dist = patternStep % (center + 1);
  
  turnOffAllLEDs();
  if (dist < pnkledno / 2) {
    int c = pnkledno / 2;
    digitalWrite(pinkLEDPins[c - dist], LOW);
    if (c + dist < pnkledno) digitalWrite(pinkLEDPins[c + dist], LOW);
    digitalWrite(ropePinkPin, HIGH);
  }
  if (dist < whtledno / 2) {
    int c = whtledno / 2;
    digitalWrite(whiteLEDPins[c - dist], LOW);
    if (c + dist < whtledno) digitalWrite(whiteLEDPins[c + dist], LOW);
    digitalWrite(ropeWhitePin, HIGH);
  }
  patternStep++;
}

void pattern_inversePulse(unsigned long t) {
  if (t - lastStepTime < 300) return;
  lastStepTime = t;
  
  turnOnAllLEDs();
  int center = max(pnkledno, whtledno) / 2;
  int dist = patternStep % (center + 1);
  
  if (dist < pnkledno / 2) {
    int c = pnkledno / 2;
    digitalWrite(pinkLEDPins[c - dist], HIGH);
    if (c + dist < pnkledno) digitalWrite(pinkLEDPins[c + dist], HIGH);
  }
  if (dist < whtledno / 2) {
    int c = whtledno / 2;
    digitalWrite(whiteLEDPins[c - dist], HIGH);
    if (c + dist < whtledno) digitalWrite(whiteLEDPins[c + dist], HIGH);
  }
  patternStep++;
}

void pattern_doubleChase(unsigned long t) {
  if (t - lastStepTime < 200) return;
  lastStepTime = t;
  
  turnOffAllLEDs();
  int i = patternStep % (whtledno + pnkledno);
  
  if (i < whtledno) {
    digitalWrite(whiteLEDPins[i], LOW);
    digitalWrite(ropeWhitePin, HIGH);
  } else {
    int rev = pnkledno - 1 - (i - whtledno);
    if (rev >= 0 && rev < pnkledno) {
      digitalWrite(pinkLEDPins[rev], LOW);
      digitalWrite(ropePinkPin, HIGH);
    }
  }
  patternStep++;
}

void pattern_bounceEffect(unsigned long t) {
  if (t - lastStepTime < 150) return;
  lastStepTime = t;
  
  int maxLeds = max(pnkledno, whtledno);
  int cycle = patternStep % (maxLeds * 2);
  
  turnOffAllLEDs();
  int pos = (cycle < maxLeds) ? cycle : (maxLeds * 2 - cycle - 1);
  
  if (pos < pnkledno) digitalWrite(pinkLEDPins[pos], LOW);
  if (pos < whtledno) digitalWrite(whiteLEDPins[pos], LOW);
  digitalWrite(ropeWhitePin, HIGH);
  digitalWrite(ropePinkPin, HIGH);
  patternStep++;
}

void pattern_snakeChase(unsigned long t) {
  if (t - lastStepTime < 200) return;
  lastStepTime = t;
  
  int totalLights = pnkledno + whtledno;
  int pos = patternStep % totalLights;
  
  turnOffAllLEDs();
  digitalWrite(ropeWhitePin, HIGH);
  digitalWrite(ropePinkPin, HIGH);
  
  for (int i = 0; i < 3; i++) {
    int idx = (pos + i) % totalLights;
    if (idx < pnkledno) {
      digitalWrite(pinkLEDPins[idx], LOW);
    } else {
      digitalWrite(whiteLEDPins[idx - pnkledno], LOW);
    }
  }
  patternStep++;
}

void pattern_festivalGrand(unsigned long t) {
  if (t - lastStepTime < 500) return;
  lastStepTime = t;
  
  int phase = patternStep % 10;
  
  if (phase < 3) {
    if (phase % 2 == 0) turnOnAllLEDs();
    else turnOffAllLEDs();
  } else if (phase < 7) {
    int i = phase - 3;
    turnOffAllLEDs();
    if (i < pnkledno) digitalWrite(pinkLEDPins[i], LOW);
    if (i < whtledno) digitalWrite(whiteLEDPins[i], LOW);
  } else {
    turnOnAllLEDs();
    for (int i = 0; i < 2; i++) {
      int randIdx = random(pnkledno + whtledno);
      if (randIdx < pnkledno) digitalWrite(pinkLEDPins[randIdx], random(2) ? LOW : HIGH);
      else digitalWrite(whiteLEDPins[randIdx - pnkledno], random(2) ? LOW : HIGH);
    }
  }
  patternStep++;
}

void pattern_heartbeatPulse(unsigned long t) {
  if (t - lastStepTime < 200) return;
  lastStepTime = t;
  
  int beat = patternStep % 6;
  
  if (beat == 0 || beat == 2) {
    turnOnAllLEDs();
  } else if (beat == 1 || beat == 3) {
    turnOffAllLEDs();
  } else {
    digitalWrite(ropeWhitePin, HIGH);
    digitalWrite(ropePinkPin, HIGH);
  }
  patternStep++;
}

void pattern_borderHighlight(unsigned long t) {
  if (t - lastStepTime < 1000) return;
  lastStepTime = t;
  
  int phase = patternStep % 3;
  
  turnOffAllLEDs();
  if (phase == 0) {
    digitalWrite(pinkLEDPins[0], LOW);
    digitalWrite(pinkLEDPins[pnkledno - 1], LOW);
    digitalWrite(ropeWhitePin, HIGH);
    digitalWrite(ropePinkPin, HIGH);
  } else if (phase == 1) {
    turnOnAllLEDs();
  }
  patternStep++;
}

// ========================================
// SETUP
// ========================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  addLog("========================================");
  addLog("DIWALI LIGHT CONTROLLER - NON-BLOCKING");
  addLog("========================================");
  
  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);
  loadSettings();
  
  // Initialize GPIO
  for (int i = 0; i < pnkledno; i++) {
    pinMode(pinkLEDPins[i], OUTPUT);
    digitalWrite(pinkLEDPins[i], HIGH);
  }
  for (int i = 0; i < whtledno; i++) {
    pinMode(whiteLEDPins[i], OUTPUT);
    digitalWrite(whiteLEDPins[i], HIGH);
  }
  pinMode(ropeWhitePin, OUTPUT);
  pinMode(ropePinkPin, OUTPUT);
  digitalWrite(ropeWhitePin, LOW);
  digitalWrite(ropePinkPin, LOW);
  
  // WiFi connection
  addLog("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    addLog("WiFi Connected!");
    addLog("IP: " + WiFi.localIP().toString());
    
    configTzTime(timeZone, ntpServer);
    delay(2000);
    printLocalTime();
  } else {
    addLog("WiFi Failed - Starting AP Mode");
    WiFi.mode(WIFI_AP);
    WiFi.softAP("Diwali_Lights", "diwali2024");
    addLog("AP IP: " + WiFi.softAPIP().toString());
  }
  
  setupWebServer();
  addLog("Setup Complete!");
  addLog("Total Patterns: " + String(NUM_PATTERNS));
  addLog("========================================");
  
  patternStartTime = millis();
}

// ========================================
// MAIN LOOP (NON-BLOCKING)
// ========================================
void loop() {
  // Handle web server (non-blocking)
  server.handleClient();
  
  // WiFi reconnection attempt (non-blocking)
  if (!wifiConnected && millis() - lastWifiAttempt > wifiRetryInterval) {
    lastWifiAttempt = millis();
    if (WiFi.status() == WL_CONNECTED) {
      wifiConnected = true;
      addLog("WiFi Reconnected!");
    }
  }
  
  // Check if lights are disabled
  if (!settings.lightsEnabled) {
    turnOffAllLEDs();
    delay(100);
    return;
  }
  
  // Manual override mode
  if (settings.manualOverride) {
    if (settings.lightsOn) {
      runCurrentPattern();
    } else {
      turnOffAllLEDs();
      delay(100);
    }
    return;
  }
  
  // Auto mode - check schedule
  if (isLightShowTime() || !wifiConnected) {
    runCurrentPattern();
  } else {
    turnOffAllLEDs();
    delay(1000);
  }
}