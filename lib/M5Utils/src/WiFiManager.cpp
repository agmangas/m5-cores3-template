#include "WiFiManager.h"

WiFiManager::WiFiManager()
    : ssid(WIFI_SSID), password(WIFI_PASSWORD), connected(false), lastReconnectAttempt(0)
{
}

bool WiFiManager::begin()
{
    WiFi.begin(ssid, password);

    int attempts = 0;

    while (WiFi.status() != WL_CONNECTED && attempts < connectionAttempts)
    {
        delay(connectionAttemptDelayMs);
        attempts++;
    }

    connected = (WiFi.status() == WL_CONNECTED);

    return connected;
}

void WiFiManager::update(bool tryToReconnect)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        connected = false;

        if (tryToReconnect)
        {
            unsigned long currentMillis = millis();

            if (currentMillis - lastReconnectAttempt >= reconnectIntervalMs)
            {
                lastReconnectAttempt = currentMillis;
                begin();
            }
        }
    }
    else
    {
        connected = true;
    }
}

bool WiFiManager::isConnected() const
{
    return connected;
}

String WiFiManager::getSSID() const
{
    return ssid;
}

String WiFiManager::getIP() const
{
    return WiFi.localIP().toString();
}

int WiFiManager::getRSSI() const
{
    return WiFi.RSSI();
}