#define ROOM_NAME "TestRoom2"

#define numberOfActiveContactSensors 2  //Contact
//#define numberOfActiveRelays 1          //Relay
//#define numberOfActiveLights 1          //Relay
//#define numberOfActiveWaterMeters 2     //Termipol
#define TempHumActive 1                 //SHT-3x
#define LightSensorActive 1             //BH1750FVI

char* HumidityName = "Room2_Wilgotnisc";
char* TemperatureName = "Room2_Temperatura";
char* LightSensorName = "Room2_Jasnosc";

#if defined(numberOfActiveContactSensors)
kontaktron oKontaktron[] = {
    {22, false, "Room2_Motion", "motion", "mdi:motion-sensor"},
    {26, false, "Room2_Window", "window", ""}
};
#endif

#if defined(numberOfActiveRelays)
relay oRelay[] = {
    {4, "none"}
};
#endif

#if defined(numberOfActiveLights)
relay oLight[] = {
    {5, "none"}
};
#endif

#if defined(numberOfActiveWaterMeters)
// The digital input you attached your sensor.  (Only PIN 2, 3, 18, 19 generates interrupt!)
watermeter oWaterMeter[] = {
   watermeter(2, "none")
};
#endif

// Gateway IP address
#define BROKER_ADDR     IPAddress(192,168,0,90)
// Set the static IP address to use if the DHCP fails to assign
#define MYIPADDR 192,168,0,121
#define MYIPMASK 255,255,255,0
#define MYDNS 192,168,0,1
#define MYGW 192,168,0,1

// The MAC address can be anything you want but should be unique on your network.
// Newer boards have a MAC address printed on the underside of the PCB, which you can (optionally) use.
// Note that most of the Ardunio examples use  "DEAD BEEF FEED" for the MAC address.
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x91};
