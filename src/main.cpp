// the program works with fans connected on normally closed ports, so to turn off the fan the relay must be on

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <sstream>
#include <private.h>

// wifi
const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;
WiFiClient espClient;
PubSubClient client(espClient);

const char *mqtt_broker = "192.168.1.1";
const int mqtt_port = 1883;

#ifdef LEFT
  const char *client_id = "rackfanleft";
  const char *fan1_status_topic = "rackfanleft/fan1/status";
  const char *fan2_status_topic = "rackfanleft/fan2/status";
  const char *fan1_topic = "rackfanleft/fan1/state";
  const char *fan2_topic = "rackfanleft/fan2/state";
#endif
#ifdef RIGHT
  const char *client_id = "rackfanright";
  const char *fan1_status_topic = "rackfanright/fan1/status";
  const char *fan2_status_topic = "rackfanright/fan2/status";
  const char *fan1_topic = "rackfanright/fan1/state";
  const char *fan2_topic = "rackfanright/fan2/state";
#endif
#ifdef CENTER
  const char *client_id = "rackfancenter";
  const char *fan1_status_topic = "rackfancenter/fan1/status";
  const char *fan2_status_topic = "rackfancenter/fan2/status";
  const char *fan1_topic = "rackfancenter/fan1/state";
  const char *fan2_topic = "rackfancenter/fan2/state";
#endif



int fan1_status = 0;
int fan2_status = 0;
char in_message[100];

#define fan1 1
#define fan2 3

unsigned long previousMillis = 0;

void callback(char *topic, byte *payload, unsigned int length)
{
  unsigned int i = 0;
  for (; i < length; i++)
  {
    in_message[i] = char(payload[i]);
  }
  in_message[i] = '\0';
  if (strcmp(topic, fan1_topic) == 0)
  {
    fan1_status = atoi(in_message);
  }
  if (strcmp(topic, fan2_topic) == 0)
  {
    fan2_status = atoi(in_message);
  }
}

void InitWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  // Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    // Serial.print('.');
    delay(1000);
  }
  // Serial.println(WiFi.localIP());
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

void reconnect()
{
  while (!client.connected())
  {
    if (client.connect(client_id))
    {
      Serial.println("MQTT broker connected");
      client.publish(status_topic, "connected");
      client.publish(motion_topic, "0", true);
    }
    else
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void InitMqtt()
{
  client.setServer(mqtt_broker, mqtt_port);
  // client.setCallback(callback);
  reconnect();
}

void setup()
{
  InitWiFi();
  delay(1000);
  InitMqtt();
  delay(1000);

  pinMode(fan1, OUTPUT);
  pinMode(fan2, OUTPUT);
  digitalWrite(fan1, HIGH);
  digitalWrite(fan2, HIGH);
}

void loop()
{

  if (client.connected())
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 5000)
    {
      previousMillis = currentMillis;
      if (fan1_status == 0)
  {
    digitalWrite(fan1, LOW);
    client.publish(fan1_status_topic, "off");
  }
  else
  {
    digitalWrite(fan1, HIGH);
    client.publish(fan1_status_topic, "on");
  }

  if (fan2_status == 0)
  {
    digitalWrite(fan2, LOW);
    client.publish(fan2_status_topic, "off");
  }
  else
  {
    digitalWrite(fan2, HIGH);
    client.publish(fan2_status_topic, "on");
  }
    }
  }
  else
  {
    reconnect();
  }
}