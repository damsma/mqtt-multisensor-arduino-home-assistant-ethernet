# MQTT Multisensor for Home Assistant
Create your own mqtt multisensor for HA just by configuring the values in the header file.

This code runs on my POE powered multi-sensors in my house. Each room has one header file, where the number and occurance of sensors are configured.

![grafik](https://github.com/damsma/mqtt-multisensor-arduino-home-assistant-ethernet/assets/45398732/cbd14dd6-9130-4490-9ce1-1c4f8fab6b70)

# Features
- SHT30 - Temperature, Humidity
- BH1750FVI - Luxmeter
- Termipol PM-1/2 - Waterflow
- Contact sensor - Door, Window, Motion, Water leak detection, etc.
- Relay - Switch
- Relay - Light

# How to use it
Copy a header file (for example room_test1.h) and rename it according to your room name.

Comment out the defines that you do not need.

Change/add the sensor names, icons, pins in the corresponding variables.

Set your IP and MAC address.

Include your new header file in mqtt-multisensor-arduino-home-assistant-ethernet.ino


# Thanks to
[Arduino Home Assistant integration](https://github.com/dawidchyrzynski/arduino-home-assistant)

Waterflow class based on [Water Meter Pulse Sensor](https://www.mysensors.org/build/pulse_water)
