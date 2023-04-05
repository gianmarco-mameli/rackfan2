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
const char *status_topic = "rackfanleft/status";
const char *fan1_status_topic = "rackfanleft/fan1/status";
const char *fan2_status_topic = "rackfanleft/fan2/status";
const char *fan1_topic = "rackfanleft/fan1/state";
const char *fan2_topic = "rackfanleft/fan2/state";
#endif
#ifdef RIGHT
const char *client_id = "rackfanright";
const char *status_topic = "rackfanright/status";
const char *fan1_status_topic = "rackfanright/fan1/status";
const char *fan2_status_topic = "rackfanright/fan2/status";
const char *fan1_topic = "rackfanright/fan1/state";
const char *fan2_topic = "rackfanright/fan2/state";
#endif
#ifdef CENTER
const char *client_id = "rackfancenter";
const char *status_topic = "rackfancenter/status";
const char *fan1_status_topic = "rackfancenter/fan1/status";
const char *fan2_status_topic = "rackfancenter/fan2/status";
const char *fan1_topic = "rackfancenter/fan1/state";
const char *fan2_topic = "rackfancenter/fan2/state";
#endif

uint8_t fan1_status = 1;
uint8_t fan2_status = 1;


#define fan1 1
#define fan2 3

unsigned long previousMillis = 0;

void callback(char *topic, byte *payload, unsigned int length)
{
  char in_message[5];
  unsigned int i = 0;
  for (; i < length; i++)
  {
    in_message[i] = char(payload[i]);
  }
  in_message[i] = '\0';

  if (strcmp(topic, fan1_topic) == 0)
  {

    fan1_status = atoi(in_message);
    // digitalWrite(fan1, fan1_status);

  }
  if (strcmp(topic, fan2_topic) == 0)
  {
    fan2_status = atoi(in_message);
    // digitalWrite(fan1, fan2_status);
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
      // Serial.println("MQTT broker connected");
      client.publish(status_topic, "connected");
      client.publish(fan1_topic, "1", true);
      client.publish(fan2_topic, "1", true);
      client.subscribe(fan1_topic);
      client.subscribe(fan2_topic);
    }
    else
    {
      // Serial.print("failed with state ");
      // Serial.print(client.state());
      delay(5000);
    }
  }
}

void InitMqtt()
{
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  reconnect();
}

void setup()
{
  pinMode(fan1, OUTPUT);
  pinMode(fan2, OUTPUT);
  delay(500);
  digitalWrite(fan1, fan1_status);
  digitalWrite(fan2, fan2_status);
  delay(500);

  InitWiFi();
  delay(1000);
  InitMqtt();
}

void loop()
{
  if (client.connected())
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 5000)
    {
      previousMillis = currentMillis;
      client.loop();

      if (fan1_status == 0)
      {
        digitalWrite(fan1, LOW);
        client.publish(fan1_status_topic, "off");
      }
      // if (fan1_status == 1)
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
      // if (fan2_status == 1)
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