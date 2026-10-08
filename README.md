## Introduction

In today's technologically advanced world, energy management is a critical aspect of modern smart homes. Traditional energy meters, while functional, often lack the capability for real-time monitoring and remote access. The advent of IoT (Internet of Things) has revolutionized this space, enabling the creation of smart energy meters that offer precise, real-time monitoring and control. In this article, we present a comprehensive guide to building a Smart IoT Energy Meter using the ESP32 microcontroller, ACS712 current sensor, ZMPT101B voltage sensor, I2C LCD display, Blynk 2.0 platform, and Telegram bot integration.

## Components and Tools

### Hardware Components
- **ESP32 Development Board**: The core microcontroller is used for processing and connectivity.
- **ACS712 Current Sensor**: Measures the AC current flowing through a load.
- **ZMPT101B Voltage Sensor**: Measures the AC voltage across a load.
- **I2C LCD Display**: Displays real-time data such as voltage, current, power, energy consumption, and cost.
- **Breadboard and Jumper Wires**: For prototyping and connections.

### Software Tools
- **Arduino IDE**: For programming the ESP32.
- **Blynk 2.0**: For creating a web and mobile dashboard for monitoring and control.
- **Telegram**: For notifications and alerts via a Telegram bot.

### Connections
- **ZMPT101B Voltage Sensor**:
  - VCC to ESP32 3.3V
  - GND to ESP32 GND
  - OUT to ESP32 GPIO 34 (Analog Input)

- **ACS712 Current Sensor**:
  - VCC to ESP32 3.3V
  - GND to ESP32 GND
  - OUT to ESP32 GPIO 35 (Analog Input)
  - IP+ and IP- in series with the load.

- **I2C LCD Display**:
  - VCC to ESP32 3.3V
  - GND to ESP32 GND
  - SDA to ESP32 GPIO 21
  - SCL to ESP32 GPIO 22

## Software Setup

### 1. **Setting Up Blynk 2.0**

#### Creating a Template
1. **Log in to Blynk** and create a new template.
2. Set the **template name**, **hardware type** (ESP32), and other settings.
3. Create **datastreams** for:
   - Voltage (V0)
   - Current (V1)
   - Power (V2)
   - Energy Consumption (V3)
   - Total Cost (V4)

#### Web and Mobile Dashboard
- **Web Dashboard**: Add widgets like Gauges, Displays, etc., and link them to the corresponding datastreams.
- **Mobile Dashboard**: Use the Blynk app to configure similar widgets for real-time monitoring on your smartphone.

### 2. **Creating a Telegram Bot**

1. **Create a Bot** using BotFather in the Telegram app.
2. Save the **Bot Token** and obtain your **Chat ID** using a bot like `@userinfobot`.

### 3. **Programming the ESP32**

#### Libraries Required
- **WiFi.h**: For Wi-Fi connectivity.
- **BlynkSimpleEsp32.h**: For Blynk 2.0 integration.
- **Wire.h**: For I2C communication.
- **LiquidCrystal_I2C.h**: For LCD control.
- **UniversalTelegramBot.h**: For Telegram bot integration.

### 4. **Calculating Energy and Cost**

- **Voltage Measurement**: Convert the analog reading from the ZMPT101B sensor to the actual AC voltage.
- **Current Measurement**: Convert the analog reading from the ACS712 sensor to the actual AC.
- **Power Calculation**: \( \text{Power} = \text{Voltage} \times \text{Current} \)
- **Energy Consumption**: Integrate power over time to find the total energy consumed.
- **Cost Calculation**: Multiply the total energy consumed by the cost per unit of energy.

## Conclusion

This smart IoT energy meter offers a comprehensive solution for monitoring and managing household energy consumption. The integration with Blynk 2.0 provides an intuitive user interface for real-time data visualization, while the Telegram bot adds layer of convenience with automated billing notifications. By leveraging modern IoT technologies, this project helps monitor energy consumption and contributes to energy conservation efforts.

## Future Enhancements

- **Data Logging**: Implement data logging for historical analysis.
- **Smart Control**: Add appliance control features based on energy usage patterns.
- **Advanced Analytics**: Use machine learning algorithms to predict future consumption and optimize energy use.

This guide should provide a solid foundation for building and expanding a smart IoT energy meter system. Whether you're a hobbyist, student, or professional, this project can be an excellent way to explore the intersection of IoT, data analytics, and smart home technologies.
For full step by step guide check out this link: https://circuitdiagrams.in/iot-smart-energy-meter-using-esp32
