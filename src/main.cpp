#include <Arduino.h>
#include "secrets.h"
#include <PID_v1.h>
#include <globals.h>
#include <EEPROM.h>

#include <EEPROM.h>
#include <PID_v1.h>
#include <FS.h>
#include <LITTLEFS.h>

#include <WiFi.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
// #include <AsyncElegantOTA.h>
//  #include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <OneWire.h>
#include <Wire.h>
#include <DallasTemperature.h>
// #include <timefunctions.h>
// #include "temperaturefunctions.h"

// #include "fire-flame-curved-solid.h"
#include "flame3.h"
// #include "bullseye.h"

// #include "countdowntimer.h"
// #include "PID_loop.h"
// #include "system_modes.h"

// server
AsyncWebServer server(80);

// Sensors
DeviceAddress tempProbeAddress;
// temperature sensor
#define ONE_WIRE_BUS 19 // 2 //sensor pin
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// TFT Display
#define TFT_DC 16
#define TFT_CS 22
// #define TFT_BL 34
#define TFT_RS 21
//#define LCD_RESET 21
// Meter colour schemes
#define RED2RED 0
#define GREEN2GREEN 1
#define BLUE2BLUE 2
#define BLUE2RED 3
#define GREEN2RED 4
#define RED2GREEN 5
Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RS);
#include "ringMeterFunctions.h"
#include "displayfunctions.h"

// sensor functions
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16)
      Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
};

// Time functions
bool getLocalTimeEsp(struct tm *info, uint32_t ms)
{
  uint32_t count = ms / 10;
  time_t now;
  time(&now);
  localtime_r(&now, info);
  if (info->tm_year > (2016 - 1900))
  {
    return true;
  }
  while (count--)
  {
    delay(10);
    time(&now);
    localtime_r(&now, info);
    if (info->tm_year > (2016 - 1900))
    {
      return true;
    }
  }
  return false;
};
void askfortime()
{
  getLocalTimeEsp(&tmstruct, 100);
}

// PID
void PIDCompute()
{
  // Serial.println("targetTemp");
  //  Serial.println(targetTemp);
  // Serial.println(" PID COMPUTE");
  myPID.Compute();
};
void PID_relay()
{

  /************************************************
   * turn the output pin on/off based on pid output
   ************************************************/
  if (millis() - windowStartTime > WindowSize)
  { // time to shift the Relay Window
    windowStartTime += WindowSize;
  }
  if (Output < millis() - windowStartTime)
  {
    digitalWrite(RELAY_PIN, LOW);
    heater = 0;
    // Serial.print(" Relay Pin HIGH ");
    // Serial.print(" Output ");
    // Serial.println(Output);
    //  Serial.print(" millis() - windowStartTime ");
    //  Serial.println(millis() - windowStartTime);
  }
  else if (Output > millis() - windowStartTime && Output > 1000)
  {
    digitalWrite(RELAY_PIN, HIGH);
    heater = 1;
    // Serial.print(" Relay Pin LOW  ");
    // Serial.print(" Output ");
    // Serial.println(Output);
    //  Serial.print(" millis() - windowStartTime ");
    //  Serial.println(millis() - windowStartTime);
  }
}

// Alarm
void alarm()
{
  if (alarm_int == 1)
  {
    tone(buzzer, 1500);
    delay(1000);
    noTone(buzzer);
    delay(1000);
  }
  if (alarm_int == 0)
  {
    //Serial.println(alarm_int);
  }
}

