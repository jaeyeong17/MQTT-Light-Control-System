#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>

DHTesp              dht;
unsigned long       interval = 2000;
unsigned long       lastDHTReadMillis = 0;
float               humidity = 0;
float               temperature = 0;
 
const char*         ssid ="iptime";
const char*         password = "jhs2031%";
const char*         mqttServer = "52.7.186.144";
const int           mqttPort = 1883;

unsigned long       pubInterval = 5000;
unsigned long       lastPublished = - pubInterval;
 
WiFiClient espClient;
PubSubClient client(espClient);
void readDHT22();

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA); 
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to the WiFi network");
    client.setServer(mqttServer, mqttPort);
 
    while (!client.connected()) {
        Serial.println("Connecting to MQTT...");
 
        if (client.connect("ESP8266Client")) {
            Serial.println("connected");  
        } else {
            Serial.print("failed with state "); Serial.println(client.state());
            delay(2000);
        }
    }
    dht.setup(14, DHTesp::DHT22); // Connect DHT sensor to GPIO 14
}

void loop() {
    client.loop();

    unsigned long currentMillis = millis();
    if(currentMillis - lastPublished >= pubInterval) {
        lastPublished = currentMillis;
        readDHT22();
        Serial.printf("%.1f\t %.1f\n", temperature, humidity);
        char buf[10];
        sprintf(buf, "%.1f", temperature);
        client.publish("deviceid/yhur/evt/temperature", buf);
        sprintf(buf, "%.1f", humidity);
        client.publish("deviceid/yhur/evt/humidity", buf);
        int light = analogRead(0);
        sprintf(buf, "%d", light);
        client.publish("deviceid/yhur/evt/light", buf);
    }
}

void readDHT22() {
    unsigned long currentMillis = millis();

    if(currentMillis - lastDHTReadMillis >= interval) {
        lastDHTReadMillis = currentMillis;

        humidity = dht.getHumidity();              // Read humidity (percent)
        temperature = dht.getTemperature();        // Read temperature as Fahrenheit
    }
}