#define ROOM_NAME "TestRoom1"

#define numberOfActiveContactSensors 3  //Contact
#define numberOfActiveRelays 1          //Relay
#define numberOfActiveLights 1          //Relay
#define numberOfActiveWaterMeters 2     //Termipol
#define TempHumActive 1                 //SHT-3x
#define LightSensorActive 1             //BH1750FVI

char* HumidityName = "Room1_Wilgotnisc";
char* TemperatureName = "Room1_Temperatura";
char* LightSensorName = "Room1_Jasnosc";

#if defined(numberOfActiveContactSensors)
kontaktron oKontaktron[] = {
    {22, false, "Room1_Motion", "motion", "mdi:motion-sensor"},
    {24, false, "Room1_Door", "door", "mdi:door"},
    {26, false, "Room1_Window", "window", ""}
};
#endif

#if defined(numberOfActiveRelays)
relay oRelay[] = {
    {4, "Room1_Relay"}
};
#endif

#if defined(numberOfActiveLights)
relay oLight[] = {
    {5, "Room1_Light"}
};
#endif

#if defined(numberOfActiveWaterMeters)
// The digital input you attached your sensor.  (Only PIN 2, 3, 18, 19 generates interrupt!)
watermeter oWaterMeter[] = {
   watermeter(2, "Room1_Cold"),
   watermeter(3, "Room1_Warm")
};
#endif

// Gateway IP address
#define BROKER_ADDR     IPAddress(192,168,0,90)
// Set the static IP address to use if the DHCP fails to assign
#define MYIPADDR 192,168,0,120
#define MYIPMASK 255,255,255,0
#define MYDNS 192,168,0,1
#define MYGW 192,168,0,1

// The MAC address can be anything you want but should be unique on your network.
// Newer boards have a MAC address printed on the underside of the PCB, which you can (optionally) use.
// Note that most of the Ardunio examples use  "DEAD BEEF FEED" for the MAC address.
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x90};
