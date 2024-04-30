#include "settings.h"

#include "room_test1.h"
#include "room_test2.h"

IPAddress ip(MYIPADDR);
IPAddress dns(MYDNS);
IPAddress gw(MYGW);
IPAddress sn(MYIPMASK);

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device, 40);

//----------------------
#if defined(numberOfActiveWaterMeters)
HASensorNumber *oHAWaterMeter[numberOfActiveWaterMeters];
#endif
//----------------------

//----------------------
//--- Settings Lightsensor BH1750FVI
#if defined(LightSensorActive)
uint8_t ADDRESSPIN = 23;
BH1750FVI::eDeviceAddress_t DEVICEADDRESS = BH1750FVI::k_DevAddress_H;
BH1750FVI::eDeviceMode_t DEVICEMODE = BH1750FVI::k_DevModeContHighRes;

// Create the Lightsensor instance
BH1750FVI LightSensor(ADDRESSPIN, DEVICEADDRESS, DEVICEMODE);

long previousMillisLight = 0;
long lightReadInterval = 12000;

uint16_t actualLux = 0;

HASensorNumber HALightSensor(LightSensorName, HASensorNumber::PrecisionP2);
#endif
//--- Settings BH1750FVI
//----------------------

//----------------------
#if defined(TempHumActive)
SHTSensor sht(SHTSensor::SHT3X);

char bufferTmp[10];
int sensorMissing=1;

long previousMillis = 0;
long tempReadInterval = 10000;

float actualTemperature = 99.99;
float actualHumidity = 99.99;

HASensorNumber HATemperature(TemperatureName, HASensorNumber::PrecisionP2);
HASensorNumber HAHumidity(HumidityName, HASensorNumber::PrecisionP2);
#endif
//----------------------


//----------------------
unsigned long lastReadAt = millis();
bool allreadySent = false;

#define RELAY_ON LOW // GPIO value to write to turn on attached relay
#define RELAY_OFF HIGH // GPIO value to write to turn off attached relay
//----------------------

//----------------------
#if defined(numberOfActiveContactSensors)
HABinarySensor *oHABinarySensor[numberOfActiveContactSensors];
#endif
//----------------------

//----------------------
#if defined(numberOfActiveRelays)
HASwitch *oHASwitch[numberOfActiveRelays];
#endif

#if defined(numberOfActiveLights)
HALight *oHALight[numberOfActiveLights];
#endif
//----------------------

#if defined(numberOfActiveRelays)
void onSwitchCommand(bool state, HASwitch* sender)
{
    Serial.print("onSwitchCommand State: ");
    Serial.println(state);

    for (int nRelayNo = 0; nRelayNo <= numberOfActiveRelays-1; nRelayNo++) {
        if (strcmp(sender->uniqueId(), oRelay[nRelayNo].name) == 0) { // uniqueId is the name you passed to the constructor
            digitalWrite(oRelay[nRelayNo].pin, (state ? RELAY_ON : RELAY_OFF));
        }
    }

    sender->setState(state); // report state back to the Home Assistant
}
#endif

#if defined(numberOfActiveLights)
void onStateCommand(bool state, HALight* sender) {
    Serial.print("onStateCommand State: ");
    Serial.println(state);

    for (int nLightNo = 0; nLightNo <= numberOfActiveLights-1; nLightNo++) {
        if (strcmp(sender->uniqueId(), oLight[nLightNo].name) == 0) { // uniqueId is the name you passed to the constructor
            digitalWrite(oLight[nLightNo].pin, (state ? RELAY_ON : RELAY_OFF));
        }
    }

    sender->setState(state); // report state back to the Home Assistant
}
#endif

