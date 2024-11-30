/*
 * M5Unified is a unified library that supports multiple M5Stack devices
 * (like M5Stack, M5Core2, M5StickC, etc.) with a common API.
 * Unlike board-specific libraries (e.g., M5Stack.h, M5Core2.h),
 * M5Unified allows the same code to work across different M5 devices
 * without modification, making code more portable and maintainable.
 */
#include <M5Unified.h>

uint32_t count;

void setup()
{
  // M5.config() provides platform-specific initialization settings
  auto cfg = M5.config();
  M5.begin(cfg);

  M5.Display.setTextSize(3);
  M5.Display.print("Hello World!!!");
  Serial.println("Hello World!!!");
  count = 0;
}

void loop()
{
  M5.update();        // Required to update battery status
  M5.Display.clear(); // Clear the display before drawing new content

  float batteryLevel = M5.Power.getBatteryLevel();
  bool isCharging = M5.Power.isCharging(); // Check if the device is charging

  M5.Display.setCursor(0, 20);
  M5.Display.printf("COUNT: %d\n", count);
  M5.Display.printf("BATT: %.1f%%\n", batteryLevel);
  M5.Display.printf("CHRG: %s\n", isCharging ? "Yes" : "No");

  Serial.printf("COUNT: %d\n", count);
  Serial.printf("BATT: %.1f%%\n", batteryLevel);
  Serial.printf("CHRG: %s\n", isCharging ? "Yes" : "No");

  count++;
  count = count % 100;
  delay(1000);
}
