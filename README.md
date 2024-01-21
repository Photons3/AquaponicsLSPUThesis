# Aquaponics System ESP32
Automated Aquaponics System powered by Artificial Neural Network created for ESP32.

Created from ESPIDF, ESPNN, Tensorflow for Microcontrollers and ESP-IDF Components library.

Created as a capstone project for Laguna State Polytechnic University
Aquaponics System
![image](https://github.com/Photons3/AquaponicsLSPUThesis/assets/18119113/7311f7c9-a31b-4d40-b6f1-90a68240caee)
Supports different sensor acquitition and parameter controls
![image](https://github.com/Photons3/AquaponicsLSPUThesis/assets/18119113/76bed2a9-d8cb-455b-b96c-f760cb0a1e79)



**Features:**
1. Forecast of sensor values using Tensorflow Lite for Microcontrollers trained using Kaggle Dataset for Aquaponics System.
![image](https://github.com/Photons3/AquaponicsLSPUThesis/assets/18119113/a4a5298a-88fc-40ac-994e-908f26a3862e)
2. Supports data transmission through IoT using HiveMQTT Protocol.
   (Create MQTT account and change the header for HiveMQTT and .pem cerificate for this to work)
3. Allows uploading of data through a webserver and set parameters for the system in the internet.
   ![image](https://github.com/Photons3/AquaponicsLSPUThesis/assets/18119113/0d65e95a-925e-46e8-8fc9-5e11757a2a9a)
4. Uses Lagrange Interpolation for Polynomials for better system parameters control

**Instructions for use:**
1.	Download and install Visual Studio Code from Microsoft website.
2.	Install the Espressif IoT Development Framework extension in VS Code. (https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/vscode-setup.html)
3.	Open the AquaponicsLSPUThesis folder provided in the CD.
4.	Press Ctrl+Shift+P to open the configuration manager in VS Code, search for ESP-IDF: SDK Configuration Editor (menuconfig).
5.	Find the Wifi and MQTT tab in the configuration editor.
6.	Replace the WIFI SSID and Password accordingly.
7.	If you have changed the MQTT broker replace the MQTT Broker.
8.	If you have changed the MQTT broker follow the ESP-IDF manual for creating an SSL certificate for the new URI. (https://github.com/espressif/esp-idf/tree/01d014c42d/examples/protocols/mqtt/ssl).
9.	Replace the contents of main/hivemq_client_cert.pem with the newly created SSL key and save the file.
10.	Click Save.
11.	Press the ESP-IDF Full Clean at the bottom of the screen.
12.	Press ESP-IDF Build Project and wait for the compilation to finish.
13.	Connect the PC or Laptop to the NodeMCU-32 with a USB A to MicroUSB B connector.
14.	Press the ESP-IDF Select Port and choose the desired communication port.
15.	Press the ESP-IDF Flash Device and when the screen flashes waiting for the device, press and hold the reset button on the ESP32 located at the right of the USB port.
16.	After the flashing is done remove the USB cable from the device.
17.	Check to see if error occurs.

For Hive MQTT
1.	Go to https://console.hivemq.cloud/ and log-in.
2.	If the cluster is full, create a new cluster and add new users with a password for the created cluster.
3.	Update the settings for the NodeMCU-32 and Heroku Website accordingly.

