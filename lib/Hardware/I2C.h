#ifndef CANE_I2C_H
#define CANE_I2C_H

#include <Arduino.h>
#include <Wire.h>

// probs didn't need a dual core processor, zomg the trouble it's caused lol
SemaphoreHandle_t i2c_mutex;

#define SAFE_I2C_CALL(code_block)                   \
   do                                               \
   {                                                \
      if (xSemaphoreTake(i2c_mutex, portMAX_DELAY)) \
      {                                             \
         code_block;                                \
         xSemaphoreGive(i2c_mutex);                 \
      }                                             \
   } while (0)

void setup_i2c()
{
   i2c_mutex = xSemaphoreCreateMutex();
}

// helper function to scan the i2c bus
void scan_i2c()
{
   Wire.begin(6, 7);
   for (byte addr = 1; addr < 127; addr++)
   {
      Wire.beginTransmission(addr);
      if (Wire.endTransmission() == 0)
      {
         Serial.print("Found I2C device at 0x");
         Serial.println(addr, HEX);
      }
   }
}

#endif