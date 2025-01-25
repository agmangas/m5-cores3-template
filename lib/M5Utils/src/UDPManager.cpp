#include "UDPManager.h"

UDPManager::UDPManager(WiFiManager &wifiManager, uint16_t port)
    : wifiManager(wifiManager), localPort(port), initialized(false)
{
}

void UDPManager::stop()
{
    if (initialized)
    {
        udp.stop();
        initialized = false;
    }
}

void UDPManager::update()
{
    if (wifiManager.isConnected())
    {
        if (!initialized)
        {
            begin();
        }
    }
    else
    {
        // WiFi disconnected, cleanup UDP
        stop();
    }
}

bool UDPManager::begin()
{
    if (!wifiManager.isConnected())
    {
        return false;
    }

    // Stop any existing connection first
    stop();

    if (udp.begin(localPort))
    {
        initialized = true;
        return true;
    }

    return false;
}

bool UDPManager::send(const char *address, uint16_t port, const char *message)
{
    if (!initialized)
    {
        return false;
    }

    IPAddress targetIP;

    if (!targetIP.fromString(address))
    {
        return false;
    }

    return send(targetIP, port, message);
}

bool UDPManager::send(const char *address, uint16_t port, const uint8_t *buffer, size_t size)
{
    if (!initialized)
    {
        return false;
    }

    IPAddress targetIP;

    if (!targetIP.fromString(address))
    {
        return false;
    }

    return send(targetIP, port, buffer, size);
}

bool UDPManager::send(IPAddress address, uint16_t port, const char *message)
{
    if (!initialized)
    {
        return false;
    }

    return send(address, port, (const uint8_t *)message, strlen(message));
}

bool UDPManager::send(IPAddress address, uint16_t port, const uint8_t *buffer, size_t size)
{
    if (!initialized)
    {
        return false;
    }

    udp.beginPacket(address, port);
    udp.write(buffer, size);
    return udp.endPacket();
}

int UDPManager::parsePacket()
{
    if (!initialized)
    {
        return 0;
    }

    return udp.parsePacket();
}

int UDPManager::read(uint8_t *buffer, size_t maxSize)
{
    if (!initialized)
    {
        return 0;
    }

    return udp.read(buffer, maxSize);
}

String UDPManager::readString()
{
    if (!initialized)
    {
        return String();
    }

    return udp.readString();
}