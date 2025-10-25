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

This project transforms your Diwali decoration into a smart, automated light show using an ESP32 microcontroller. Control 13 jhalar lights (7 Pink + 6 White) plus 2 rope lights (Pink and White) through an intuitive web interface accessible from any device on your network.

Perfect for:
- 🏠 Home Diwali decorations
- 🎉 Festival celebrations
- 🎊 Party lighting
- 🏢 Commercial decorations
- 🎨 Creative lighting projects

---

## ✨ Features

### 🌐 Web Control Panel
- **Responsive web interface** - Control from phone, tablet, or PC
- **Real-time status updates** - Live monitoring of WiFi, mode, and current pattern
- **Beautiful gradient UI** - Modern, intuitive design

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
- **Next/Previous** - Navigate through patterns easily
- **Individual Pattern Control** - Enable/disable any pattern
- **Duration Adjustment** - Customize timing (5-60 seconds per pattern)

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
| Enclosure Box | 1 | For safe mounting |

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
   - Connect ESP32 GND to Relay GND
   - Connect ESP32 5V to Relay VCC
   - Connect GPIO pins to respective relay IN pins (as per pin mapping)

2. **Relay Module to Lights:**
   - Connect AC Live wire through relay COM and NO pins
   - Each light connects to one relay channel
   - All lights share common Neutral wire

3. **Power Supply:**
   - Power ESP32 with 5V USB or dedicated 5V regulator
   - Ensure proper AC wiring with circuit breaker

⚠️ **SAFETY WARNING:** Working with 220V AC is dangerous. If you're not experienced with electrical work, please hire a qualified electrician.

---

## 💻 Software Requirements

