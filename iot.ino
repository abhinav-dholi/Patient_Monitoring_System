#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library.   

#include <TinyGPS.h>
#include <SoftwareSerial.h>
SoftwareSerial SIM900(7, 8);

TinyGPS gps;  //Creates a new instance of the TinyGPS object



const int PulseWire = 0;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
const int LED13 = 13;          // The on-board Arduino LED, close to PIN 13.
int Threshold = 550;           // Determine which Signal to "count as a beat" and which to ignore.
                               // Use the "Project guide" to fine-tune Threshold Value beyond default setting.
                               // Otherwise leave the default "550" value. 
                               
PulseSensorPlayground pulseSensor;


void setup()
{
  Serial.begin(9600);
  SIM900.begin(9600);  

  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED13);        // blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);   


   if (pulseSensor.begin()) {
      Serial.println("pulseSensor Object created & activeted");  
    }
 
}

void loop()
{
  int myBPM = pulseSensor.getBeatsPerMinute(); 


  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial.available())
    {
      char c = Serial.read();
      //Serial.print(c);
      if (gps.encode(c)) 
        newData = true;  
    }
  }
  

  if (newData)      //If newData is true
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);   
    SIM900.print("AT+CMGF=1\r"); 
    delay(100);
    SIM900.println("AT + CMGS = \"+919090502324\"");
    delay(200);
    SIM900.print("Latitude = ");
    SIM900.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    SIM900.print(" Longitude = ");
    SIM900.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    SIM900.println((char)26); 
    delay(200);
    SIM900.println("BPM: ");
    Serial.println("BPM: ");  
   
    SIM900.println();

    if (pulseSensor.sawStartOfBeat()){                  
        SIM900.println(myBPM);
        Serial.println(myBPM);  
        delay(20);   
    }else{
          Serial.println("DATA not Available");
          Serial.println(myBPM);  
          SIM900.println("DATA not Available");        
          Serial.println("========");  
          delay(20);   
      }

  }
 Serial.println(failed);
}
