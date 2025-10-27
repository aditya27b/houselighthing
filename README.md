# 🪔 ESP32 Diwali Light Controller

> A feature-rich, web-controlled Diwali light automation system with 32 stunning patterns, scheduling, and EEPROM persistence.

![final lighting](https://github.com/user-attachments/assets/056f5526-1579-48cc-b0ab-44756c51c704)


---

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Circuit Diagram](#circuit-diagram)
- [Software Requirements](#software-requirements)
- [Installation](#installation)
- [Configuration](#configuration)
- [Web Interface](#web-interface)
- [Pattern Library](#pattern-library)
- [Usage](#usage)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)

---

## 🎯 Overview

This project transforms Diwali decoration into intelligent, automated light show using an ESP32 microcontroller. currently Controls 13 jhalar lights (7 Pink + 6 White {customisable}) plus 2 rope lights (Pink and White) through an interactive web interface accessible from any device on your local network.

Ideal for:
- 🏠 Home Diwali decorations
- 🎉 Festival celebrations
- 🎊 Party lighting
- 🏢 Commercial decorations
- 🎨 Creative lighting projects

---

## ✨ Features

### 🌐 Web Control Panel
- **Responsive web interface** - Control from any device.
- **Real-time status updates** - Live monitoring of WiFi, mode, and current pattern

### 🎭 32 Lighting Patterns
- **Wave Chase** - Smooth left-to-right chasing lights
- **Alternate Flash** - Bold color switching
- **Mirror Converge** - Lights meet in the middle
- **Color Breathing** - Smooth color transitions
- **Random Sparkle** - Twinkling star effect
- **Firecracker Burst** - Explosive bursts with falling sparks
- **Diya Wave** - Traditional diya lighting effect
- **Lightning Storm** - Random intense flashes
- **Shimmer Curtain** - Elegant shimmering effect
- **And 23 more stunning patterns!**

### ⚡ Smart Control Features
- **Manual Override** - Instant control over automatic scheduling
- **Pattern Jump** - Switch to any pattern instantly
- **Next/Previous** - Navigate patterns easily
- **Individual Pattern Control** - Enable/disable indiv pattern
- **Duration Adjustment** - Customize timing (5-60s per pattern)

### ⏰ Scheduling System
- **Automatic time-based control** - Set start and end times
- **Crossing midnight support** - Perfect for evening to late night shows
- **NTP time synchronization** - Accurate timekeeping via internet

### 💾 Persistent Storage
- **EEPROM saving** - All settings survive power loss
- **Instant restore** - Returns to last saved state on restart
- **No reconfiguration needed** - Set it once, works forever

### 📡 Network Features
- **WiFi connectivity** - Connect to your home network
- **Fallback AP mode** - Creates its own hotspot if WiFi fails
- **Auto-reconnect** - Keeps trying to reconnect every 30 seconds
- **Offline operation** - Works without WiFi using saved settings

ps:- some of the features have not yet been implimented.. they could be topic for further updates
---

## 🛠️ Hardware Requirements

### Components List

| Component | Quantity | Specification |
|-----------|----------|---------------|
| ESP32 Development Board | 1 | Any ESP32 variant |
| 16-Channel Relay Module | 1 | 5V or 12V, Active LOW |
| Pink Jhalar Lights | 7 | 220V AC |
| White Jhalar Lights | 6 | 220V AC |
| White Rope Light (WTOP) | 1 | 220V AC |
| Pink Rope Light (PTOP) | 1 | 220V AC |
| Power Supply | 1 | 5V 2A for ESP32 |
| Jumper Wires | As needed | Male-to-Female |

### Pin Mapping

**Pink Jhalar Lights (7):**
```
GPIO 13 → Pink 1
GPIO 14 → Pink 2
GPIO 26 → Pink 3
GPIO 33 → Pink 4
GPIO 5  → Pink 5
GPIO 19 → Pink 6
GPIO 15 → Pink 7
```

**White Jhalar Lights (6):**
```
GPIO 12 → White 1
GPIO 27 → White 2
GPIO 25 → White 3
GPIO 32 → White 4
GPIO 18 → White 5
GPIO 21 → White 6
```

**Rope Lights:**
```
GPIO 2  → White Rope Light (WTOP)
GPIO 4  → Pink Rope Light (PTOP)
```

### Hardware Setup Image

![Hardware Setup](https://github.com/user-attachments/assets/9bc7dd12-c53d-4bf7-82ef-3353c6190421)


---

## 📐 Circuit Diagram

![Circuit Diagram]<img width="1496" height="752" alt="Screenshot 2025-10-25 125716" src="https://github.com/user-attachments/assets/3b4e3742-4565-4c18-b2f9-6546a434b1dc" />

### Wiring Instructions

1. **ESP32 to Relay Module:**
   - Connect ESP32 GND -> Relay GND
   - Connect ESP32 5V -> Relay VCC
   - Connect GPIO pins -> respective relay IN pins (as per pin mapping)

2. **Relay Module to Lights:**
   - Connect AC Live wire through relay COM and NO(normally open) pins
   - Each indv light connects to one relay channel
   - All lights share common Neutral

3. **Power Supply:**
   - Power ESP32 with 5V USB or dedicated 5V regulator

⚠️ **WARNING:** Working with 220V AC is dangerous. If you're not experienced with AC voltage, please hire a qualified electrician.
---

## 💻 Software Requirements

### Required Software
- [Arduino IDE](https://www.arduino.cc/en/software)
- ESP32 Board package

### Required Libraries
 libraries are included with ESP32 board package:
- WiFi.h
- WebServer.h
- EEPROM.h
- time.h

---

## ⚙️ Configuration

### WiFi Configuration

Edit these lines in the code:

```cpp
const char* ssid = "";        //  WiFi name
const char* password = "";  //  WiFi password
```

### Default Settings

The system comes with these defaults:

| Setting | Default Value |
|---------|---------------|
| Start Time | 6:00 PM (18:00) |
| End Time | 1:00 AM (01:00) |
| Pattern Duration | 15-20 seconds |
| All Patterns | Enabled |
| Mode | Automatic |

### Changing Defaults

To change default schedule, edit `loadDefaultSettings()` function:

```cpp
settings.startHour = 18;    // 6:00 PM
settings.startMinute = 0;
settings.endHour = 1;       // 1:00 AM
settings.endMinute = 0;
```

### Fallback AP Settings

If WiFi connection fails, ESP32 creates its own hotspot:

| Setting | Value |
|---------|-------|
| SSID | Diwali_Lights_Setup |
| Password | diwali2024 |
| IP Address | 192.168.4.1 |

---

## 🌐 Web Interface

### Accessing the Web Interface

**If WiFi Connected:**
1. Check Serial Monitor for IP address (e.g., `192.168.1.100`)
2. Open browser and go to: `http://192.168.1.100`

**If WiFi Failed (AP Mode):**
1. Connect to WiFi hotspot "Diwali_Lights_Setup"
2. Password: `diwali2025`
3. Open browser and go to: `http://192.168.4.1`

### Web Interface Screenshot


### Interface Sections

#### 1. Status Bar
- **WiFi Status:**  Connected/Offline
- **Mode:**  Auto/Manual
- **Current Pattern:**  which pattern is running

#### 2. Quick Controls
- **All Lights ON:** Turn all lights on 
- **All Lights OFF:** Turn all lights off
- **Manual Override:** Toggle  Auto and Manual mode
- **Save Settings:** Save changes to EEPROM

#### 3. Schedule Settings
- **Start Time:** Set when lights  turn on
- **End Time:** Set when lights  turn off
- safe crossing midnight (e.g., 6 PM to 1 AM)

#### 4. Pattern Control
- **Jump to Pattern:** Dropdown to select any pattern
- **Next Pattern:** Skip to next enabled pattern
- **Previous Pattern:** Go back to previous pattern

#### 5. Pattern List
For each pattern:
- **Toggle Switch:** Enable/disable pattern
- **Duration Input:** Set duration (5-60 seconds)
- Changes save automatically

---

## 🎨 Pattern Library

### Category: Chasing Effects

**1. Wave Chase**
- Smooth left-to-right wave motion
- Ping-pong effect with rope flash
- Perfect for elegant flow

**2. Double Chase**
- Two waves moving in opposite directions
- Color-synchronized rope lights
- Dynamic and engaging

**3. Snake Chase**
- Moving segment of 3 lights
- Creates slithering effect
- Keeps rope lights on for base

### Category: Flash & Strobe

**4. Alternate Flash**
- Bold pink and white switching
- Strong color contrast
- High visual impact

**5. Firecracker Burst**
- Random explosive bursts
- Falling spark effect
- Perfect for celebrations

**6. Lightning Storm**
- Random intense flashes
- Thunder effect with all lights
- Dramatic and exciting

### Category: Symmetrical Patterns

**7. Mirror Converge**
- Lights meet in middle from both ends
- Rope pulses when converged
- Elegant symmetry

**8. Symmetrical Pulse**
- Center-out pulsing effect
- Breathing motion
- Calm and rhythmic

**9. Rangoli Bloom**
- Blooms from center like rangoli
- Breathing at full bloom

### Category: Smooth Transitions

**10. Color Breathing**
- Smooth pink to white transitions
- Breathing effect

**11. Diya Wave**
- Lights accumulate like lighting diyas
- Gentle flickering when all lit

**12. Color Tide**
- Waves crossing in opposite directions
- Complementary rope effects

### Category: Random & Sparkle

**13. Random Sparkle**
- Twinkling lights effect
- Keeps rope on for base

**14. Random Glow**
- Lights turn on randomly one by one
- Builds to full brightness

**15. Shimmer Curtain**
- All lights on with random shimmer gaps
- high ON time

### Category: Rhythmic Patterns

**16. Heartbeat Pulse**
- Double pulse rhythm
- Simulates heartbeat

**17. Heartbeat of House**
- Gradual fade in/out
- Living, breathing effect

**18. Festival March**
- Moving groups of 3 lights
- Drum beat flashes
- Marching & band effect

### Category: Complex Sequences

**19. Festival Grand**
- Combination of multiple effects
- Flash → Wave → Sparkle sequence

**20. Rising Cascade**
- Lights fill up gradually
- Peak flash celebration

**21. Cascading Waterfall**
- Falling water effect
- Overlapping cascades

---

## 📖 Usage

### First Time Setup

1. **Upload code** to ESP32
2. **Power on** the system
3. **Open Serial Monitor** to find IP address
4. **Access web interface** via browser
5. **Verify all lights work** using "All Lights ON"
6. **Test patterns** one by one
7. **Customize** durations and enabled patterns
8. **Save settings** using "Save Settings" button

---

## 🔧 Troubleshooting

### Issue: Can't Find Web Interface

**Symptoms:** Can't access web page after upload

**Solutions:**
1. Check Serial Monitor for IP address
2. Ensure phone/computer is on same WiFi network
3. Try pinging the IP address
4. If WiFi failed, look for "Diwali_Lights_Setup" hotspot
5. Connect to hotspot and try `http://192.168.4.1`

### Issue: Lights Not Turning On

**Symptoms:** Web interface works but lights don't respond

**Solutions:**
1. Check relay connections to ESP32
2. Verify AC power to lights
3. Test with "All Lights ON" button
4. Check if Manual Override is ON
5. Verify schedule times are correct
6. Ensure patterns are enabled

### Issue: WiFi Keeps Disconnecting

**Symptoms:** Web interface becomes inaccessible randomly

**Solutions:**
1. Move ESP32 closer to WiFi router
2. Check WiFi signal strength
3. check for WiFi interference 
4. ESP32 auto-reconnects every 30 seconds
5. Lights continue working with saved settings during disconnection

### Issue: Settings Not Saving

**Symptoms:** Settings revert after restart

**Solutions:**
1. Wait for "Settings saved!" confirmation
2. Don't power off immediately after saving
3. Check EEPROM initialization in Serial Monitor
4. Re-upload code if EEPROM is corrupted

### Issue: Web Page Loads Slowly

**Symptoms:** Interface takes time to respond

**Solutions:**
1. Improve WiFi signal strength
2. Reduce distance to ESP32
3. Close other tabs/apps using network
5. Clear browser cache

---

## 🎓 Advanced Customization

### Adding New Patterns

### Changing Web UI Theme

### Password Protection

### OTA (Over-The-Air) Updates

## 📱 Mobile App

## 🔐 Safety Guidelines

### Electrical Safety

⚠️ **WARNING: This project involves 220V AC mains voltage which can be lethal!**

**Safety Rules:**
1. ✅ Always disconnect power before working on circuits
2. ✅ Use proper insulated enclosure for relays and ESP32
3. ✅ Use proper gauge wires for AC loads

1. Don't overload relay channels
2. Install in dry, protected location

1. Use proper 5V power supply (2A minimum)
2. Use optocoupler-based relay modules

---

## 🤝 Contributing

Contributions are welcome!


## 📄 License

This project isnt licensed xD 

---

## 🌟 Star History

If this project helped you, please consider giving it a ⭐!

---

## 📝 Changelog

### Version 1.0.0 (2024)
- Initial release
- 32 lighting patterns
- Web interface
- EEPROM persistence
- WiFi fallback AP mode
- Manual override
- Schedule system

---

## 🎉 Happy Diwali!

May your celebrations be bright and joyful! 🪔✨

**Shubh Deepavali!** 

---

<p align="center">Made with ❤️ for the Festival of Lights</p>
<p align="center">⭐ Star this repo if you found it helpful!</p>
