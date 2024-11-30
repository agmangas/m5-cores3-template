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
  M5.Display.setCursor(0, 20);
  M5.Display.printf("COUNT: %d\n", count);
  Serial.printf("COUNT: %d\n", count);
  count++;
  delay(1000);
}