void setup() {
    Serial.begin(115200);
    delay(2000);                       // wait for a second
    Serial.println("Setup begins");

    // START WATCHDOG
    pinMode(LED_BUILTIN, OUTPUT);
    wdt_enable(WDTO_8S);
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(50);                       // wait for a second
    digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage LOW
    delay(50);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(50);                       // wait for a second
    digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage LOW
    delay(50);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(50);                       // wait for a second
    digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage LOW
    delay(50);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(50);                       // wait for a second
    // START WATCHDOG END

    // START SHT30
    #if defined(TempHumActive)
    Wire.begin();
    Serial.begin(115200);
    delay(1000); // let serial console settle

    if (sht.init()) {
      //Serial.print("sht30 init(): success\n");
      sensorMissing = 0;
      sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x
    } else {
      Serial.print("sht30 init(): failed\n");
      sensorMissing = 1;
    }
    HATemperature.setDeviceClass("temperature");
    HATemperature.setName(txt_temperature);
    HATemperature.setUnitOfMeasurement("°C");
    HAHumidity.setDeviceClass("humidity");
    HAHumidity.setName(txt_humidity);
    HAHumidity.setUnitOfMeasurement("%");
    #endif

    // START LIGHTSENSOR
    #if defined(LightSensorActive)
    LightSensor.begin();
    HALightSensor.setDeviceClass("illuminance");
    HALightSensor.setName(txt_light_intensity);
    HALightSensor.setUnitOfMeasurement("lx");
    #endif

    //START WATER METER
    #if defined(numberOfActiveWaterMeters)
    for (int waterSensorNo = 0; waterSensorNo <= numberOfActiveWaterMeters-1; waterSensorNo++) {
      oHAWaterMeter[waterSensorNo] = new HASensorNumber(oWaterMeter[waterSensorNo].getName(), HASensorNumber::PrecisionP2);
      oHAWaterMeter[waterSensorNo]->setDeviceClass("water");
      oHAWaterMeter[waterSensorNo]->setName(oWaterMeter[waterSensorNo].getName());
      oHAWaterMeter[waterSensorNo]->setUnitOfMeasurement("l/mn");
      oWaterMeter[waterSensorNo].checkWaterMeter();

      if(waterSensorNo == 0) {
        attachInterrupt(digitalPinToInterrupt(oWaterMeter[waterSensorNo].getAssignedPin()), onPulse1, FALLING);
      }
      if(waterSensorNo == 1) {
        attachInterrupt(digitalPinToInterrupt(oWaterMeter[waterSensorNo].getAssignedPin()), onPulse2, FALLING);
      }
      if(waterSensorNo == 2) {
        attachInterrupt(digitalPinToInterrupt(oWaterMeter[waterSensorNo].getAssignedPin()), onPulse3, FALLING);
      }
      if(waterSensorNo == 3) {
        attachInterrupt(digitalPinToInterrupt(oWaterMeter[waterSensorNo].getAssignedPin()), onPulse4, FALLING);
      }
    }
    #endif

    //EEPROM.begin(512);

    //Ethernet.init(17); // Raspberry Pi Pico with w5500
    //Ethernet.init(48); // Easyswitch with w5500
    Ethernet.init();

    Ethernet.begin(mac, ip, dns, gw, sn);
    Serial.println("LAN OK!");

    Serial.print("Local IP : ");
    Serial.println(Ethernet.localIP());
    Serial.print("Subnet Mask : ");
    Serial.println(Ethernet.subnetMask());
    Serial.print("Gateway IP : ");
    Serial.println(Ethernet.gatewayIP());
    Serial.print("DNS Server : ");
    Serial.println(Ethernet.dnsServerIP());

    // optional device's details
    device.setSoftwareVersion("1.0.0");
    //device.setName(ROOM_NAME);
    device.setName(strcat("Multisensor ", ROOM_NAME));

    #if defined(numberOfActiveContactSensors)
    oHABinarySensorInit();
    #endif

    #if defined(numberOfActiveRelays)
    oHASwitchInit();
    #endif

    #if defined(numberOfActiveLights)
    oHALightInit();
    #endif

    Serial.println("setup OK");
    wdt_reset();
    delay(1000);
    wdt_reset();
    delay(1000);
    wdt_reset();
    Serial.println("wait OK");

    Serial.println("connecting to MQTT broker...");
}

