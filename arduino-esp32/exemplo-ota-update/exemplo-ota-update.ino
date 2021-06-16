// This sketch provide the functionality of OTA Firmware Upgrade (1.0.5 ou superiores, da erro)
#include <WiFi.h>
//#include <Update.h>
#include <HttpsOTAUpdate.h>
// This sketch shows how to implement HTTPS firmware update Over The Air.
// Please provide your WiFi credentials, https URL to the firmware image and the server certificate.

static const char *ssid     = "Ribeiro_2.4G";  // your network SSID (name of wifi network)
static const char *password = "99955015"; // your network password

static const char *url = "https://www.dl.dropboxusercontent.com/s/r0cp91jeb7vp2j1/esp32_mqtt_gas_1_0.bin"; //state url of your firmware image

static const char *server_certificate = "-----BEGIN CERTIFICATE-----\n" \
  "MIIGXzCCBUegAwIBAgIQAkuvMgi2l3dyPfgpr40frTANBgkqhkiG9w0BAQsFADBw\n" \
  "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
  "d3cuZGlnaWNlcnQuY29tMS8wLQYDVQQDEyZEaWdpQ2VydCBTSEEyIEhpZ2ggQXNz\n" \
  "dXJhbmNlIFNlcnZlciBDQTAeFw0yMDEwMjcwMDAwMDBaFw0yMTExMjEyMzU5NTla\n" \
  "MGkxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpDYWxpZm9ybmlhMRYwFAYDVQQHEw1T\n" \
  "YW4gRnJhbmNpc2NvMRUwEwYDVQQKEwxEcm9wYm94LCBJbmMxFjAUBgNVBAMMDSou\n" \
  "ZHJvcGJveC5jb20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDUcoP7\n" \
  "52AYL/lIYXViV6lSlngsKrXQ9lYUJjP5ERI50fXu3r+bJZX0L6BAnYrjz13+1iDL\n" \
  "j2Kpwo8DCAtnhY3QK0Dj1FJyFFpBLTLjuJ1Ard5MBgszyhtIOy8dUVB5PRP0eAOS\n" \
  "t37Mm2BkqCms19VzPooOEW7oYfJ7Oj64RDJ3NebVdz/B+L8isnmeQOZIk3PZapG8\n" \
  "lwAq5zZoXVe9H9+yLOaoJDYtOO0622hr8C4kPgKeBH6tK5cMeFSDILN0ljCcCAR0\n" \
  "92fVQ0626ZWQwOR28SEBkQpHbrXrGPuxZs7yEDMLqQ0Smy6qJhJaVypUtK+iB2Ck\n" \
  "tkJ27TcRe5Vv4sEBAgMBAAGjggL6MIIC9jAfBgNVHSMEGDAWgBRRaP+QrwIHdTzM\n" \
  "2WVkYqISuFlyOzAdBgNVHQ4EFgQUPWw+5yqJb4AXcqq1gInhPz0YkMQwJQYDVR0R\n" \
  "BB4wHIINKi5kcm9wYm94LmNvbYILZHJvcGJveC5jb20wDgYDVR0PAQH/BAQDAgWg\n" \
  "MB0GA1UdJQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjB1BgNVHR8EbjBsMDSgMqAw\n" \
  "hi5odHRwOi8vY3JsMy5kaWdpY2VydC5jb20vc2hhMi1oYS1zZXJ2ZXItZzYuY3Js\n" \
  "MDSgMqAwhi5odHRwOi8vY3JsNC5kaWdpY2VydC5jb20vc2hhMi1oYS1zZXJ2ZXIt\n" \
  "ZzYuY3JsMEwGA1UdIARFMEMwNwYJYIZIAYb9bAEBMCowKAYIKwYBBQUHAgEWHGh0\n" \
  "dHBzOi8vd3d3LmRpZ2ljZXJ0LmNvbS9DUFMwCAYGZ4EMAQICMIGDBggrBgEFBQcB\n" \
  "AQR3MHUwJAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNvbTBNBggr\n" \
  "BgEFBQcwAoZBaHR0cDovL2NhY2VydHMuZGlnaWNlcnQuY29tL0RpZ2lDZXJ0U0hB\n" \
  "MkhpZ2hBc3N1cmFuY2VTZXJ2ZXJDQS5jcnQwDAYDVR0TAQH/BAIwADCCAQMGCisG\n" \
  "AQQB1nkCBAIEgfQEgfEA7wB1APZclC/RdzAiFFQYCDCUVo7jTRMZM7/fDC8gC8xO\n" \
  "8WTjAAABdWe4tDEAAAQDAEYwRAIgE7Qw9/ZLlEOWQfSdlhUkKKL/AjVRMpj6dVMs\n" \
  "pDRhzZkCIAMu8EIuY45nqd04JkCEx8zQ4qng9zafzwR4fRpmQ5DNAHYAXNxDkv7m\n" \
  "q0VEsV6a1FbmEDf71fpH3KFzlLJe5vbHDsoAAAF1Z7i0jAAABAMARzBFAiADgRX5\n" \
  "hg86ria/0QOoD+Eg+3NhFsFSh65WpBtkxatoaAIhANCpYx8XvF9BRqPTOih0xmrs\n" \
  "7HmN0p1OSFlgILSyPN+NMA0GCSqGSIb3DQEBCwUAA4IBAQBWBB3yrjBdn1KkGCM1\n" \
  "ZtFb0EIwbdPd08sJxlpBzHRonOa2RWF+MR4tHklk95xiPvZWRK2nQoKW6GNsIM1w\n" \
  "WFGYsSZXa0JB6rHbQslnN8LDFxIEu1qvNa2R9mkwrTsq91169SBn4FXPgvxPwUxO\n" \
  "FpScPooaAB7VFuCOQDh688ZIUVtF6USAOXEiG++B+SiR8hCPkBiNYwAYbAVoV9Rk\n" \
  "fu/UwtH/QXy9It2RYslVbBfK/nBoE2z0n8mmnSQ7oC7YcnUjIbuyzsK7yL3wp2c4\n" \
  "OzRpGxTJLtoRYg9EDRvti5x+eoejZClNqTfMc274PItFWgMBVUBbLW6JAfUVLPzH\n" \
  "IrWY\n" \
  "-----END CERTIFICATE-----";

