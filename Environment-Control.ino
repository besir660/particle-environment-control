

// This #include statement was automatically added by the Particle IDE.
#include "myTimer.h"

// This #include statement was automatically added by the Particle IDE.
#include <blynk.h>


// This #include statement was automatically added by the Particle IDE.
#include "Device.h"


// This #include statement was automatically added by the Particle IDE.
//#include <Wire.h>


SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(SEMI_AUTOMATIC);

int subscriptionHandler(const char *event, const char *data); // forward declaration
int HumidityEventHandler(const char *data); // forward declaration
int TemperatureEventHandler(const char *data); // forward declaration
int menu_selection = 1 ;


//BLYNK Tokens:
//be careful to have the correct Blynk token for device before flash to prevent multiple devices updating the same stream
char auth[] = "your-blynk-token";

float Temperature= 20.0;
float Humidity= 0.0;
float dew_point = 0.0;
float Target = 6;

myTimer Environment_timer(10000);
myTimer UI_timer(10000);
WidgetLED bilgeLed(V10); 
WidgetLED FridgeLed(V12);

DeviceMain Heater(6);
DeviceMain Dehumidifier(5);
DeviceMain BilgePump(4);
DeviceMain Fridge(3);
Homeostat Thermostat(0,20, 5.0); //mode 1 for humistat 0 for thermostat
Homeostat Humidistat(1,40, 5.0);

void setup() 
{
  setupWifi();
  Particle.subscribe("Temperature", subscriptionHandler, MY_DEVICES);
  Particle.subscribe("Humidity", subscriptionHandler, MY_DEVICES);
  //Particle.function("Temperature", tempHandler); //debug function to manually adjust temperature, comment out temperature subscription above if using this
  Wire.begin();
  Blynk.begin(auth);
  bilgeLed.off();
  FridgeLed.off();
  //Time.zone(2);
  Blynk.virtualWrite(V22, menu_selection);
  Blynk.virtualWrite(V16, Time.format(Time.now(),"%d/%m/%y %H:%M:%S"));
  BlynkUI_Initialize();
  Particle.process();
 }

void loop() 
{
    if (!Particle.connected()){
        Particle.connect();
    }
    Blynk.run();
    if (Environment_timer.trigger()) EnvironmentControl();
    if (UI_timer.trigger()) BlynkUI_Update();
    Particle.process();
}

void setupWifi() {
  WiFi.on();
  WiFi.disconnect();
  WiFi.clearCredentials();
  WiFi.setCredentials("network-id", "password", WPA2, WLAN_CIPHER_AES);
  WiFi.connect();
  waitUntil(WiFi.ready);
  Particle.connect();
}



void BlynkUI_Initialize()
{
// You can send any value at any time.
// Please don't send more that 10 values per second.
    Blynk.virtualWrite(V19, Fridge.isSwitchOn());
    Blynk.virtualWrite(V21, BilgePump.isSwitchOn());
    Blynk.virtualWrite(V16,Time.format(Time.local(),"%d/%m/%y %H:%M:%S"));
    if (menu_selection == 1)
    {
        Blynk.virtualWrite(V5, Heater.isManual());
        Blynk.virtualWrite(V6,Heater.isSwitchOn());
        Blynk.virtualWrite(V1,Thermostat.ReadHysteresis());
        Blynk.virtualWrite(V0, Thermostat.ReadTarget());
        Blynk.virtualWrite(V2, Thermostat.ReadLow());
        Blynk.virtualWrite(V4,Thermostat.ReadHigh());
        Blynk.virtualWrite(V3,FormatSecs(Heater.WorkTime()));
        Blynk.virtualWrite(V15,FormatSecs(Heater.TotalWorkTime()));
    }
    else if (menu_selection == 2)
    {
        Blynk.virtualWrite(V5, Dehumidifier.isManual());
        Blynk.virtualWrite(V6,Dehumidifier.isSwitchOn());
        Blynk.virtualWrite(V1,Humidistat.ReadHysteresis());
        Blynk.virtualWrite(V0, Humidistat.ReadTarget());
        Blynk.virtualWrite(V2, Humidistat.ReadLow());
        Blynk.virtualWrite(V4,Humidistat.ReadHigh());
        Blynk.virtualWrite(V3,FormatSecs(Dehumidifier.WorkTime()));
        Blynk.virtualWrite(V15,FormatSecs(Dehumidifier.TotalWorkTime()));
    }
    Blynk.virtualWrite(V19, Fridge.isSwitchOn());
    Blynk.virtualWrite(V21, BilgePump.isSwitchOn());
}


void BlynkUI_Update()
{
// You can send any value at any time.
// Please don't send more that 10 values per second.
    Blynk.virtualWrite(V11, FormatSecs(BilgePump.WorkTime()));
    Blynk.virtualWrite(V18, FormatSecs(Fridge.WorkTime()));
    Blynk.virtualWrite(V8, Humidity);
    Blynk.virtualWrite(V7, Temperature);
    dew_point = Temperature - ((100 - Humidity)/5.0);
    Blynk.virtualWrite(V9, dew_point);
    if (menu_selection == 1)
    {
        Blynk.virtualWrite(V3,FormatSecs(Heater.WorkTime()));
        Blynk.virtualWrite(V15,FormatSecs(Heater.TotalWorkTime()));
    }
    else if (menu_selection == 2)
    {
        Blynk.virtualWrite(V3,FormatSecs(Dehumidifier.WorkTime()));
        Blynk.virtualWrite(V15,FormatSecs(Dehumidifier.TotalWorkTime()));
    }
}

