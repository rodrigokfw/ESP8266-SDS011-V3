#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <avr/dtostrf.h>
#include <strings.h>
#include <AvgStd.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>
#include <Arduino_JSON.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <stdbool.h>
#include <time.h>
#include <cstdlib>
#include <base64.h>
#include <bearssl/bearssl.h>
#include <bearssl/bearssl_hmac.h>
#include <libb64/cdecode.h>
#include <az_result.h>
#include <az_span.h>
#include <az_iot_hub_client.h>
#include <ca.h>
#include <SdsDustSensor.h>
#include <Adafruit_BME280.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h" //https://github.com/tzapu/WiFiManager

bool SDS_011_ = true;      // SDS011 Indicator
bool BME_280_ = true;      // BME280 Indicator
byte Azure_IoT = 1;        // Azure IoT Hub
SdsDustSensor sds(Serial); // SDS011 Sensor
Adafruit_BME280 bme0;      // I2C BME280

// Init Variables
int configg[8];
uint64_t start99;
byte FirstStart;
short ret;
float _latitud_, _longitud_;
int _env_;
uint8_t time_smple;
File myFile;
byte Error_Ind;
int Reset_IT, Numbr_IT;

// ESP8266 Configuration
int LED_PORT = 2;


/********************* Global connection instances **************************/
HTTPClient https;

// JSON message creation part
JSONVar message_object_5; // Thingspeak JSON
JSONVar message_object_6; // Azure JSON
String json_msg_string;   // to stringify JSON message object
#define STATIC_MESSAGE_BUF_LEN 150
char message_string_buf[STATIC_MESSAGE_BUF_LEN]; // to use in mqtt publish function

/*------------------------------------------------------
   Measurement
------------------------------------------------------*/
#include "config_iot.h"
#include "azure_iot.h"
#include "secureupdate_.h"

void configModeCallback(WiFiManager *myWiFiManager)
{
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  // if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

// Send Thingspeak Function
void send_thingspeak(JSONVar message_object_5)
{
  std::unique_ptr<BearSSL::WiFiClientSecure> client2(new BearSSL::WiFiClientSecure);
  client2->setInsecure();
  if (https.begin(*client2, TS_SERVER))
  {
    Serial.print("[HTTPS] POST...\n");
    https.addHeader("Content-Type", "application/json");
    json_msg_string = JSON.stringify(message_object_5);
    json_msg_string.toCharArray(message_string_buf, json_msg_string.length() + 1);
    int httpCode = https.POST(message_string_buf);
    // cleanup memory used
    memset(message_string_buf, 0, STATIC_MESSAGE_BUF_LEN);
    // httpCode will be negative on error
    if (httpCode > 0)
    {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        String payload = https.getString();
        Serial.println(payload);
      }
    }
    else
    {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }
    https.end();
  }
}

/*------------------------------------------------------
 * Function NTP Sync
 --------------------------------------------------------*/
WiFiUDP ntpUDP; // WiFi UDP
NTPClient timeClient(ntpUDP, "time.google.com");

// AZURE CONFIG
void CONFIGAZUREIOT_()
{
  // Get a time structure
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Formatted Time: ");
  Serial.println(formattedTime);
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon + 1;
  int currentYear = ptm->tm_year + 1900;

  String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  Serial.print("Current date: ");
  Serial.println(currentDate);

  char Azure_datet[50];
  sprintf(Azure_datet, "%4d-%02d-%02dT%02d:%02d:%02d", currentYear, currentMonth, monthDay, currentHour, currentMinute, currentSecond);
  message_object_6["datetime"] = Azure_datet;
}

int *BME_280__(int time_smple)
{
  AvgStd temp_r, pres_r, hume_r;
  for (int jj = 0; jj < time_smple; jj++)
  {
    temp_r.checkAndAddReading(bme0.readTemperature());
    pres_r.checkAndAddReading(bme0.readPressure() / 100.0F);
    hume_r.checkAndAddReading(bme0.readHumidity());
    Serial.print(".");
    delay(10);
  }

  static int _METEO_[3];
  _METEO_[0] = -1;
  _METEO_[1] = -1;
  _METEO_[2] = -1;

  if (temp_r.getN() > 0)
  {
    _METEO_[0] = temp_r.getMean();
    _METEO_[1] = pres_r.getMean();
    _METEO_[2] = hume_r.getMean();
  }
  Serial.println("BME280 OK!");
  return _METEO_;
}

int *_SDS_011__(int time_sample)
{

  static int _PM_[2];
  _PM_[0] = -1;
  _PM_[1] = -1;

  AvgStd PM10RAW, PM25RAW;
  for (int jj = 0; jj < time_sample; jj++)
  {
    PmResult pm = sds.queryPm();
    if (pm.isOk())
    {
      PM10RAW.checkAndAddReading(pm.pm10);
      PM25RAW.checkAndAddReading(pm.pm25);
      delay(1050);
    }
  }

  if (PM10RAW.getN() > 0)
  {
    _PM_[0] = PM10RAW.getMean();
    _PM_[1] = PM25RAW.getMean();
  }

  Serial.println(" SDS011 OK!");
  return _PM_;
}

