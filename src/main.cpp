#include <Arduino.h>
// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
// Replace with your network credentials
const char* ssid = "BASAK"; // Input your wifi network name
const char* password = "09052011"; // Input your wifi password

IPAddress ip(192, 168, 1, 110);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// Set web server port number to 3000
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String calState = "off";
String relay2State = "off";
String cwState = "off";
String ccwState = "off";


// Assign output variables to GPIO pins
const int cal = 02; // GPIO5 
//const int relay2 = 02; // GPIO4 
const int cw = 15; // GPIO4 
const int ccw = 02; // GPIO4 

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  //pinMode(cal, OUTPUT);
  //pinMode(relay2, OUTPUT);
  pinMode(cw, OUTPUT);
  pinMode(ccw, OUTPUT);

  // Set outputs to HIGH. relay active LOW
  //digitalWrite(cal, HIGH);
  //digitalWrite(relay2, HIGH);
  digitalWrite(cw, HIGH);
  digitalWrite(ccw, HIGH);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  //WiFi.mode(WIFI_STA);
	WiFi.config(ip, gateway, subnet);
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
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
            
            
            if (header.indexOf("GET /0/on") >= 0) // CW ON
            {
              Serial.println("GPIO 0 on");
              cwState = "on";
              digitalWrite(cw, LOW);
              ccwState = "off";
              calState = "off";
              digitalWrite(ccw, HIGH);
              
              
            }
            else if (header.indexOf("GET /0/off") >= 0) // CW OFF
            {
              Serial.println("GPIO 0 off");
              cwState = "off";
              digitalWrite(cw, HIGH);
            }
            else if (header.indexOf("GET /2/on") >= 0) { // CCW ON
              Serial.println("GPIO 2 on");
              ccwState = "on";
              digitalWrite(ccw, LOW);
              cwState = "off";
              calState = "off";
              digitalWrite(cw, HIGH);
              
            }
            else if (header.indexOf("GET /2/off") >= 0) { // CCW OFF
              Serial.println("GPIO 2 off");
              ccwState = "off";
              digitalWrite(ccw, HIGH);
            }
            else if (header.indexOf("GET /5/on") >= 0) { // CALIBRATE ON
              Serial.println("GPIO 5 on");
              calState= "on";
              digitalWrite(ccw, LOW);
              cwState = "off";
              ccwState = "off";
              digitalWrite(cw, HIGH);
              
             
            }
            else if (header.indexOf("GET /5/off") >= 0) { // CALIBRATE OFF
              Serial.println("GPIO 5 off");
              calState = "off";
              digitalWrite(ccw, HIGH);
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            //client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html, body { font-family: Helvetica; display: block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #209e48; border: none; color: white; padding: 12px 24px;");
            client.println("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #c20a0a;}");
            client.println(".textbox {width: 80px; border: 1px solid #333; padding: 16px 20px 0px 24px; background-image: linear-gradient(180deg, #fff, #ddd 40%, #ccc);}");
            client.println(".mytext {font-size: 16px; font-weight:bold; font-family:Arial ; text-align: justify;}");
            client.println("#container {width: 100%; height: 100%; margin-left: 5px; margin-top: 20px; padding: 10px; display: -webkit-flex; -webkit-justify-content: center; display: flex; justify-content: center;} ");
            
            client.println("</style></head>");

            
            // Web Page Heading
            client.println("<body><h1>  TURN TABLE</h1>");

            // Display current state, and ON/OFF buttons for GPIO 5
            client.println("<div id=\"container\">");
            client.println("<p><div class=\"textbox mytext\">CALIBRATE </div>");
            // If the calState is off, it displays the ON button
            if (calState == "off") {
              client.println("<a href=\"/5/on\"><button class=\"button\">OFF</button></a></p>");
            } else {
              client.println("<a href=\"/5/off\"><button class=\"button button2\">ON</button></a></p>");
            }
            client.println("</div>");
            
            // Display current state, and ON/OFF buttons for GPIO 0
            client.println("<div id=\"container\">");
            client.println("<p><div class=\"textbox mytext\">CW </div>");
            // If the calState is off, it displays the ON button
            if (cwState == "off") {
              client.println("<a href=\"/0/on\"><button class=\"button\">OFF</button></a></p>");
            } else {
              client.println("<a href=\"/0/off\"><button class=\"button button2\">ON</button></a></p>");
            }
            client.println("</div>");
            
            // Display current state, and ON/OFF buttons for GPIO 2
            client.println("<div id=\"container\">");
            client.println("<p><div class=\"textbox mytext\">CCW </div>");
            // If the relay2State is off, it displays the ON button
            if (ccwState == "off") {
              client.println("<a href=\"/2/on\"><button class=\"button\">OFF</button></a></p>");
            } else {
              client.println("<a href=\"/2/off\"><button class=\"button button2\">ON</button></a></p>");
            }
            client.println("</div>");

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
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