String FormatSecs(int worksecs)
{
    int workhours = worksecs/3600;
    int workmins = (worksecs-workhours*3600)/60;
    worksecs = worksecs - (workhours*3600+workmins*60);
    return String::format("%04d:%02d:%02d", workhours,workmins, worksecs);
    }
    
void EnvironmentControl()
{
    if (!Heater.isManual()) Heater.Switch(Thermostat.Output(Temperature));
    if (!Dehumidifier .isManual()) Dehumidifier.Switch(Humidistat.Output(Humidity));
    if (menu_selection ==1) Blynk.virtualWrite(V6,Heater.isSwitchOn());
    else if (menu_selection ==2) Blynk.virtualWrite(V6,Dehumidifier.isSwitchOn());
}


int subscriptionHandler(const char *event, const char *data) {
	Serial.printlnf("subscriptionHandler event=%s data=%s", event, data);
	if ( strcmp(event, "Temperature") == 0){
	    return TemperatureEventHandler(data);
	} else if ( strcmp(event, "Humidity") == 0){
	    return HumidityEventHandler(data);
	}
	return 0;
}

int HumidityEventHandler(const char *data) {
	Humidity = atof(data);
	return 0;
}

int TemperatureEventHandler(const char *data) {
	Temperature = atof(data);
	return 0;
}

/* debug function to manually adjust temperature
int tempHandler(String data) {
    Temperature = data.toFloat();
    return 1;
}
*/

BLYNK_WRITE(V22) //selection for heater/dehumidifer
{
    int pinValue = param.asInt(); 
    menu_selection = pinValue;
    if (menu_selection == 1)
    {
        Blynk.virtualWrite(V5, Heater.isManual());
        Blynk.virtualWrite(V6,Heater.isSwitchOn());
        Blynk.virtualWrite(V1,Thermostat.ReadHysteresis());
        Blynk.virtualWrite(V0, Thermostat.ReadTarget());
        Blynk.virtualWrite(V2, Thermostat.ReadLow());
        Blynk.virtualWrite(V4,Thermostat.ReadHigh());
        Blynk.virtualWrite(V3,FormatSecs(Heater.WorkTime()));
        Blynk.virtualWrite(V15,FormatSecs(Heater.TotalWorkTime()));
    }
    else if (menu_selection == 2)
    {
        Blynk.virtualWrite(V5, Dehumidifier.isManual());
        Blynk.virtualWrite(V6,Dehumidifier.isSwitchOn());
        Blynk.virtualWrite(V1,Humidistat.ReadHysteresis());
        Blynk.virtualWrite(V0, Humidistat.ReadTarget());
        Blynk.virtualWrite(V2, Humidistat.ReadLow());
        Blynk.virtualWrite(V4,Humidistat.ReadHigh());
        Blynk.virtualWrite(V3,FormatSecs(Dehumidifier.WorkTime()));
        Blynk.virtualWrite(V15,FormatSecs(Dehumidifier.TotalWorkTime()));
    }
}

BLYNK_WRITE(V0) //target slider
{
  double pinValue = param.asDouble(); // assigning incoming value from pin V0 to a variable
  if (menu_selection ==1)
  {
    Thermostat.Target(pinValue);  
    Blynk.virtualWrite(V2, Thermostat.ReadLow());
    Blynk.virtualWrite(V4,Thermostat.ReadHigh());
  }
  else if (menu_selection ==2)
  {
    Humidistat.Target(pinValue);  
    Blynk.virtualWrite(V2, Humidistat.ReadLow());
    Blynk.virtualWrite(V4,Humidistat.ReadHigh());
  }
}

BLYNK_WRITE(V1) //hysteresis value
{
  double pinValue = param.asDouble(); // assigning incoming value from pin V1 to a variable
  
  if (menu_selection ==1)
  {
    Thermostat.Hysteresis(pinValue);  
    Blynk.virtualWrite(V2, Thermostat.ReadLow());
    Blynk.virtualWrite(V4,Thermostat.ReadHigh());
  }
  else if (menu_selection ==2)
  {
    Humidistat.Hysteresis(pinValue);  
    Blynk.virtualWrite(V2, Humidistat.ReadLow());
    Blynk.virtualWrite(V4,Humidistat.ReadHigh());
  }
}


BLYNK_WRITE(V6) //switch on/off button
{
  int pinValue = param.asInt(); 
  if (menu_selection ==1)
  {
    if (Heater.isManual()) Heater.Switch(pinValue);
    else Blynk.virtualWrite(V6, (1-pinValue));
  }
  else if (menu_selection ==2)
  {
    if (Dehumidifier .isManual()) Dehumidifier.Switch(pinValue);
    else Blynk.virtualWrite(V6, (1-pinValue));
  }
  }

BLYNK_WRITE(V5) //manual override button
{
  int pinValue = param.asInt(); 
  if (menu_selection ==1) Heater.Manual(pinValue);
  if (menu_selection ==2) Dehumidifier.Manual(pinValue);
}


BLYNK_WRITE(V19) //Fridge power button
{
  int pinValue = param.asInt(); 
  Fridge.Switch(pinValue);
  if (pinValue == 1) FridgeLed.on();
  else FridgeLed.off();
}

BLYNK_WRITE(V21) //Bilge Pump power button
{
  int pinValue = param.asInt(); 
  BilgePump.Switch(pinValue);
  if (pinValue == 1) bilgeLed.on();
  else bilgeLed.off();
}

BLYNK_WRITE(V14) //RESET
{
  int pinValue = param.asInt(); 
  if (pinValue == 1) System.reset();
}

