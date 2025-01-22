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

    return mqttClient.publish(topic, payload);
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
    if (!connected)
    {
        return false;
    }

    return mqttClient.subscribe(topic, qos);
}

bool MQTTManager::unsubscribe(const char *topic)
{
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