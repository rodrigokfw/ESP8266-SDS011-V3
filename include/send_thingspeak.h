
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