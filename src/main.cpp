#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <Losant.h>

//wifi credentials
const char *WIFI_SSID = "";
const char *WIFI_PASS = "";

//losant credentials
const char *LOSANT_DEVICE_ID = "";
const char *LOSANT_ACCESS_KEY = "";
const char *LOSANT_ACCESS_SECRET = "";

WiFiClientSecure wifiClient;

LosantDevice device(LOSANT_DEVICE_ID);

// Digital out pin that is turned on when reading
// and then turned off when not reading
#define sensor_on 33

// moisture analog input pin
const int moisture_sensor_input = A3;

// moisture reading value
int moisture_reading = 0;

// battery voltage pin
const int battery_voltage_pin = A13;

// battery voltage value
int batteryVoltageValue = 0;


// function to connect to wifi
void connect() {

  // Connect to Wifi.
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println();
  Serial.print("Connecting to Losant...");

  device.connectSecure(wifiClient, LOSANT_ACCESS_KEY, LOSANT_ACCESS_SECRET);

  while (!device.connected()) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected!");
  Serial.println("This device is now ready for use!");
} 

// this function will send moisture value to Losant
void reportAttibutes(int moisture, int batteryV)
{
  StaticJsonDocument<200> root;
  root["moisture"] = moisture;
  root["batteryV"] = batteryV;
  JsonObject object = root.as<JsonObject>();
  device.sendState(object);
}


void setup() {
  Serial.begin(9600);
  pinMode(sensor_on,OUTPUT);
  digitalWrite(sensor_on, LOW); // turn sensor off at first to make sure there's no power

  connect();
}

// this funciton reads moisture form the moisture sensor
int read_moisture() {
  // turn on the sensor
  digitalWrite(sensor_on, HIGH);

  // delay for accuracy
  delay(10);

  // read the value
  moisture_reading = analogRead(moisture_sensor_input);

  // turn the sensor off
  digitalWrite(sensor_on, LOW);

  return moisture_reading;
}

int read_battery() {
  batteryVoltageValue = analogRead(battery_voltage_pin);
  
  // huzzah32 reads 1/2 voltage
  batteryVoltageValue = batteryVoltageValue * 2;

  return batteryVoltageValue;
}

void loop() {
  Serial.print("Moisture = ");
  Serial.println(read_moisture());

  Serial.print("Battery = ");
  Serial.println(read_battery());

  // sent moisture & battery values to Losant
  reportAttibutes(read_moisture(), read_battery());

  // and then do it every 15 minutes
  delay(15 * 60 * 1000);
}