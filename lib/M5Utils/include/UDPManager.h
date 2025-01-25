#ifndef UDP_MANAGER_H
#define UDP_MANAGER_H

#include <WiFiUdp.h>
#include <Arduino.h>
#include <IPAddress.h>
#include "WiFiManager.h"

class UDPManager
{
private:
    WiFiUDP udp;
    WiFiManager &wifiManager;
    uint16_t localPort;
    bool initialized;

    void stop();

public:
    UDPManager(WiFiManager &wifiManager, uint16_t port = 8888);

    bool begin();
    void update();
    bool isInitialized() const { return initialized; }

    // Send UDP packet
    bool send(const char *address, uint16_t port, const char *message);
    bool send(const char *address, uint16_t port, const uint8_t *buffer, size_t size);
    bool send(IPAddress address, uint16_t port, const char *message);
    bool send(IPAddress address, uint16_t port, const uint8_t *buffer, size_t size);

    // Receive UDP packet
    int parsePacket();
    int read(uint8_t *buffer, size_t maxSize);
    String readString();

    // Get sender information
    IPAddress remoteIP() { return udp.remoteIP(); }
    uint16_t remotePort() { return udp.remotePort(); }

    // Get local information
    uint16_t getLocalPort() const { return localPort; }
};

#endif