#if defined(numberOfActiveWaterMeters)
void IRQ_HANDLER_ATTR onPulse1()
{
  oWaterMeter[0].onPulse();
}

void IRQ_HANDLER_ATTR onPulse2()
{
  oWaterMeter[1].onPulse();
}

void IRQ_HANDLER_ATTR onPulse3()
{
  oWaterMeter[2].onPulse();
}

void IRQ_HANDLER_ATTR onPulse4()
{
  oWaterMeter[3].onPulse();
}
#endif

#if defined(numberOfActiveContactSensors)
void oHABinarySensorInit()
{
  for (int binSensorNo = 0; binSensorNo <= numberOfActiveContactSensors-1; binSensorNo++) {
    oHABinarySensor[binSensorNo] = new HABinarySensor(oKontaktron[binSensorNo].name);

    pinMode(oKontaktron[binSensorNo].pin, INPUT_PULLUP);
    oKontaktron[binSensorNo].lastInputState = digitalRead(oKontaktron[binSensorNo].pin);

    oHABinarySensor[binSensorNo]->setCurrentState(oKontaktron[binSensorNo].lastInputState);
    oHABinarySensor[binSensorNo]->setName(oKontaktron[binSensorNo].name);
    oHABinarySensor[binSensorNo]->setDeviceClass(oKontaktron[binSensorNo].deviceClass);
    Serial.print("setup binSensor ");
    Serial.print(binSensorNo);
    Serial.print(" uniqueId ");
    Serial.println(oHABinarySensor[binSensorNo]->uniqueId());
    if (strcmp(oKontaktron[binSensorNo].icon , "") == 0) { // uniqueId is the name you passed to the constructor
      Serial.println("No icon specified");
    }
    else {
      oHABinarySensor[binSensorNo]->setIcon(oKontaktron[binSensorNo].icon);
    }
    Serial.print("\n");
  }
}
#endif

#if defined(numberOfActiveRelays)
void oHASwitchInit()
{
    for (int nRelayNo = 0; nRelayNo <= numberOfActiveRelays-1; nRelayNo++) {
        oHASwitch[nRelayNo] = new HASwitch(oRelay[nRelayNo].name);
Serial.print("setup nRelay ");
Serial.print(nRelayNo);
Serial.print(" uniqueId ");
Serial.println(oHASwitch[nRelayNo]->uniqueId());
Serial.print("\n");
        pinMode(oRelay[nRelayNo].pin, OUTPUT);
        digitalWrite(oRelay[nRelayNo].pin, RELAY_OFF);

        // set icon (optional)
        oHASwitch[nRelayNo]->setName(oRelay[nRelayNo].name);

        // handle switch state
        oHASwitch[nRelayNo]->onCommand(onSwitchCommand);
    }
}
#endif

#if defined(numberOfActiveLights)
void oHALightInit()
{
    for (int nLightNo = 0; nLightNo <= numberOfActiveLights-1; nLightNo++) {
        oHALight[nLightNo] = new HALight(oLight[nLightNo].name);
Serial.print("setup nLight ");
Serial.print(nLightNo);
Serial.print(" uniqueId ");
Serial.println(oHALight[nLightNo]->uniqueId());
Serial.print("\n");
        pinMode(oLight[nLightNo].pin, OUTPUT);
        digitalWrite(oLight[nLightNo].pin, RELAY_OFF);

        // set icon (optional)
        oHALight[nLightNo]->setName(oLight[nLightNo].name);

        // handle light state
        oHALight[nLightNo]->onStateCommand(onStateCommand);
    }
}
#endif

int mqttMaintainConnection() {
  if(Ethernet.linkStatus() == 2) {
    Serial.println("ERROR: Cable disconnected!!!");
    delay(1000);
  }
  else {
    if (Ethernet.maintain() % 2 == 1) {
      Serial.println("ERROR: DHCP server hosed!!!");
      delay(1000);
    }
    else {
      //(re)connect to MQTT Broker
      while(!mqtt.isConnected()) {
        Serial.println("MQTT... FAIL!!!");
        wdt_reset();
        delay(1000);
        mqtt.begin(BROKER_ADDR);
        mqtt.loop();
      }
      Serial.println("MQTT... OK");
      return 1;
    }
  }
  return 0;
}

