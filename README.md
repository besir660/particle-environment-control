# particle-environment-control

Code for particle photon to control multiple devices through a blynk project. The code here provides remote control for 4 devices and is designed for a boat, but this can be customized for a general setting where you want to control multiple devices. In this project 4 devices are controlled:

-Heater

-Dehumidifier

-Fridge 

-Bilge pump

The device objects are defined in the device.h and .cpp files. The instance of a device class takes the output photon pin to control the physical device as an argument. The pins controls a relay for the the conncected device. The devices have manual and automatic modes. In the automatic mode, the operation is triggered by firing of a homeostat device. To do this, the homeostat's output method is called by the relevant environment variable, and this method will return 0 or 1 based on the working principle (suppress or elevate - see below), and the level of the environment variable. In this project, the blynk interface has a selection button to choose between heater and dehumidifier so they can be controlled using the same target/hysteresis sliders. 

The "homeostat" class is provided for automatic control of a device (heater and dehumidifier in this project, but any device is eligible of course, i.e bilge pump can be connected by a water level sensor). This enables the devices to work in a suppression or elevation mode (i.e. the dehumidifier works in suppressive mode, it kicks in at the higher threshold to lower the humidity and cuts out at the lower threshold, and the heater does the opposite) 

The environment variables of temperature and humidity is collected from the cloud by subscribing to the events published by another particle photon in the environment. I chose for this option as the control device is in an instrument cabinet, and the environment readings from a sensor in this location wouldn't properly reflect the actual environment. 

The connected virtual blynk pins are self-evident from the code, these are arbitrary and can be customized.

Please note that you need to provide your blynk authentication token, and wi-fi credentials in the relevant places in the code.