### Required Software
- [Arduino IDE](https://www.arduino.cc/en/software) (v1.8.x or v2.x)
- ESP32 Board Support Package

### Required Libraries
All libraries are included with ESP32 board package:
- WiFi.h
- WebServer.h
- EEPROM.h
- time.h

---

## 📥 Installation

### Step 1: Arduino IDE Setup

1. **Install Arduino IDE** from [arduino.cc](https://www.arduino.cc/en/software)

2. **Add ESP32 Board Support:**
   - Open Arduino IDE
   - Go to `File` → `Preferences`
   - In "Additional Board Manager URLs", add:
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to `Tools` → `Board` → `Boards Manager`
   - Search for "ESP32"
   - Install "ESP32 by Espressif Systems"

3. **Select Your Board:**
   - Go to `Tools` → `Board` → `ESP32 Arduino`
   - Select your ESP32 board (usually "ESP32 Dev Module")

### Step 2: Upload Code

1. **Download the Code:**
   - Clone or download this repository
   - Open `diwali_light_controller.ino` in Arduino IDE

2. **Configure WiFi Credentials:**
   ```cpp
   // Line 15-16 in the code
   const char* ssid = "Your_WiFi_SSID";
   const char* password = "Your_WiFi_Password";
   ```

3. **Connect ESP32:**
   - Connect ESP32 to computer via USB
   - Select correct COM port in `Tools` → `Port`

4. **Upload:**
   - Click the Upload button (→)
   - Wait for "Done uploading" message

5. **Open Serial Monitor:**
   - Go to `Tools` → `Serial Monitor`
   - Set baud rate to **115200**
   - Note the IP address displayed

---

## ⚙️ Configuration

### WiFi Configuration

Edit these lines in the code:

```cpp
const char* ssid = "JioFiber-4x4gk";        // Your WiFi name
const char* password = "oR8OlaiZ7zai8yei";  // Your WiFi password
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
2. Password: `diwali2024`
3. Open browser and go to: `http://192.168.4.1`

### Web Interface Screenshot


### Interface Sections

#### 1. Status Bar
- **WiFi Status:** Shows Connected/Offline
- **Mode:** Shows Auto/Manual
- **Current Pattern:** Shows which pattern is running

#### 2. Quick Controls
- **All Lights ON:** Turn all lights on instantly
- **All Lights OFF:** Turn all lights off instantly
- **Manual Override:** Toggle between Auto and Manual mode
- **Save Settings:** Save all changes to EEPROM

#### 3. Schedule Settings
- **Start Time:** Set when lights should turn on
- **End Time:** Set when lights should turn off
- Supports crossing midnight (e.g., 6 PM to 1 AM)

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
- Traditional Diwali feel

### Category: Smooth Transitions

**10. Color Breathing**
- Smooth pink to white transitions
- Breathing effect
- Relaxing and elegant

**11. Diya Wave**
- Lights accumulate like lighting diyas
- Gentle flickering when all lit
- Traditional and meaningful

**12. Color Tide**
- Waves crossing in opposite directions
- Complementary rope effects
- Fluid and dynamic

### Category: Random & Sparkle

**13. Random Sparkle**
- Twinkling lights effect
- Keeps rope on for base
- Magical and lively

**14. Random Glow**
- Lights turn on randomly one by one
- Builds to full brightness
- Anticipation and excitement

**15. Shimmer Curtain**
- All lights on with random shimmer gaps
- 96% ON time
- Elegant and sophisticated

### Category: Rhythmic Patterns

**16. Heartbeat Pulse**
- Double pulse rhythm
- Simulates heartbeat
- Unique and attention-grabbing

**17. Heartbeat of House**
- Gradual fade in/out
- Living, breathing effect
- Emotional and alive

**18. Festival March**
- Moving groups of 3 lights
- Drum beat flashes
- Marching band effect

### Category: Complex Sequences

**19. Festival Grand**
- Combination of multiple effects
- Flash → Wave → Sparkle sequence
- Showstopper pattern

**20. Rising Cascade**
- Lights fill up gradually
- Peak flash celebration
- Dramatic build-up

**21. Cascading Waterfall**
- Falling water effect
- Overlapping cascades
- Continuous flow

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

### Daily Operation

**Automatic Mode (Default):**
- Lights turn on automatically at scheduled time
- Runs through all enabled patterns in sequence
- Turns off automatically at end time
- No manual intervention needed

**Manual Mode:**
1. Click "Manual Override" button
2. Use "Jump to Pattern" to select specific pattern
3. Or use "All Lights ON/OFF" for simple control
4. Pattern stays until you change it
5. Turn off "Manual Override" to return to automatic

### Party Mode

For showing off to guests:
1. Enable "Manual Override"
2. Keep only favorite high-energy patterns enabled
3. Use Next/Previous buttons to demonstrate
4. Jump to specific patterns on request
5. Use "All Lights ON" for group photos

### Calm Evening Mode

For peaceful ambiance:
1. Disable intense patterns (Lightning, Strobe, Firecracker)
2. Keep only smooth patterns (Color Breathing, Shimmer, Diya Wave)
3. Increase durations to 25-30 seconds
4. Save settings for future use

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
3. Reduce WiFi interference (move away from microwave, etc.)
4. ESP32 auto-reconnects every 30 seconds
5. Lights continue working with saved settings during disconnection

### Issue: Some Patterns Don't Work

**Symptoms:** Specific patterns show no effect

**Solutions:**
1. Check if pattern is enabled in web interface
2. Verify all relay channels are working
3. Test individual lights with "All Lights ON"
4. Check for loose connections
5. Increase pattern duration for better visibility

### Issue: Settings Not Saving

**Symptoms:** Settings revert after restart

**Solutions:**
1. Always click "Save Settings" button after changes
2. Wait for "Settings saved!" confirmation
3. Don't power off immediately after saving
4. Check EEPROM initialization in Serial Monitor
5. Re-upload code if EEPROM is corrupted

### Issue: Web Page Loads Slowly

**Symptoms:** Interface takes time to respond

**Solutions:**
1. Improve WiFi signal strength
2. Reduce distance to ESP32
3. Close other tabs/apps using network
4. Restart ESP32
5. Clear browser cache

---

## 🎓 Advanced Customization

### Adding New Patterns

To create your own pattern:

```cpp
void patternX_myCustomPattern() {
  unsigned long startTime = millis();
  while (millis() - startTime < settings.patternDurations[X]) {
    if (settings.manualOverride) break;
    
    // Your custom light sequence here
    // Example:
    turnOnAllLEDs();
    delay(500);
    turnOffAllLEDs();
    delay(500);
  }
  turnOffAllLEDs();
}
```

Then add to pattern array:
```cpp
PatternFunction patterns[] = {
  // ... existing patterns ...
  patternX_myCustomPattern
};
```

### Changing Web UI Theme

Edit CSS in `handleRoot()` function:

```cpp
// Find this line and change colors:
background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);

// Change to any gradient you like:
background: linear-gradient(135deg, #f093fb 0%, #f5576c 100%); // Pink
background: linear-gradient(135deg, #4facfe 0%, #00f2fe 100%); // Blue
background: linear-gradient(135deg, #fa709a 0%, #fee140 100%); // Sunset
```

### Password Protection

Add authentication to web interface:

```cpp
// Add before server.begin() in setupWebServer():
server.on("/", []() {
  if (!server.authenticate("admin", "yourpassword")) {
    return server.requestAuthentication();
  }
  handleRoot();
});
```

### OTA (Over-The-Air) Updates

To enable wireless updates, add ArduinoOTA library:

```cpp
#include <ArduinoOTA.h>

// In setup():
ArduinoOTA.begin();

// In loop():
ArduinoOTA.handle();
```

---

## 📱 Mobile App

### Create Home Screen Shortcut

**Android (Chrome):**
1. Open web interface
2. Tap menu (⋮)
3. Select "Add to Home screen"
4. Name it "Diwali Lights"
5. Icon appears on home screen

**iOS (Safari):**
1. Open web interface
2. Tap Share button
3. Select "Add to Home Screen"
4. Name it "Diwali Lights"
5. Icon appears on home screen

Now you have a dedicated app icon!

---

## 🔐 Safety Guidelines

### Electrical Safety

⚠️ **WARNING: This project involves 220V AC mains voltage which can be lethal!**

**Safety Rules:**
1. ✅ Always disconnect power before working on circuits
2. ✅ Use proper insulated enclosure for relays and ESP32
3. ✅ Install circuit breaker/MCB for AC circuit
4. ✅ Use proper gauge wires for AC loads
5. ✅ Keep water away from all electrical components
6. ✅ Don't work on live circuits
7. ✅ If unsure, hire a qualified electrician

### Fire Safety

1. Don't overload relay channels
2. Check wire ratings match load
3. Ensure proper ventilation for relays
4. Don't leave system unattended for extended periods
5. Install in dry, protected location

### ESP32 Safety

1. Use proper 5V power supply (2A minimum)
2. Don't exceed GPIO voltage limits
3. Isolate relay control signals from AC mains
4. Use optocoupler-based relay modules

---

## 📊 Technical Specifications

### Performance Metrics

| Metric | Value |
|--------|-------|
| Max Patterns | 32 |
| Pattern Duration Range | 5-60 seconds |
| EEPROM Usage | 512 bytes |
| Web Server Port | 80 |
| WiFi Reconnect Interval | 30 seconds |
| Status Update Interval | 3 seconds |
| Max Relay Channels | 16 (14 used) |

### Memory Usage

```
Sketch uses approximately 800KB of program storage
Global variables use approximately 40KB of dynamic memory
EEPROM uses 512 bytes for persistent storage
```

### Supported ESP32 Variants

- ESP32-WROOM-32
- ESP32-DevKitC
- ESP32-WROVER
- NodeMCU-32S
- Any ESP32 board with WiFi

---

## 🤝 Contributing

Contributions are welcome! Here's how you can help:

### Ways to Contribute

1. **Report Bugs** - Open an issue with detailed description
2. **Suggest Features** - Share your ideas in issues
3. **Add Patterns** - Create and share new light patterns
4. **Improve Documentation** - Fix typos, add examples
5. **Submit Code** - Fork, modify, and create pull request

### Contribution Guidelines

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

### Code Style

- Use meaningful variable names
- Add comments for complex logic
- Follow existing code formatting
- Test thoroughly before submitting

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

### MIT License Summary

✅ Commercial use
✅ Modification
✅ Distribution
✅ Private use

⚠️ Liability and warranty disclaimers apply

---

## 🙏 Acknowledgments

- Thanks to the ESP32 community for excellent documentation
- Inspired by traditional Diwali celebrations
- Built with love for the festival of lights

---

## 📞 Support

### Getting Help

- 📖 Check [Troubleshooting](#troubleshooting) section first
- 🐛 Open an [Issue](https://github.com/yourusername/diwali-light-controller/issues) for bugs
- 💡 Start a [Discussion](https://github.com/yourusername/diwali-light-controller/discussions) for questions
- ⭐ Star this repo if you found it helpful!

### Contact

- **GitHub:** [@yourusername](https://github.com/yourusername)
- **Email:** your.email@example.com

---

## 🌟 Star History

If this project helped you, please consider giving it a ⭐!



---

## 🗺️ Roadmap

### Planned Features

- [ ] MQTT integration for Home Assistant
- [ ] Mobile app (iOS/Android)
- [ ] Sound-reactive mode with microphone
- [ ] Brightness control (PWM dimming)
- [ ] Pattern editor via web interface
- [ ] Multiple schedule profiles
- [ ] Cloud synchronization
- [ ] Sunrise/sunset auto-scheduling

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
