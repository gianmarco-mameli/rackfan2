// the program works with fans connected on normally closed ports, so to turn off the fan the relay must be on

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <sstream>
#include <private.h>

const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;

const char *mqtt_broker = "192.168.1.1";
const int mqtt_port = 1883;

const char *client_id = "rackfan2";
const char *fan1_status_topic = "rackfan2/fan1/status";
const char *fan2_status_topic = "rackfan2/fan2/status";
const char *fan1_topic = "rackfan2/fan1/state";
const char *fan2_topic = "rackfan2/fan2/state";

int fan1_status = 0;
int fan2_status = 0;
char in_message[100];

#define fan1 1
#define fan2 3

WiFiClient espClient;
PubSubClient client(espClient);

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
  if (WiFi.status() == WL_NO_SHIELD)
  {
    while (true)
      ;
  }
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(ssid, pass);
  }
}

void InitMqtt()
{
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
}

void reconnect()
{
  while (!client.connected())
  {
    if (client.connect(client_id))
    {
      client.publish(fan1_status_topic, "connected");
      client.publish(fan2_status_topic, "connected");
      delay(500);
      client.publish(fan1_topic, "1", true);
      client.publish(fan2_topic, "1", true);
      delay(500);
      client.subscribe(fan1_topic, 1);
      client.subscribe(fan2_topic, 1);
    }
    else
    {
      delay(5000);
    }
  }
}

void setup()
{
  pinMode(fan1, OUTPUT);
  pinMode(fan2, OUTPUT);
  delay(1000);
  digitalWrite(fan1, HIGH);
  digitalWrite(fan2, HIGH);
  delay(1000);
  InitWiFi();
  delay(1000);
  InitMqtt();
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

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
  delay(5000);
}