static HttpsOTAStatus_t otastatus;

void HttpEvent(HttpEvent_t *event)
{
    switch(event->event_id) {
        case HTTP_EVENT_ERROR:
            Serial.println("Http Event Error");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            Serial.println("Http Event On Connected");
            break;
        case HTTP_EVENT_HEADER_SENT:
            Serial.println("Http Event Header Sent");
            break;
        case HTTP_EVENT_ON_HEADER:
            Serial.printf("Http Event On Header, key=%s, value=%s\n", event->header_key, event->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            break;
        case HTTP_EVENT_ON_FINISH:
            Serial.println("Http Event On Finish");
            break;
        case HTTP_EVENT_DISCONNECTED:
            Serial.println("Http Event Disconnected");
            break;
    }
}

void setup(){

    Serial.begin(115200);
    Serial.print("Attempting to connect to SSID: ");
    WiFi.begin(ssid, password);

    // attempt to connect to Wifi network:
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }

    Serial.print("Connected to ");
    Serial.println(ssid);
    
    HttpsOTA.onHttpEvent(HttpEvent);
    Serial.println("Starting OTA");
    HttpsOTA.begin(url, server_certificate, true);

    Serial.println("Please Wait it takes some time ...");
}

void loop(){
    otastatus = HttpsOTA.status();
    if(otastatus == HTTPS_OTA_SUCCESS) { 
        Serial.println("Firmware written successfully. To reboot device, call API ESP.restart() or PUSH restart button on device");
    } else if(otastatus == HTTPS_OTA_FAIL) { 
        Serial.println("Firmware Upgrade Fail");
    }
    delay(1000);
}
