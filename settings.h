#include <Ethernet.h>
#include <ArduinoHA.h>
#include <Watchdog.h>
#include "SHTSensor.h"
#include <Wire.h>
#include <BH1750FVI.h>

#define txt_temperature "Temperature"
#define txt_humidity "Humidity"
#define txt_light_intensity "Light intensity"

//#define ARDUINOHA_DEBUG
#define EX_ARDUINOHA_BUTTON
#define EX_ARDUINOHA_CAMERA
#define EX_ARDUINOHA_COVER
#define EX_ARDUINOHA_DEVICE_TRACKER
#define EX_ARDUINOHA_DEVICE_TRIGGER
#define EX_ARDUINOHA_FAN
#define EX_ARDUINOHA_HVAC
#define EX_ARDUINOHA_NUMBER
#define EX_ARDUINOHA_SCENE
#define EX_ARDUINOHA_SELECT
#define EX_ARDUINOHA_SENSOR
#define EX_ARDUINOHA_TAG_SCANNER
#define PULSE_FACTOR 574                       // Number of blinks per m3 of your meter (One rotation/liter)
#define MAX_FLOW 30                             // Max flow (l/min) value to report. This filters outliers.

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
#define IRQ_HANDLER_ATTR ICACHE_RAM_ATTR
#else
#define IRQ_HANDLER_ATTR
#endif

typedef struct {
  int pin;
  bool lastInputState;
  char* name;
  char* deviceClass;
  char* icon;
} kontaktron;

typedef struct {
  int pin;
  char* name;
} relay;

class watermeter {
  int WATER_METER_PIN; // The digital input you attached your sensor.  (Only 2, 3, 18, 19 generates interrupt!)
  char* WATER_METER_NAME;
  double lastWaterMeterValue = 0;

  uint32_t SEND_FREQUENCY = 1000;           // Minimum time between send (in milliseconds). We don't want to spam the gateway.

  double ppl = ((double)PULSE_FACTOR)/1000;        // Pulses per liter
  volatile uint32_t pulseCount = 0;
  volatile uint32_t lastBlink = 0;
  volatile double flow = 0;
  uint32_t oldPulseCount = 0;
  double oldflow = 0;
  double oldvolume =0;
  uint32_t lastSend =0;
  uint32_t lastPulse =0;
  float actualFlow = 0.00;

  public:
  watermeter(int waterMeterPin, char* waterMeterName) : WATER_METER_PIN(waterMeterPin), WATER_METER_NAME(waterMeterName)
  {
    // initialize our digital pins internal pullup resistor so one pulse switches from high to low (less distortion)
    pinMode(waterMeterPin, INPUT_PULLUP);
    pulseCount = oldPulseCount = 0;
    lastSend = lastPulse = millis();
  }

  void onPulse()
  {
  	uint32_t newBlink = micros();
  	uint32_t interval = newBlink-lastBlink;

  	if (interval!=0) {
  		lastPulse = millis();
  		if (interval<500000L) {
  			// Sometimes we get interrupt on RISING,  500000 = 0.5 second debounce ( max 120 l/min)
  			return;
  		}
  		flow = (60000000.0 /interval) / ppl;
  	}
  	lastBlink = newBlink;
  	pulseCount++;
  }

  char* getName()
  {
    return WATER_METER_NAME;
  }

  float getActualFlow()
  {
    return actualFlow;
  }

  int getAssignedPin()
  {
    return WATER_METER_PIN;
  }

  void checkWaterMeter()
  {
  	uint32_t currentTime = millis();

  	// Only send values at a maximum frequency or woken up from sleep
  	if (currentTime - lastSend > SEND_FREQUENCY) {
  		lastSend=currentTime;

  		if (flow != oldflow) {
  			oldflow = flow;

  			Serial.print("l/min:");
  			Serial.println(flow);

            actualFlow = flow;
   		}

  		// No Pulse count received in x sec
  		if(currentTime - lastPulse > 1000) {
  		  flow = 0;
        actualFlow = flow;
  		}

  		// Pulse count has changed
  		if (pulseCount != oldPulseCount) {
  			oldPulseCount = pulseCount;

  			Serial.print("pulsecount:");
  			Serial.println(pulseCount);

  			double volume = ((double)pulseCount/((double)PULSE_FACTOR));
  			if (volume != oldvolume) {
  				oldvolume = volume;

  				Serial.print("volume:");
  				Serial.println(volume, 3);
  			}
  		}
  	}
  }

};
