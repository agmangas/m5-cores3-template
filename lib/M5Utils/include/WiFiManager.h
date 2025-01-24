#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "secrets.h"

class WiFiManager
{
private:
    const char *ssid;
    const char *password;
    bool connected;
    unsigned long lastReconnectAttempt;
    unsigned long reconnectIntervalMs = 30000;
    int connectionAttempts = 4;
    unsigned long connectionAttemptDelayMs = 500;

public:
    WiFiManager();

    bool begin();
    void update(bool tryToReconnect = true);
    bool isConnected() const;
    String getSSID() const;
    String getIP() const;
    int getRSSI() const;
};

#endif