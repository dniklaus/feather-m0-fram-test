/*
 * wiring-skeleton.cpp
 *
 *  Created on: 15.03.2017
 *      Author: niklausd
 */

#include <Arduino.h>

// PlatformIO libraries
#include <SerialCommand.h>  // pio lib install 173, lib details see https://github.com/kroimon/Arduino-SerialCommand
#include <Timer.h>          // pio lib install 1699, lib details see https://github.com/dniklaus/wiring-timer

// private libraries
#include <DbgCliNode.h>
#include <DbgCliTopic.h>
#include <DbgCliCommand.h>
#include <DbgTracePort.h>
#include <DbgTraceContext.h>
#include <DbgTraceOut.h>
#include <DbgPrintConsole.h>
#include <DbgTraceLevel.h>
#include <AppDebug.h>
#include <ProductDebug.h>
#include <RamUtils.h>
#include <Wire.h>
#include <Adafruit_FRAM_I2C.h>
#include <Timer.h>

#ifndef BUILTIN_LED
#define BUILTIN_LED 13
#endif

SerialCommand* sCmd = 0;

//-----------------------------------------------------------------------------

class HeartBeatTimerAdapter : public TimerAdapter
{
private:
  bool m_state;
public:
  HeartBeatTimerAdapter()
  : m_state(false)
  { }

  void timeExpired()
  {
    m_state = !m_state;
    digitalWrite(BUILTIN_LED, m_state);
  }
};

//-----------------------------------------------------------------------------

Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C();

void setup()
{
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, 0);

  setupProdDebugEnv();

  new Timer(new HeartBeatTimerAdapter(), Timer::IS_RECURRING, 500);

  delay(2000);


  Wire.begin();

  if (fram.begin()) // you can stick the new i2c addr in here, e.g. begin(0x51);
  {
    Serial.println("Found I2C FRAM");
  }
  else
  {
    Serial.println("No I2C FRAM found ... check your connections\r\n");
    while (1)
      ;
  }

  // Read the first byte
  uint8_t test = fram.read8(0x0);
  Serial.print("Restarted ");
  Serial.print(test);
  Serial.println(" times");

  // Test write ++
  fram.write8(0x0, test + 1);
}

void loop()
{
  if (0 != sCmd)
  {
    sCmd->readSerial();     // process serial commands
  }
  yield();                  // process Timers
}
