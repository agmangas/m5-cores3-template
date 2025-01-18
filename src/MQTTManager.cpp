#include "MQTTManager.h"

MQTTManager::MQTTManager(WiFiManager &wifiManager)
    : wifiManager(wifiManager), broker(MQTT_BROKER), port(MQTT_PORT), clientId(MQTT_CLIENT_ID), connected(false), lastReconnectAttempt(0), mqttClient(wifiClient)
{
}

bool MQTTManager::begin()
{
    if (!wifiManager.isConnected())
    {
        return false;
    }

    mqttClient.setId(clientId);
    mqttClient.setKeepAliveInterval(keepAliveIntervalMs);

    int attempts = 0;

    while (!mqttClient.connect(broker, port) && attempts < connectionAttempts)
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
        mqttClient.poll();
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

    return mqttClient.beginMessage(topic) &&
           mqttClient.print(payload) &&
           mqttClient.endMessage();
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

    mqttClient.subscribe(topic, qos);
    return true;
}

bool MQTTManager::unsubscribe(const char *topic)
{
    if (!connected)
    {
        return false;
    }

    mqttClient.unsubscribe(topic);
    return true;
}

void MQTTManager::handleJsonMessage(int messageSize)
{
    if (!jsonMessageHandler)
    {
        Serial.println(F("Warning: No JSON message handler has been registered"));
        return;
    }

    String payload;

    while (mqttClient.available())
    {
        payload += (char)mqttClient.read();
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    jsonMessageHandler(mqttClient, mqttClient.messageTopic(), doc);
}

void MQTTManager::onJsonMessage(void (*callback)(MqttClient &, const String &, JsonDocument &))
{
    if (jsonMessageHandler)
    {
        Serial.println(F("Warning: Cannot set JSON message handler - a handler is already registered"));
        return;
    }

    jsonMessageHandler = callback;

    // Warning: Static instance risks dangling pointer if MQTTManager instance is destroyed
    // or callback redirection if new MQTTManager instance created
    static MQTTManager *instance = this;

    mqttClient.onMessage([](int messageSize)
                         { instance->handleJsonMessage(messageSize); });
}