#include "MQTTManager.h"

MQTTManager::MQTTManager(WiFiManager &wifiManager)
    : wifiManager(wifiManager), broker(MQTT_BROKER), port(MQTT_PORT), clientId(MQTT_CLIENT_ID), connected(false), lastReconnectAttempt(0), mqttClient(1024)
{
}

bool MQTTManager::begin()
{
    if (!wifiManager.isConnected())
    {
        return false;
    }

    mqttClient.begin(broker, port, wifiClient);
    mqttClient.setKeepAlive(keepAliveIntervalMs);

    int attempts = 0;
    while (!mqttClient.connect(clientId) && attempts < connectionAttempts)
    {
        delay(connectionAttemptDelayMs);
        attempts++;
    }

    connected = mqttClient.connected();

    if (connected && !subscribedTopic.isEmpty())
    {
        Serial.printf("Subscribing to topic: %s with QoS: %d\n", subscribedTopic.c_str(), subscribedQos);
        mqttClient.subscribe(subscribedTopic.c_str(), subscribedQos);
    }

    return connected;
}

void MQTTManager::update()
{
    if (!wifiManager.isConnected())
    {
        connected = false;
        return;
    }

    if (!mqttClient.connected())
    {
        connected = false;
        unsigned long currentMillis = millis();

        if (currentMillis - lastReconnectAttempt >= reconnectIntervalMs)
        {
            lastReconnectAttempt = currentMillis;
            begin();
        }
    }
    else
    {
        connected = true;
        mqttClient.loop();
    }
}

bool MQTTManager::isConnected() const
{
    return connected;
}

bool MQTTManager::publish(const char *topic, const char *payload)
{
    if (!connected)
    {
        return false;
    }

    bool retained = false;
    bool success = mqttClient.publish(topic, payload, retained, 0);

    if (!success)
    {
        Serial.printf("Could not publish message - failed\n");
        mqttClient.disconnect();
        connected = false;
    }

    return success;
}

String MQTTManager::getBroker() const
{
    return broker;
}

int MQTTManager::getPort() const
{
    return port;
}

String MQTTManager::getClientId() const
{
    return clientId;
}

bool MQTTManager::subscribe(const char *topic, uint8_t qos)
{
    if (subscribedTopic.equals(topic))
    {
        Serial.printf("Already subscribed to topic: %s\n", subscribedTopic.c_str());
        return true;
    }
    else if (!subscribedTopic.isEmpty() && !subscribedTopic.equals(topic))
    {
        Serial.printf("Could not subscribe to topic - already subscribed to: %s\n", subscribedTopic.c_str());
        return false;
    }

    subscribedTopic = topic;
    subscribedQos = qos;

    if (!connected)
    {
        Serial.printf("Could not subscribe to topic - not connected\n");
        return false;
    }

    bool success = mqttClient.subscribe(topic, qos);

    if (!success)
    {
        Serial.printf("Could not subscribe to topic - failed\n");
        mqttClient.disconnect();
        connected = false;
    }

    return success;
}

bool MQTTManager::unsubscribe(const char *topic)
{
    subscribedTopic = "";
    subscribedQos = 0;

    if (!connected)
    {
        return false;
    }

    return mqttClient.unsubscribe(topic);
}

void MQTTManager::handleJsonMessage(String &topic, String &payload)
{
    if (!jsonMessageHandler)
    {
        Serial.println(F("Warning: No JSON message handler has been registered"));
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    jsonMessageHandler(mqttClient, topic, doc);
}

void MQTTManager::onJsonMessage(void (*callback)(MQTTClient &, const String &, JsonDocument &))
{
    if (jsonMessageHandler)
    {
        Serial.println(F("Warning: Cannot set JSON message handler - a handler is already registered"));
        return;
    }

    jsonMessageHandler = callback;

    mqttClient.onMessage([this](String &topic, String &payload)
                         { this->handleJsonMessage(topic, payload); });
}