void setup()
{

  pinMode(buzzer, OUTPUT);

  delay(1000);
  Serial.begin(115200);

  delay(1000);
  Serial.println("GC9A01A Test!");

  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(GC9A01A_BLACK);

  Serial.println("Configuring EEPROM ...");
  EEPROM.begin(EEPROM_SIZE);

  if (!LittleFS.begin())
  {
    Serial.println("LittleFS mount failed");
    return;
  }

  Serial.println("Configuring WIFI...");
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("esp_rims_system");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");

  server.begin();

  Serial.println("HTTP server started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Contacting Time Server");
  configTime(3600 * timezone, daysavetime * 3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
  while (time(nullptr) < 1617460172) // minimum valid epoch
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(" ");
  Serial.println("Timer Server Connected");

  // Sensors
  // delay(2000);
  sensors.begin();
  Serial.println("sensorBegin");
  // delay(2000);

  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");
  // delay(2000);
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode())
    Serial.println("ON");
  else
    Serial.println("OFF");
  // delay(2000);
  Serial.print("Device 0 Address: ");
  printAddress(tempProbeAddress);
  Serial.println();
  // delay(2000);
  sensors.getAddress(tempProbeAddress, 0);
  Serial.println("getAddress");
  sensors.setResolution(tempProbeAddress, 9);
  // delay(2000);
  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(tempProbeAddress), DEC);
  //  delay(2000);
  Serial.println();
  sensors.requestTemperatures();
  Serial.println("requestTemperaturesByIndex");
  //   delay(2000);
  actualTemp = sensors.getTempF(tempProbeAddress);
  Serial.print("actualTemp ");
  Serial.println(actualTemp);
  // PID
  windowStartTime = millis();
  pinMode(RELAY_PIN, OUTPUT);

  targetTemp_select = EEPROM.read(eeprom_address_targetTemp);
  targetTemp_int = targetTemp_select.toInt();
  targetTemp = targetTemp_int;
  myPID.SetOutputLimits(0, WindowSize);
  myPID.SetMode(AUTOMATIC);
  myPID.SetSampleTime(50);

  // TFT Display

 

  askfortime();
  Serial.printf("\n Current Date Time  : %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct.tm_year) + 1900, (tmstruct.tm_mon) + 1, tmstruct.tm_mday, tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec);

  Serial.println("Ready");

  // server on functions
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", "text/html"); });

  server.on("/js/scripts.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/js/scripts.js", "application/javascript"); });

  server.on("/js/jquery.simple.timer.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/js/jquery.simple.timer.js", "application/javascript"); });

  server.on("/img/fire-flame-grey.svg", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/img/fire-flame-grey.svg", "image/svg+xml"); });

  server.on("/img/fire-flame-red.svg", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/img/fire-flame-red.svg", "image/svg+xml"); });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/favicon.ico", "image/x-icon"); });

  server.on("/AreYouThere", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", "Yes"); });

  server.on("/getActualTemp", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", String(actualTemp)); });

  server.on("/getHours", HTTP_GET, [](AsyncWebServerRequest *request)
            { hours_select=EEPROM.read(eeprom_address_hours);
              request->send(200, "text/plain", hours_select); });

  server.on("/getMinutes", HTTP_GET, [](AsyncWebServerRequest *request)
            { minutes_select=EEPROM.read(eeprom_address_minutes);
              request->send(200, "text/plain", minutes_select); });

  server.on("/setTimer", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              if (request->hasParam("hours_select", true))
              {
                  hours_select = request->getParam("hours_select", true)->value();
           
                  int hours_select_int = hours_select.toInt();
                  EEPROM.write(eeprom_address_hours, hours_select_int);
                  EEPROM.commit();
              }
              if (request->hasParam("minutes_select", true))
              {
                  minutes_select = request->getParam("minutes_select", true)->value();
                  int minutes_select_int = minutes_select.toInt();
                  EEPROM.write(eeprom_address_minutes, minutes_select_int);
                  EEPROM.commit();
              }



              request->send(200, "text/plain", "Set Timer Complete"); });

  server.on("/setTargetTemp", HTTP_POST, [](AsyncWebServerRequest *request)
            {
            if (request->hasParam("targetTemp_select", true))
              {
                  targetTemp_select = request->getParam("targetTemp_select", true)->value();
                   targetTemp_int = targetTemp_select.toInt();
                  EEPROM.write(eeprom_address_targetTemp, targetTemp_int);
                  EEPROM.commit();
              }
              request->send(200, "text/plain", "TargetTemp Set"); });

  server.on("/ESPrestart", HTTP_GET, [](AsyncWebServerRequest *request)
            {
            request->send(200, "text/plain", "ESP restart"); 
            ESP.restart(); }

  );
  server.on("/gettargetTemp", HTTP_GET, [](AsyncWebServerRequest *request)
            { targetTemp_select=EEPROM.read(eeprom_address_targetTemp);
            targetTemp_int = targetTemp_select.toInt();
              request->send(200, "text/plain", String(targetTemp_select)); });

  server.on("/getTimerRunning", HTTP_GET, [](AsyncWebServerRequest *request)
            { timerRunning=EEPROM.read(eeprom_address_timerRunning);
            EEPROM.commit();
              request->send(200, "text/plain", String(timerRunning)); });

  server.on("/setTimerRunning", HTTP_POST, [](AsyncWebServerRequest *request)
            {
            if (request->hasParam("timerRunningVal", true))
              {
                timerRunning_str = request->getParam("timerRunningVal", true)->value();
                timerRunning = timerRunning_str.toInt();
                EEPROM.write(eeprom_address_timerRunning, timerRunning);
                EEPROM.commit();
              }
              request->send(200, "text/plain", String(timerRunning)); });

  server.on("/setTimeLeft", HTTP_POST, [](AsyncWebServerRequest *request)
            {
            if (request->hasParam("timeLeftVal", true))
              {
                timeLeft_str = request->getParam("timeLeftVal", true)->value();
                timeLeft = timeLeft_str.toInt();
                EEPROM.put(eeprom_address_timeLeft, timeLeft);
                EEPROM.commit();
              }
              request->send(200, "text/plain", String(timeLeft)); });

  server.on("/getTimeLeft", HTTP_GET, [](AsyncWebServerRequest *request)
            { EEPROM.get(eeprom_address_timeLeft,timeLeft);
            EEPROM.commit();
            request->send(200, "text/plain", String(timeLeft)); });

  server.on("/getStartTime", HTTP_GET, [](AsyncWebServerRequest *request)
            { EEPROM.get(eeprom_address_startTime,startTime_str);
            EEPROM.commit();
              //  Serial.println("startTime_str");
              //  Serial.println(startTime_str);
            request->send(200, "text/plain",  startTime_str ); });

  server.on("/setStartTime", HTTP_POST, [](AsyncWebServerRequest *request)
            {
            if (request->hasParam("StartTimeVal", true))
              {
                startTime_str = request->getParam("StartTimeVal", true)->value();
                startTime = startTime_str.toInt();
               //  Serial.println("startTime_str");
               // Serial.println(startTime_str);
                // Serial.println("startTime");
                // Serial.println(startTime);
                EEPROM.put(eeprom_address_startTime, startTime_str);
                EEPROM.commit();
              }
              request->send(200, "text/plain", startTime_str); });

  server.on("/getStopTime", HTTP_GET, [](AsyncWebServerRequest *request)
            { EEPROM.get(eeprom_address_stopTime,stopTime_str);
            EEPROM.commit();
            //    Serial.println("stopTime_str");
            //    Serial.println(stopTime_str);
              request->send(200, "text/plain",  stopTime_str ); });

  server.on("/setStopTime", HTTP_POST, [](AsyncWebServerRequest *request)
            {
            if (request->hasParam("StopTimeVal", true))
              {
                stopTime_str = request->getParam("StopTimeVal", true)->value();
                stopTime = stopTime_str.toInt();
               // Serial.println("stopTime_str");
               // Serial.println(stopTime_str);
                // Serial.println("stopTime");
                // Serial.println(stopTime);
                EEPROM.put(eeprom_address_stopTime, stopTime_str);
                EEPROM.commit();
              }
              request->send(200, "text/plain", stopTime_str); });

  server.on("/setAlarm", HTTP_POST, [](AsyncWebServerRequest *request)
            {
            if (request->hasParam("Alarm_int", true))
              {
                  alarm_str = request->getParam("Alarm_int", true)->value();
                  alarm_int = alarm_str.toInt();
                  EEPROM.write(eeprom_address_Alarm, alarm_int);
                  EEPROM.commit();
              }
              request->send(200, "text/plain", "TargetTemp Set"); });

  server.on("/setPID", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              if (request->hasParam("pid_kpValue", true))
              {
                  pid_kp_str = request->getParam("pid_kpValue", true)->value();

                  pid_kp = pid_kp_str.toDouble();

                  EEPROM.put(eeprom_address_pid_kp, pid_kp);
                  EEPROM.commit();
              }
             if (request->hasParam("pid_kiValue", true))
              {
                 pid_ki_str = request->getParam("pid_kiValue", true)->value();
                  //Serial.print(" pid_ki from /setPID ");
                  //Serial.println(pid_ki);
                  pid_ki = pid_ki_str.toDouble();

                  EEPROM.put(eeprom_address_pid_ki, pid_ki);
                  EEPROM.commit();
              }
              if (request->hasParam("pid_kdValue", true))
              {
                    pid_kd_str = request->getParam("pid_kdValue", true)->value();
                  //Serial.print(" pid_kd from /setPID ");
                  //Serial.println(pid_kd);
                  pid_kd = pid_kd_str.toDouble();

                  EEPROM.put(eeprom_address_pid_kd, pid_kd);
                  EEPROM.commit();
              }
                myPID.SetTunings(pid_kp,pid_ki,pid_kd,P_ON_M);
              request->send(200, "text/plain", "PID Complete"); });

  server.on(
      "/getKp_set", HTTP_GET, [](AsyncWebServerRequest *request)
      { EEPROM.get(eeprom_address_pid_kp, pid_kp);
        request->send(200, "text/plain", String(pid_kp)); });

  server.on(
      "/getKi_set", HTTP_GET, [](AsyncWebServerRequest *request)
      { EEPROM.get(eeprom_address_pid_ki, pid_ki);
        request->send(200, "text/plain", String(pid_ki)); });

  server.on(
      "/getKd_set", HTTP_GET, [](AsyncWebServerRequest *request)
      { EEPROM.get(eeprom_address_pid_kd, pid_kd);
      request->send(200, "text/plain", String(pid_kd)); });

  server.on("/PID_Output", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", String(Output)); });
  // Get heater value
  server.on("/get_heater_state", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
            //Serial.print("heater ");
            //Serial.println(heater);
            request->send(200, "text/plain", String(heater)); });
}

void loop()
{
  // Serial.println("  tft.fillScreen(GC9A01A_GREEN);");
  //   tft.fillScreen(GC9A01A_GREEN);

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    sensors.requestTemperatures();

    // printTemperature(tempProbeAddress);
    actualTemp = sensors.getTempF(tempProbeAddress);
    //   reading = actualTemp;
    // Serial.println(actualTemp);

    if (heater == 1)
    {
      tft.drawBitmap(120 - 16, 220 - 43, epd_bitmap_fire_flame_curved_solid_32, 32, 43, GC9A01A_RED);
    }
    else
    {
      tft.drawBitmap(120 - 16, 220 - 43, epd_bitmap_fire_flame_curved_solid_32, 32, 43, GC9A01A_LIGHTGREY);
    }

    displayActualLabel();
    displayTemp();
    displayTargetLabel();
    displayTarget();
    displayTempRing();
    displayTargetLine();
    previousMillis = currentMillis;
  }
  // displayActualLabel();
  PIDCompute();
  PID_relay();
  alarm_int=0;
alarm();
}