void loop() {
    if(mqttMaintainConnection()) {
      digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage LOW
      mqtt.loop();
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
      delay(100);
    }

    // RESET WATCHDOG TIMER
    wdt_reset();

    //KONTAKTRON

    for (int binSensorNo = 0; binSensorNo <= numberOfActiveContactSensors-1; binSensorNo++) {
      Serial.print("binSensor ");
      Serial.print(binSensorNo);
      Serial.print(" uniqueId ");
      Serial.println(oHABinarySensor[binSensorNo]->uniqueId());
      Serial.print("\n");
    }

    #if defined(numberOfActiveContactSensors)
    if ((millis() - lastReadAt) > 300) { // read in 30ms interval

        // library produces MQTT message if a new state is different than the previous one
        for (int binSensorNo = 0; binSensorNo <= numberOfActiveContactSensors-1; binSensorNo++) {
            oHABinarySensor[binSensorNo]->setState(digitalRead(oKontaktron[binSensorNo].pin));
            oKontaktron[binSensorNo].lastInputState = oHABinarySensor[binSensorNo]->getCurrentState();
        }

        lastReadAt = millis();
    }
    #endif

    // RESET WATCHDOG TIMER
    wdt_reset();

    //TEMPERATURE
    unsigned long currentMillis = millis();
    if( sensorMissing == 0 ) {
      if(currentMillis - previousMillis > tempReadInterval) {
        Serial.print("\n readTemp.. ");
        previousMillis = currentMillis;

        readTemp();
      }
    }

    // RESET WATCHDOG TIMER
    wdt_reset();

    //LIGHTSENSOR
    #if defined(LightSensorActive)
    if(currentMillis - previousMillisLight > lightReadInterval) {
      previousMillisLight = currentMillis;
      Serial.print("\n readLight.. ");

      actualLux = LightSensor.GetLightIntensity();

      HALightSensor.setValue(actualLux);
      Serial.print("Light: ");
      Serial.println(actualLux);
    }
    #endif

    // RESET WATCHDOG TIMER
    wdt_reset();

    #if defined(numberOfActiveWaterMeters)
    for (int waterSensorNo = 0; waterSensorNo <= numberOfActiveWaterMeters-1; waterSensorNo++) {
      oWaterMeter[waterSensorNo].checkWaterMeter();
      oHAWaterMeter[waterSensorNo]->setValue(oWaterMeter[waterSensorNo].getActualFlow()); // Send flow value to gw
    }
    #endif

    // RESET WATCHDOG TIMER
    wdt_reset();
}

/** ----------- **/
/** SENSOR READ **/
/** ----------- **/
#if defined(TempHumActive)
void readTemp()
{
  //Serial.print("Reading SHT30 Data...\n");
  //Serial.print("SHT:\n");
  //Serial.print("  RH: ");

  if (sht.readSample()) {
    actualHumidity = sht.getHumidity();
    Serial.print(actualHumidity, 2);
    Serial.print("\n");
    Serial.print("  T:  ");

    actualTemperature = sht.getTemperature();
    Serial.print(actualTemperature, 2);
    Serial.print("\n");
    Serial.print("Reading SHT30 Data...OK\n");
  } else {
      Serial.print("Error in readSample()\n");
  }

  if (! isnan(actualHumidity)) {  // check if 'is not a number'
      HAHumidity.setValue(actualHumidity);
      //Serial.print("SENT SHT30 HAHumidity Data...OK\n");
  }
  if (! isnan(actualTemperature)) {  // check if 'is not a number'
      HATemperature.setValue(actualTemperature);
      //Serial.print("SENT SHT30 HATemperature Data...OK\n");
  }
}
#endif
