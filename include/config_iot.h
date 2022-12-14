// Thingspeak variables
const char *TS_SERVER = "https://api.thingspeak.com/update.json";

// Azure Variables
const char *host = "sensor-node.azure-devices.net";
String macAddress = WiFi.macAddress();
const char *device_id = macAddress.c_str();
const char *device_key = "nZ5OVRD0xa6BaLjTa9BSMwQ+YkheyQO1IP+4d7lg2xw=";

void config_json()
{

  Serial.println(" ");
  Serial.println(macAddress.c_str());

  if (macAddress == "84:F3:EB:C9:85:7E")
  {

    message_object_5["api_key"] = "XU8N1L47LX51780K";
    message_object_6["ID"] = 990;
    message_object_6["Lat"] = -33.5835;
    message_object_6["Lon"] = -70.5656;
    message_object_6["Env"] = 1;
  }

  if (macAddress == "84:F3:EB:C9:13:65")
  {

    message_object_5["api_key"] = "QJTNGUUFRYZ6G17I";
    message_object_6["ID"] = 991;
    message_object_6["Lat"] = -33.5834;
    message_object_6["Lon"] = -70.5656;
    message_object_6["Env"] = 1;
  }

  if (macAddress == "84:F3:EB:C9:33:0B")
  {

    message_object_5["api_key"] = "K1PDWGUI3P7CJPHK";
    message_object_6["ID"] = 992;
    message_object_6["Lat"] = -33.5835;
    message_object_6["Lon"] = -70.5657;
    message_object_6["Env"] = 1;
  }

  if (macAddress == "84:F3:EB:C9:0C:5C")
  {

    message_object_5["api_key"] = "10WYF5X1AWUOMFOH";
    message_object_6["ID"] = 993;
    message_object_6["Lat"] = -33.5835;
    message_object_6["Lon"] = -70.5656;
    message_object_6["Env"] = 1;
  }

}