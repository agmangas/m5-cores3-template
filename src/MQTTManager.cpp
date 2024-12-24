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
