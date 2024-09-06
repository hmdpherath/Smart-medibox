#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Set web server port number to 80
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to Wi-Fi");
  Serial.println(WiFi.localIP()); // Display the IP address
  
  // Start the server
  server.begin();
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client.");
    String currentLine = ""; // Make sure to clear the string
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          // If the current line is blank, you have two newline characters in a row.
          if (currentLine.length() == 0) {
            // HTTP response
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // Web page content
            client.println("<!DOCTYPE HTML><html>");
            client.println("<h1>Medibox Status</h1>");
            client.println("<p>Temperature: 25°C</p>");
            client.println("<p>Humidity: 60%</p>");
            client.println("</html>");
            
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    // Close the connection
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
