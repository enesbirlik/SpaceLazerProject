#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <HTTPClient.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>



const char* ssid = "my_wifi";
const char* password = "my_password";



Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

static const int servosPins[2] = {15 , 2};

const int relayPin = 19;
bool relayState = false;
int i = 0;
int anglex;
int averageHeading = 0;
String toSend;

Servo servos[2];

void setup(void) 
{
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("Starting...");

  WiFi.begin(ssid,password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi network");

  delay(5000);

/////////////////////////////////////////////COMPASS///////////////////////////////////////////////////////////////////////////////////

  Serial.println("HMC5883 Magnetometer Test"); Serial.println("");

  /* Initialise the sensor */
  if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while(1);
  }
  
  /* Display some basic information on this sensor */
  sensor_t sensor;
  mag.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");  
  Serial.println("------------------------------------");
  Serial.println("");


  for(int i = 0; i < 2; ++i) {
    if(!servos[i].attach(servosPins[i])) {
      Serial.print("Servo ");
      Serial.print(i);
      Serial.println("attach error");
    }
  }


  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);


  delay(500);
}

void loop(void) { 

  int selectedObject = Serial2.read();
  if (selectedObject>9||selectedObject<0) {
    selectedObject = 0;
  }

  //Serial.print("Selected Object: ");
  //Serial.println(selectedObject);




  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

    digitalWrite(LED_BUILTIN, HIGH);
    
    HTTPClient http;  //Declare an object of class HTTPClient

    http.begin("http://192.168.119.199:5000/calculate");  //Specify request destination
    http.addHeader("Content-Type", "application/json");  //Specify content-type header

    
    // can you change the selected object here
    String json = "{\"index\": " + String(selectedObject) + "}";
    int httpResponseCode = http.POST(json);   //Send the actual POST request

    if (httpResponseCode>0) {
    String response = http.getString();               //Get the response to the request
    //Serial.println(httpResponseCode);   //Print return code
    //Serial.println(response);           //Print request answer

    // Parse the response data
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, response);

    // Get the altitude and azimuth from the parsed data
    int altitude = doc["altitude"];
    int azimuth = doc["azimuth"];
    anglex = azimuth;

    

    // Create the string to send over Serial2
    String toSend = String(altitude) + "," + String(azimuth)+ "," ;

    
    // Serial.println(altitude);
    // Serial.println(azimuth);
    // Serial.println(averageHeading);

    //Serial.println("------------------------------------");
    Serial.print(toSend);
    

    // Send the data over Serial2
    Serial2.println(toSend);
    }
    else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);


    http.end();  //Free resources
    }
  } 
  else {
    Serial.println("Error in WiFi connection");   
  }



  sensors_event_t event;
  mag.getEvent(&event);

  float sum = 0.0;
  int numReadings = 10; // 10 ölçüm alacağız


  //Serial.print("X: "); Serial.print(event.magnetic.x); Serial.print("  ");
  //Serial.print("Y: "); Serial.print(event.magnetic.y); Serial.print("  ");
  //Serial.print("Z: "); Serial.print(event.magnetic.z); Serial.print("  "); Serial.println("uT");


  for (int i = 0; i < numReadings; ++i) {
    mag.getEvent(&event);

    float heading = atan2(event.magnetic.y, event.magnetic.x);
    float declinationAngle = 0.11;
    heading += declinationAngle;

    if (heading < 0) heading += 2 * PI;
    if (heading > 2 * PI) heading -= 2 * PI;

    float headingDegrees = heading * 180 / M_PI;

    if (headingDegrees < 0) {
      headingDegrees += 360;
    }

    sum += headingDegrees;
    averageHeading= sum / numReadings;
  }

  
  //Serial.print("Ortalama Kuzey Yönü: ");
  Serial.print(averageHeading);
  Serial2.print(averageHeading);

  Serial.println(" ");
  Serial2.println(" ");
  //Serial.println("------------------------------------");

  



  servos[0].write(anglex);


  
    
}
