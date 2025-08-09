#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

// SoftwareSerial setup (RX, TX)
SoftwareSerial arduinoSerial(D7, D8); // RX (NodeMCU), TX (NodeMCU)

// Variables to store sensor data
float temperature = 0;
float humidity = 0;
float co_value = 0;     // Variable for CO reading
float no2_value = 0;    // Variable for NO2 reading
float ozone_value = 0;  // Variable for Ozone reading
float so2_value = 0;    // Variable for SO2 reading

const char* ssid = "iotbt1";
const char* password = "98765432";
const char* host = "myiot.co.in"; // Replace with your server address
const uint16_t port = 80;

void setup() {
  // Start the serial communication
  Serial.begin(9600);
  arduinoSerial.begin(9600);

  // Connect to WiFi
  WiFi.begin(ssid, password);

  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Waiting for data from Arduino...");
}

void loop() {
  // Check if data is available from Arduino
  if (arduinoSerial.available()) {
    // Read data from Arduino as a string
    String sensorData = arduinoSerial.readString();
    Serial.println("Received data: " + sensorData);

    // Parse the received string
    parseSensorData(sensorData);

    // Print stored variables for debugging
    Serial.print("T: ");
    Serial.println(temperature);
    Serial.print("H: ");
    Serial.println(humidity);
    Serial.print("CO: ");
    Serial.println(co_value);
    Serial.print("NO2: ");
    Serial.println(no2_value);
    Serial.print("Ozone: ");
    Serial.println(ozone_value);
    Serial.print("SO2: ");
    Serial.println(so2_value);
  }
  sendDataToServer();
  delay(3000); // Optional delay before the next read
}

// Function to parse sensor data from the received string
void parseSensorData(String data) {
  // Variables to store the positions of delimiters
  int tempIndex, humidIndex, coIndex, no2Index, ozoneIndex, so2Index;

  // Find positions of the sensor labels in the string
  tempIndex = data.indexOf("T:");
  humidIndex = data.indexOf("H:");
  coIndex = data.indexOf("C:");
  no2Index = data.indexOf("N:");
  ozoneIndex = data.indexOf("O:");
  so2Index = data.indexOf("S:");

  // Extract and convert substrings to their respective variables
  if (tempIndex >= 0) {
    temperature = data.substring(tempIndex + 2, humidIndex - 1).toFloat();
  }
  if (humidIndex >= 0) {
    humidity = data.substring(humidIndex + 2, coIndex - 1).toFloat();
  }
  if (coIndex >= 0) {
    co_value = data.substring(coIndex + 2, no2Index - 1).toFloat();
  }
  if (no2Index >= 0) {
    no2_value = data.substring(no2Index + 2, ozoneIndex - 1).toFloat();
  }
  if (ozoneIndex >= 0) {
    ozone_value = data.substring(ozoneIndex + 2, so2Index - 1).toFloat();
  }
  if (so2Index >= 0) {
    so2_value = data.substring(so2Index + 2).toInt();
  }
}



void sendDataToServer() {
  // Create the URL for the GET request
  String url = "/emupdate.php?";
  url += "temp=" + String(temperature);
  url += "&hum=" + String(humidity);
  url += "&co=" + String(co_value);
  url += "&no=" + String(no2_value);
  url += "&ozo=" + String(ozone_value);
  url += "&so=" + String(so2_value);

  Serial.print("Connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create a TCP connection
  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("Connection to server failed");
    return;
  }

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Data sent to server!");
  Serial.println("URL: " + url);

  // Wait for the server response
  while (client.connected() || client.available()) {
    if (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println(line); // Print the server response
    }
  }

  client.stop(); // Close the connection
}
