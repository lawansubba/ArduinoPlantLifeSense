// Fill in  your WiFi networks SSID and password
#define SECRET_SSID "##WIFI_ACCESSPOINT_NAME##"
#define SECRET_PASS "##WIFI_PASSWORD##"

// Fill in the hostname of your AWS IoT broker
#define SECRET_BROKER ""AWS_IOT_BROKER""

// Fill in the boards public certificate
const char SECRET_CERTIFICATE[] = R"(
-----BEGIN CERTIFICATE-----
##SECRET_CERTIFICATE##
-----END CERTIFICATE-----
)";
