#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

// WiFi network name and password:
const char * networkName = "***yourSSID***";
const char * networkPswd = "***yourPW***";

const char * udpAddress = "***yourIPadress***";
const int udpPort = 1111;

//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udp;

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
 
void displaySensorDetails(void)
{
 sensor_t sensor;
 accel.getSensor(&sensor);
 Serial.println("------------------------------------");
 Serial.print ("Sensor: "); Serial.println(sensor.name);
 Serial.print ("Driver Ver: "); Serial.println(sensor.version);
 Serial.print ("Unique ID: "); Serial.println(sensor.sensor_id);
 Serial.print ("Max Value: "); Serial.print(sensor.max_value); Serial.println(" m/s^2");
 Serial.print ("Min Value: "); Serial.print(sensor.min_value); Serial.println(" m/s^2");
 Serial.print ("Resolution: "); Serial.print(sensor.resolution); Serial.println(" m/s^2");
 Serial.println("------------------------------------");
 Serial.println("");
 delay(500);
}
 
void displayDataRate(void)
{
 Serial.print ("Data Rate: ");
 
 switch(accel.getDataRate())
 {
 case ADXL345_DATARATE_3200_HZ:
 Serial.print ("3200 ");
 break;
 case ADXL345_DATARATE_1600_HZ:
 Serial.print ("1600 ");
 break;
 case ADXL345_DATARATE_800_HZ:
 Serial.print ("800 ");
 break;
 case ADXL345_DATARATE_400_HZ:
 Serial.print ("400 ");
 break;
 case ADXL345_DATARATE_200_HZ:
 Serial.print ("200 ");
 break;
 case ADXL345_DATARATE_100_HZ:
 Serial.print ("100 ");
 break;
 case ADXL345_DATARATE_50_HZ:
 Serial.print ("50 ");
 break;
 case ADXL345_DATARATE_25_HZ:
 Serial.print ("25 ");
 break;
 case ADXL345_DATARATE_12_5_HZ:
 Serial.print ("12.5 ");
 break;
 case ADXL345_DATARATE_6_25HZ:
 Serial.print ("6.25 ");
 break;
 case ADXL345_DATARATE_3_13_HZ:
 Serial.print ("3.13 ");
 break;
 case ADXL345_DATARATE_1_56_HZ:
 Serial.print ("1.56 ");
 break;
 case ADXL345_DATARATE_0_78_HZ:
 Serial.print ("0.78 ");
 break;
 case ADXL345_DATARATE_0_39_HZ:
 Serial.print ("0.39 ");
 break;
 case ADXL345_DATARATE_0_20_HZ:
 Serial.print ("0.20 ");
 break;
 case ADXL345_DATARATE_0_10_HZ:
 Serial.print ("0.10 ");
 break;
 default:
 Serial.print ("???? ");
 break;
 }
 Serial.println(" Hz");
}
 
void displayRange(void)
{
 Serial.print ("Range: +/- ");
 
 switch(accel.getRange())
 {
 case ADXL345_RANGE_16_G:
 Serial.print ("16 ");
 break;
 case ADXL345_RANGE_8_G:
 Serial.print ("8 ");
 break;
 case ADXL345_RANGE_4_G:
 Serial.print ("4 ");
 break;
 case ADXL345_RANGE_2_G:
 Serial.print ("2 ");
 break;
 default:
 Serial.print ("?? ");
 break;
 }
 Serial.println(" g");
}
 
void setup(void)
{
 Serial.begin(9600);
 connectToWiFi(networkName, networkPswd);
 Serial.println("Accelerometer Test"); Serial.println("");
 
 /* Initialise the sensor */
 if(!accel.begin())
 {
 /* There was a problem detecting the ADXL345 ... check your connections */
 Serial.println("no ADXL345 detected ... Check your wiring!");
 while(1);
 }
 
 /* Set the range to whatever is appropriate for your project */
 accel.setRange(ADXL345_RANGE_16_G);
 // displaySetRange(ADXL345_RANGE_8_G);
 // displaySetRange(ADXL345_RANGE_4_G);
 // displaySetRange(ADXL345_RANGE_2_G);
 
 /* Display some basic information on this sensor */
 displaySensorDetails();
 
 /* Display additional settings (outside the scope of sensor_t) */
 displayDataRate();
 displayRange();
 Serial.println("");
}
 
void loop(void)
{
 /* Get a new sensor event */
 sensors_event_t event;
 accel.getEvent(&event);
 
 /* Display the results (acceleration is measured in m/s^2) */
 Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print(" ");
 Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print(" ");
 Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print(" ");Serial.println("m/s^2 ");
 delay(500);

 float sensorValueX = event.acceleration.x;
 float sensorValueY = event.acceleration.y;
 float sensorValueZ = event.acceleration.z;
 int sensorValueB = analogRead(35);
 Serial.print("B: "); Serial.println(sensorValueB);
  //only send data when connected
  if(connected){
    OSCMessage msg("/xyzb");
    msg.add(sensorValueX);
    msg.add(sensorValueY);
    msg.add(sensorValueZ);
    msg.add(sensorValueB);
    udp.beginPacket(udpAddress, udpPort);
    msg.send(udp);
    udp.endPacket();
    msg.empty();
  }else{
    Serial.println("wi-fi NO");  
  }
  //Wait
  delay(2);

}

void connectToWiFi(const char * ssid, const char * pwd){
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);

  //Initiate connection
  WiFi.begin(ssid, pwd);
  Serial.println("Waiting for WIFI connection...");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
  switch(event) {
  case SYSTEM_EVENT_STA_GOT_IP:
  //When connected set
  Serial.print("WiFi connected! IP address: ");
  Serial.println(WiFi.localIP());
  //initializes the UDP state
  //This initializes the transfer buffer
  udp.begin(WiFi.localIP(),udpPort);
  connected = true;
  break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
  Serial.println("WiFi lost connection");
  connected = false;
  break;
  }
}