void setup()
{
  FirstStart = 1;
  Error_Ind = 0;
  delay(1000);

  Serial.begin(9600);

  Serial.println(" ");
  configg[0] = 0;       // ex ID variable
  configg[1] = 300;      // TimeSample
  configg[2] = 1;        // Data Transmission
  configg[3] = 0;        // GMT Zone
  time_smple = 10;       //
  Reset_IT = 30;
  //config_json();
  Serial.println(" ");
  Serial.println("Initializing...");
  Numbr_IT = 0;

  // We start by connecting to a WiFi network
  if (int(configg[2]) == 1)
  {
    // Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    // wifiManager.addParameter(&latitudd);
    // wifiManager.addParameter(&longitudd);
    // reset saved settings
    // wifiManager.resetSettings();

    // set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
    wifiManager.setAPCallback(configModeCallback);

    wifiManager.setTimeout(300);

    // fetches ssid and pass and tries to connect
    // if it does not connect it starts an access point with the specified name
    // here  "AutoConnectAP"
    // and goes into a blocking loop awaiting configuration
    if (!wifiManager.autoConnect())
    {
      Serial.println("failed to connect and hit timeout");
      // reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(1000);
    }
  }
  /******************************************/

  if (WiFi.status() == WL_CONNECTED)
  {
    timeClient.begin();
    timeClient.setTimeOffset(-14400);
  }

  setClock();
  pinMode(LED_BUILTIN, OUTPUT);
  setClock();
  FirmwareUpdate();

  // bme280
  if (BME_280_)
  {
    bool status;
    status = bme0.begin();
    delay(1000);
    BME_280_ = true;
    Serial.print("BME280 ");
    if (!status)
    {
      status = bme0.begin(0x76);
      delay(1000);
      if (!status)
      {
        Serial.println("Not Found!");
        delay(1000);
        BME_280_ = false;
      }
    }
    if (status)
    {
      Serial.println("begin");
    }
  }

  if (SDS_011_)
  {
    pinMode(D5, OUTPUT);
    digitalWrite(D5, LOW);
    delay(5000);
    digitalWrite(D5, HIGH);
    delay(5000);

    Serial.print("SDS011 ");
    delay(1000);

    sds.begin();
    delay(10000);

    sds.setQueryReportingMode();
    delay(10000);
    PmResult pm = sds.queryPm();
    delay(10000);
    if (pm.isOk())
    {
      Serial.println(" begin");
    }
    else
    {
      SDS_011_ = false;
    }
  }

  config_json();

  // End Setup


  Serial.println(" ");
  Serial.print("Waiting");
}

void loop()
{
  // put your main code here, to run repeatedly:

  // if (Error_Ind == 1)
  // {
  //   pinMode(LED_PORT, OUTPUT);
  //   digitalWrite(LED_PORT, LOW); // Turn the LED on by making the voltage LOW
  //   delay(120000);               // Wait
  //   //ESP.reset();
  // }

  if (Numbr_IT == Reset_IT)
  {
    Serial.println(". Reset..");
    ESP.restart(); // processor software reset
  }
  else
  {
    Numbr_IT = Numbr_IT + 1;
  }

  if (FirstStart == 0)
  {
    for (int ii = 0; ii < configg[1]; ii++)
    {
      Serial.print(".");
      delay(1000);
    }
  }
  Serial.println(" OK!");

  if (Azure_IoT == 1)
  {
    CONFIGAZUREIOT_();
  }

  if (BME_280_)
  {
    int *_METEO_ = BME_280__(time_smple);
    if (_METEO_[0] > -1 && _METEO_[0] < 100 && _METEO_[2] > 0 && _METEO_[2] < 100)
    {
      message_object_5["field1"] = _METEO_[0];
      ;
      message_object_5["field2"] = _METEO_[1];
      message_object_5["field3"] = _METEO_[2];
      message_object_6["Temp"] = _METEO_[0];
      message_object_6["Press"] = _METEO_[1];
      message_object_6["RH"] = _METEO_[2];
    }
  }

  if (SDS_011_)
  {
    byte _PM_working = 1;
    if (configg[1] > 30)
    {
      WorkingStateResult state1 = sds.wakeup();
      delay(10000);
      if (!state1.isWorking())
      {
        _PM_working = 0;
      }
    }

    if (_PM_working == 1)
    {
      int *_PM_ = _SDS_011__(time_smple);
      if (_PM_[0] != -1)
      {
        message_object_5["field4"] = _PM_[0];
        message_object_5["field5"] = _PM_[1];
        message_object_6["PM10"] = _PM_[0];
        message_object_6["PM25"] = _PM_[1];
        message_object_6["Riesgo"] = 1;
      }

      if (configg[1] > 30)
      {
        sds.sleep();
        delay(10000);
      }
    }
    else
    {
      digitalWrite(D5, LOW);
      delay(5000);
      digitalWrite(D5, HIGH);
      delay(5000);

      sds.begin();
      delay(10000);

      sds.setQueryReportingMode();
      delay(10000);
      PmResult pm = sds.queryPm();
      delay(10000);
      if (!pm.isOk())
      {
        SDS_011_ = false;
      }
    }
  }

  send_thingspeak(message_object_5);
  // Azure IoT Hub
  if (Azure_IoT == 1)
  {
    if (!mqtt_client.connected())
    {
      establishConnection();
    }
    send_azure(message_object_6);
    digitalWrite(LED_PORT, HIGH); // Turn the LED off by making the voltage LOW
  }

  FirstStart = 0;
  Serial.println(" ");
  Serial.print("Waiting");
}