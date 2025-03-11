Collecting workspace informationThis code sets up an ESP32 microcontroller as a Wi-Fi access point and web server to control GPIO pins and read RGB values from a TCS34725 color sensor. Here's a summary of how it works:

1. **Setup**:
   - Includes necessary libraries: `WiFi.h`, `Wire.h`, and `Adafruit_TCS34725.h`.
   - Defines network credentials, web server port, and GPIO pins.
   - Initializes the TCS34725 sensor.
   - Configures the ESP32 as a Wi-Fi access point.
   - Starts the web server.

2. **Main Loop**:
   - Listens for incoming client connections.
   - Reads HTTP requests from clients.
   - Parses the request to control GPIO pins 20 and 21 based on the URL (`/20/on`, `/20/off`, `/21/on`, `/21/off`).
   - Reads RGB values from the TCS34725 sensor.
   - Sends an HTML response to the client, displaying the current state of the GPIO pins and the background color based on the RGB values.

3. **HTTP Response**:
   - Constructs an HTML page with buttons to control the GPIO pins.
   - Sets the background color of the page to the RGB values read from the sensor.

This allows users to connect to the ESP32's Wi-Fi network, open a web browser, and control the GPIO pins while viewing the color detected by the sensor.
