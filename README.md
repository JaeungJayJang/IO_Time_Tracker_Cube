# 🎲 IoT Time Tracker Cube
### *Enhanced Implementation of Adafruit's Time Tracking Cube Tutorial*

[![ESP32](https://img.shields.io/badge/ESP32-Feather-blue?style=flat&logo=espressif)](https://www.adafruit.com/product/2821)
[![Adafruit IO](https://img.shields.io/badge/Adafruit%20IO-Connected-green?style=flat)](https://io.adafruit.com/)
[![C++](https://img.shields.io/badge/C++-Arduino-blue?style=flat&logo=cplusplus)](https://www.arduino.cc/)
[![License](https://img.shields.io/badge/License-MIT-yellow?style=flat)](LICENSE)

> *"Time is what we want most, but what we use worst."* - William Penn

**🚨 IMPORTANT: This project is based entirely on the excellent [Adafruit Time Tracking Cube Tutorial](https://learn.adafruit.com/time-tracking-cube/overview) by Brent Rubell and the Adafruit team. All core functionality, hardware design, and original implementation credit goes to Adafruit Industries. This repository contains my personal implementation following their tutorial with additional enhancements.**

This is my implementation of Adafruit's time tracking cube tutorial, enhanced with additional features for improved usability and robustness. The original tutorial provides all the foundational knowledge and design - I've simply added some extra functionality on top of their excellent work.

## 🎥 Demo

https://github.com/user-attachments/assets/a2f46155-c069-45c2-b4e2-25bc5e06bae9

*Watch the cube in action - seamless task switching with beautiful visual feedback!*

## ✨ About This Implementation

### 📚 Original Tutorial Credit
This project is a direct implementation of the **[Adafruit Time Tracking Cube Tutorial](https://learn.adafruit.com/time-tracking-cube/overview)** created by **Brent Rubell** and the **Adafruit Industries team**. The tutorial provides:

- Complete hardware design and 3D printable files
- Wiring diagrams and assembly instructions
- Base Arduino code implementation
- Adafruit IO integration setup
- Zapier automation configuration

**All fundamental concepts, hardware design, and core implementation belong to Adafruit Industries.**

### 🚀 My Enhancements
Following the tutorial completely, I added these improvements to enhance the user experience:

- **📱 Tap-to-Break System**: Added intelligent tap detection using the existing LIS3DH accelerometer
- **🔄 Smart State Management**: Enhanced state transitions with debouncing for more reliable face detection
- **💡 Visual Break Indicator**: Blinking LED pattern when on break for clear status indication
- **🛡️ Robust Orientation Detection**: Improved face detection algorithm with validation delays to prevent false triggers
- **📊 Enhanced Activity Logging**: More detailed logging with Start, Stop, Break, and Resume events

### 🎯 Original Tutorial Features
The Adafruit tutorial provides all of these amazing core features:
- **6-Sided Task Management**: Each cube face represents a different activity
- **Real-time Cloud Sync**: Instant data upload to Adafruit IO
- **Beautiful RGB Feedback**: Custom color schemes for each task type
- **Wireless Connectivity**: ESP32 WiFi for seamless IoT integration
- **Battery Powered**: Portable design for any workspace

## 🛠️ Tech Stack & Components

### Hardware
- **🧠 ESP32 Feather HUZZAH** - The brain with built-in WiFi
- **📏 Adafruit Prop-Maker FeatherWing** - Accelerometer and sensor integration
- **🌈 NeoPixel Stick (8x RGB LEDs)** - Beautiful visual feedback
- **🔊 Piezo Buzzer** - Audio notifications (optional)
- **🏠 3D Printed Enclosure** - Custom cube housing

### Software
- **⚡ Arduino IDE** with ESP32 board support
- **📡 Adafruit IO** for cloud data logging
- **🎨 NeoPixel Library** for LED control
- **📊 LIS3DH Library** for accelerometer data
- **🌐 WiFi Client** for internet connectivity

## 🎨 Task Configuration

Each cube face is mapped to a specific activity with unique visual feedback:

| Face | Task | LED Color | Use Case |
|------|------|-----------|----------|
| 🟠 Left | Coffee Time | Orange/Warm White | Breaks & refreshments |
| 🔵 Right | Programming | Blue/Cool White | Coding sessions |
| 🟡 Front | Relaxation | Yellow/Orange | Mental breaks |
| 🟢 Back | Base State | Yellow | Default/planning |
| ⚫ Top | Base State | Off | Idle time |
| 🔵 Bottom | Base State | Cyan | Setup/calibration |

*💡 Tap any face during an active task to enter "break mode" - the LEDs will blink to indicate you're on a break!*

## 🔧 Setup & Installation

### Prerequisites
- Arduino IDE with ESP32 board package
- Adafruit IO account ([sign up here](https://io.adafruit.com/))
- WiFi network credentials

### Quick Start
1. **Clone this repository**
   ```bash
   git clone https://github.com/JaeungJayJang/IO_Time_Tracker_Cube.git
   cd IO_Time_Tracker_Cube
   ```

2. **Configure your credentials**
   - Edit `config.h` with your WiFi and Adafruit IO credentials:
   ```cpp
   #define IO_USERNAME "your_adafruit_io_username"
   #define IO_KEY "your_adafruit_io_key"
   #define WIFI_SSID "your_wifi_name"
   #define WIFI_PASS "your_wifi_password"
   ```

3. **Install required libraries** in Arduino IDE:
   - Adafruit IO Arduino
   - Adafruit LIS3DH
   - Adafruit NeoPixel
   - Adafruit Unified Sensor

4. **Upload the code** to your ESP32 Feather

5. **Create the TimeTracker feed** in your Adafruit IO dashboard

### Hardware Assembly
**Please follow the detailed [Adafruit tutorial](https://learn.adafruit.com/time-tracking-cube/overview) for complete assembly instructions including:**
- 3D printing the cube enclosure (STL files provided by Adafruit)
- Wiring diagrams and component connections
- Step-by-step assembly process
- Troubleshooting tips

**The tutorial is comprehensive and includes everything needed for a successful build.**

## 📊 Data Integration

The cube logs detailed activity data to Adafruit IO:
- **Start events**: When beginning a new task
- **Stop events**: When switching between tasks
- **Break events**: When tapping to pause
- **Resume events**: When returning from break

### Zapier Integration
Set up automated data export to Google Sheets, Notion, or your favorite productivity app using Zapier webhooks with the Adafruit IO integration.

## 🎯 My Development Approach

Following the Adafruit tutorial completely, I wanted to explore how to enhance the user experience without changing the core functionality. My additions focus on:
- **Improved reliability** through better state management
- **Enhanced user feedback** with visual break indicators  
- **More detailed tracking** for better productivity insights

The beauty of open-source tutorials like Adafruit's is that they provide a solid foundation for experimentation and learning!

## 🛠️ Technical Deep Dive

### Enhanced State Management
My implementation includes sophisticated debouncing and state validation:

```cpp
// Check cube orientation with validation
int faceState = check_the_face(event);
if (faceState == 0) return;

delay(300);  // Debounce
// Validate the reading
if (check_the_face(event) == faceState) {
    // Process state change
}
```

### Smart Break Detection
The tap detection system provides seamless break management:

```cpp
uint8_t click = lis.getClick();
if (click & 0x1 || click & 2 || click & 4) {
    touched = !touched;
    if (currentTask != taskBase) {
        if (touched) {
            cubetask->save("Break: " + currentTask);
        } else {
            cubetask->save("Resume: " + currentTask);
        }
    }
}
```

## 🔮 Future Enhancements

- [ ] **Mobile App**: Companion app for detailed analytics
- [ ] **Multiple Cubes**: Support for team environments
- [ ] **AI Integration**: Smart task recommendations
- [ ] **Voice Commands**: Alexa/Google Assistant integration
- [ ] **Advanced Analytics**: Productivity pattern analysis

### � Learning Journey

Following the Adafruit tutorial was an incredible learning experience that taught me:
- **IoT Development**: Understanding sensor integration and cloud connectivity
- **ESP32 Programming**: Working with WiFi, accelerometers, and LED strips
- **Code Enhancement**: Building upon existing codebases respectfully
- **Documentation**: Properly attributing original work while showcasing improvements

**I highly recommend following the [original Adafruit tutorial](https://learn.adafruit.com/time-tracking-cube/overview) first** - it's exceptionally well-written and provides all the foundational knowledge needed to build this project.

## 🤝 Contributing

Found a bug or want to add a feature? I'd love your input!

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments & Credits

- **🏆 Primary Credit: Adafruit Industries** - This entire project is based on their excellent [Time Tracking Cube Tutorial](https://learn.adafruit.com/time-tracking-cube/overview)
- **👨‍💻 Original Code Author: Brent Rubell** - All core functionality and original implementation
- **🎨 Hardware Design: Adafruit Team** - 3D printable enclosure and component selection
- **📚 Tutorial Creation: Adafruit Learning System** - Comprehensive documentation and learning materials
- **🛠️ Component Design: Adafruit Industries** - Feather ecosystem and FeatherWing design
- **🤝 The Maker Community** - For inspiration and open-source collaboration

**This project exists thanks to Adafruit's commitment to open-source education and their fantastic tutorial. Please support them by purchasing components from [Adafruit](https://www.adafruit.com/)!**

## 📧 Connect With Me

Enhanced with ❤️ by [Jaeung Jang](https://github.com/JaeungJayJang) following the amazing [Adafruit Tutorial](https://learn.adafruit.com/time-tracking-cube/overview)

*Interested in learning IoT development? Start with the original Adafruit tutorial - it's fantastic!*

---

⭐ **Star this repo if you found the enhancements helpful!** ⭐  
🏆 **Support the original creators at [Adafruit](https://www.adafruit.com/)!** 🏆
