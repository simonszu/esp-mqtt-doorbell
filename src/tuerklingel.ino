#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* SSID = "SSID";
const char* PSK = "PWD";
const char* MQTT_BROKER = "192.168.10.4";
uint32_t bootTime, setupTime, WiFiTime;
IPAddress local_IP(192, 168, 12, 50);
IPAddress gateway(192, 168, 12, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 12, 1);

WiFiClient espClient;
PubSubClient client(espClient);
ADC_MODE(ADC_VCC);

RF_PRE_INIT() {
  bootTime = millis();
}

void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(SSID);
    WiFi.mode(WIFI_STA);
    WiFi.config(local_IP,gateway,subnet);
    WiFi.hostname("flur-tuerklingel");
    WiFi.begin(SSID, PSK);
 
    while (WiFi.status() != WL_CONNECTED) {
        delay(5);
        Serial.print(".");
    }
 
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void setup() {
  setupTime=millis();
  Serial.begin(19200);
  Serial.println("Waking up...");
  
  setup_wifi();
  WiFiTime=millis();

  Serial.println();
  Serial.println(ESP.getResetReason().c_str());
  Serial.printf("Core: %s/%s\n",ESP.getCoreVersion().c_str(),ESP.getSdkVersion());
  Serial.printf("bootTime: %d setupTime: %d WifiTime %d\n", bootTime, setupTime, WiFiTime);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
  Serial.println();
  client.setServer(MQTT_BROKER, 1883);
  connect_mqtt();

  float vcc = ESP.getVcc();
  String vccstring = String(vcc);
  char c[vccstring.length()+1];
  vccstring.toCharArray(c,vccstring.length()+1);
  client.publish("tuerklingel/status", "online");
  client.publish("tuerklingel/vcc", c);
  delay(500);
  client.publish("tuerklingel/status", "offline");
  delay(100);
  Serial.println("Published, going to sleep now");
  ESP.deepSleep(0);
}


void connect_mqtt() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("tuerklingel")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {

}
