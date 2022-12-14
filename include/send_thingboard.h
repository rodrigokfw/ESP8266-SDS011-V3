
void CONFIG_TB_()
{
  // send data to flespi MQTT broker via secure connection
  json_msg_string = JSON.stringify(message_object);
  char message_string_buf[STATIC_MESSAGE_BUF_LEN];
  json_msg_string.toCharArray(message_string_buf, json_msg_string.length() + 1);
  Serial.print("Publishing message to broker: ");
  Serial.println(message_string_buf);

  mqtt_1.setServer(MQTT_SERVER, MQTT_SERVERPORT);
  if (mqtt_1.connect(CLIENT_ID, IO_TOKEN, IO_PASS))
  {
    mqtt_1.publish(DEST_TOPIC, message_string_buf);
  }

  // cleanup memory used
  memset(message_string_buf, 0, STATIC_MESSAGE_BUF_LEN);
}

void Send_Meteo_TB(int TEMP, int HR)
{
  now_ = rtc.now();
  unixxtime = now_.unixtime();
  unixxtime = unixxtime - (int(configg[3]) * 60 * 60);
  snprintf(buf_, sizeof(buf_), "%u%u", unixxtime, 512); // format 2 numbers as string
  Serial.println(buf_);
  double z;
  z = atof(buf_);
  delay(1000);
  JSONVar message_object_3;
  message_object_3["modelo"] = "BME280";
  message_object_3["temperatura"] = TEMP;
  message_object_3["fecha:meteorologia:temperatura"] = z;
  message_object_3["medicion:meteorologia:humedad"] = HR;
  message_object_3["fecha:meteorologia:humedad"] = z;

  json_msg_string = JSON.stringify(message_object_3);
  char message_string_buf[STATIC_MESSAGE_BUF_LEN];
  json_msg_string.toCharArray(message_string_buf, json_msg_string.length() + 1);
  Serial.print("Publishing message to broker: ");
  Serial.println(message_string_buf);

  if (mqtt_1.connect(CLIENT_ID, IO_TOKEN, IO_PASS))
  {
    mqtt_1.publish(DEST_TOPIC_2, message_string_buf);
  }

  // cleanup memory used
  memset(message_string_buf, 0, STATIC_MESSAGE_BUF_LEN);
}

void Send_PM_TB(int PM10, int PM25)
{
  DateTime now_2 = rtc.now();
  unixxtime = now_2.unixtime();
  unixxtime = unixxtime - (int(configg[3]) * 60 * 60);
  snprintf(buf_, sizeof(buf_), "%u%u", unixxtime, 512); // format 2 numbers as string
  double zz;
  zz = atof(buf_);
  delay(1000);
  JSONVar message_object_2; // to store message parameters
  message_object_2["modelo"] = "SDS011";
  message_object_2["medicion:contaminante:mp2_5"] = PM25;
  message_object_2["fecha:contaminante:mp2_5"] = zz;
  message_object_2["medicion:contaminante:mp10"] = PM10;
  message_object_2["fecha:contaminante:mp10"] = zz;
  json_msg_string = JSON.stringify(message_object_2);
  char message_string_buf[STATIC_MESSAGE_BUF_LEN];
  json_msg_string.toCharArray(message_string_buf, json_msg_string.length() + 1);
  Serial.println(" ");
  Serial.print("Publishing message to broker: ");
  Serial.println(message_string_buf);

  //mqtt_1.setServer(MQTT_SERVER, MQTT_SERVERPORT);
  if (mqtt_1.connect(CLIENT_ID, IO_TOKEN, IO_PASS))
  {
    mqtt_1.publish(DEST_TOPIC_2, message_string_buf);
  }

  // cleanup memory used
  memset(message_string_buf, 0, STATIC_MESSAGE_BUF_LEN);
}