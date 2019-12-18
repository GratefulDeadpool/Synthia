#include "HardwareSerial.h"
#include "Math.h";

#include "Updater.h";

bool debug = 0;

int channel = 0;
double freq = 0;
HardwareSerial BTserial(2);

int modeChoice    = -1;  // reset to -1
int startupMillis = 0;

bool offMessage = 1;

String currentString;
Updater UpdateT;

double minRoll  = 0;
double minPitch = 0;
double minYaw   = 0;

double maxRoll  = 0;
double maxPitch = 0;
double maxYaw   = 0;

double roll;
double pitch;
double yaw;

double baseFreq;
double pitchMultiplier;

double setBound(int compareChoice, double newVal, double oldVal)
{
  if ((compareChoice == 0) && (newVal < oldVal))       // Min bound
  {
    return newVal;
  }
  else if ((compareChoice == 1) && (newVal > oldVal))  // Max bound
  {
    return newVal;
  }
  else
  {
    return oldVal;
  }
}

double mapDouble(double x, double in_min, double in_max, double out_min, double out_max)   // Adapted from Arduino Reference: https://www.arduino.cc/reference/en/language/functions/math/map/
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
 
void setup() 
{
  pinMode(32, INPUT);  // Activates piano mode
  pinMode(33, INPUT);  // Activates theremin mode

  ledcSetup(channel, freq, 16);
  ledcAttachPin(12, channel);
    
  Serial.begin(9600);
  BTserial.begin(9600);
}
 
void loop()
{
  if (modeChoice == -1)  // start Synthia and choose mode
  {
    if (digitalRead(32) == HIGH && digitalRead(33) == LOW)
    {
      Serial.println("Piano Mode selected");
      Serial.println("______________________");

      modeChoice    = 0;
      startupMillis = millis();
    }
    else if (digitalRead(33) == HIGH && digitalRead(32) == LOW)
    {
      Serial.println("Theremin Mode selected");
      Serial.println("______________________");

      modeChoice    = 1;
      startupMillis = millis();
    }
    else
    {
      modeChoice = -1;
    }
  }
  else if ((millis() - startupMillis <= 10000) && BTserial.available())  // calibrate
  {
    offMessage = 1;

    currentString = BTserial.readStringUntil('>') + ",";
    UpdateT.updateTilt(currentString, -999.9, 999.9, -999.9, 999.9, -999.9, 999.9);

    minRoll = setBound(0, UpdateT.getX(), minRoll);
    maxRoll = setBound(1, UpdateT.getX(), maxRoll);

    minPitch = setBound(0, UpdateT.getY(), minPitch);
    maxPitch = setBound(1, UpdateT.getY(), maxPitch);

    minYaw = setBound(0, UpdateT.getZ(), minYaw);
    maxYaw = setBound(1, UpdateT.getZ(), maxYaw);

    Serial.print("Roll Bounds: [");
    Serial.print(minRoll);
    Serial.print(", ");
    Serial.print(maxRoll);
    Serial.println("]");
    Serial.print("Pitch Bounds: [");
    Serial.print(minPitch);
    Serial.print(", ");
    Serial.print(maxPitch);
    Serial.println("]");
    Serial.print("Yaw Bounds: [");
    Serial.print(minYaw);
    Serial.print(", ");
    Serial.print(maxYaw);
    Serial.println("]");
    if (debug) 
    {
      Serial.println(currentString);
      Serial.println(UpdateT.getX());
      Serial.println(UpdateT.getY());
      Serial.println(UpdateT.getZ());
    }
    Serial.println("______________________");
  }
  else if ((millis() - startupMillis > 10000) && BTserial.available())  // Update Frequency
  {
    offMessage = 1;
    
    currentString = BTserial.readStringUntil('>') + ",";
    UpdateT.updateTilt(currentString, minRoll, maxRoll, minPitch, maxPitch, minYaw, maxYaw);

    roll  = UpdateT.getX();
    pitch = UpdateT.getY();
    yaw   = UpdateT.getZ();

    if (modeChoice == 0)  // Piano mode
    {
      baseFreq        = 440.0 * pow(2.0, map(roll, minRoll, maxRoll, -9, 2) / 12.0);
      pitchMultiplier = pow(2.0, map(pitch, minPitch, maxPitch, -3, 4));
    }
    else                  // Theremin mode
    {
      baseFreq        = mapDouble(roll, minRoll, maxRoll, 440.0 * pow(2.0, -9.0/12.0), 440.0 * pow(2.0, 2.0/12.0));
      pitchMultiplier = pow(2.0, mapDouble(pitch, minPitch, maxPitch, -3.0, 4.0));
    }

    freq = constrain(baseFreq * pitchMultiplier, 32.7, 7040.0);
    ledcWriteTone(channel, freq);

    Serial.print("Roll: ");
    Serial.println(roll);
    Serial.print("Pitch: ");
    Serial.println(pitch);
    Serial.print("Yaw: ");
    Serial.println(yaw);
    Serial.println();
    Serial.print("Frequency: ");
    Serial.println(freq);
    if (debug) 
    {
      Serial.println(currentString);
      Serial.println(baseFreq);
      Serial.println(pitchMultiplier);
    }
    Serial.println("______________________");
  }
  else
  {
    if (offMessage)
    {
      Serial.println("Please connect Bluetooth device.");
      offMessage = 0;
    }
    
    minRoll  = 0;
    minPitch = 0;
    minYaw   = 0;

    maxRoll  = 0;
    maxPitch = 0;
    maxYaw   = 0;
    
    ledcWriteTone(channel, 0);
    startupMillis = millis();
  }
 
  if (Serial.available())
  {
    BTserial.write(Serial.read());
  }
}
