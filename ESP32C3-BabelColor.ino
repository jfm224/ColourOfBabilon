
// Load Wi-Fi library
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>

// Replace with your network credentials
const char* ssid     = "ColorIIoT";
const char* password = "colour-of-babilon2025";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output20State = "off";
String output21State = "off";

// Assign output variables to GPIO pins
const int output20 = 20;
const int output21 = 21;

// Define the pins for the RGB LED
const int redPin = 3;
const int greenPin = 2;
const int bluePin = 1;

// TCS34725 sensor
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_4X);

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output20, OUTPUT);
  pinMode(output21, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output20, LOW);
  digitalWrite(output21, LOW);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // Initialize TCS34725 sensor
  if (tcs.begin()) {
    Serial.println("Encontrado o TCS34725");
  } else {
    Serial.println("Falha ao encontrar o TCS34725");
    while (1);
  }

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Configurando AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("Endereço de IP AP: ");
  Serial.println(IP);
  
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("Client novo.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /20/on") >= 0) {
              Serial.println("GPIO 20 ligado");
              output20State = "on";
              digitalWrite(output20, HIGH);
            } else if (header.indexOf("GET /20/off") >= 0) {
              Serial.println("GPIO 20 desligado");
              output20State = "off";
              digitalWrite(output20, LOW);
            } else if (header.indexOf("GET /21/on") >= 0) {
              Serial.println("GPIO 21 ligado");
              output21State = "on";
              digitalWrite(output21, HIGH);
            } else if (header.indexOf("GET /21/off") >= 0) {
              Serial.println("GPIO 21 desligado");
              output21State = "off";
              digitalWrite(output21, LOW);
            }
            
            // Read RGB values from TCS34725 sensor
            uint16_t r, g, b, c;
            tcs.getRawData(&r, &g, &b, &c);
            Serial.printf("R: %d, G: %d, B: %d, C: %d\n", r, g, b, c);
            // Normalize RGB values to 0-255
            uint8_t red = map(r, 0, 65535, 0, 255); // Adjust the upper limit if needed
            uint8_t green = map(g, 0, 65535, 0, 255); // Adjust the upper limit if needed
            uint8_t blue = map(b, 0, 65535, 0, 255); // Adjust the upper limit if needed
            // Set the color of the RGB LED
//            analogWrite(redPin, red);
//            analogWrite(greenPin, green);
//            analogWrite(bluePin, blue);
            // Set the color of the RGB LED based on the captured RGB values
//            digitalWrite(redPin, r > 32767 ? HIGH : LOW);
//            digitalWrite(greenPin, g > 32767 ? HIGH : LOW);
//            digitalWrite(bluePin, b > 32767 ? HIGH : LOW);

            
            // Display the HTML web page with background color set to the captured RGB values
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.printf("<body style='background-color: rgb(%d, %d, %d);'>", r, g, b);
            client.println("<h1>Babel Color ESP 2025</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 20  
            client.println("<p>GPIO 20 - State " + output20State + "</p>");
            // If the output20State is off, it displays the ON button       
            if (output20State=="off") {
              client.println("<p><a href=\"/20/on\"><button class=\"button\">Ligar</button></a></p>");
            } else {
              client.println("<p><a href=\"/20/off\"><button class=\"button button2\">Desligar</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 21  
            client.println("<p>GPIO 21 - State " + output21State + "</p>");
            // If the output21State is off, it displays the ON button       
            if (output21State=="off") {
              client.println("<p><a href=\"/21/on\"><button class=\"button\">Ligar</button></a></p>");
            } else {
              client.println("<p><a href=\"/21/off\"><button class=\"button button2\">Desligar</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Cliente desconnectedo.");
    Serial.println("");
  }
}
