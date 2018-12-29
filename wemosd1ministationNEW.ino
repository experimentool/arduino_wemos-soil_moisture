/* This wifi station sketch is called wemosd1ministationNEW.ino by JAL 11/12/2018.
   The companion wifi access point sketch is called wemosd1accesspointNEW.ino.
   These two sketches communicate over wifi without a router.
   Using WEMOS D1 mini esp8266 module, Capacitive soil moisture sensor v1.2 and waterproof ds18b20 temperature probe.
   The ADS1115 measures the 3.3V bus on pin AIN0 as a quasi way to report battery voltage.
   Deep Sleep is used (max time is approx 60 minutes).  Remove jumper from D0 to RST to allow uploading on the WEMOS D1 mini.
   see: https://github.com/esp8266/Arduino/blob/master/doc/esp8266wifi/station-class.rst
*/

#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <Wire.h>
#include <Adafruit_ADS1015.h>
Adafruit_ADS1115 ads(0x48);  //With the ADDR pin on the ADS1115 connected to GND, the I2C address is (0x48)
float Voltage = 0.0;

//data wire of ds18b20 connected to wemos pin D1/GPIO5
#define ONE_WIRE_BUS 5

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// byte ledPin = 2;  //built in led
char ssid[] = "Wemos_AP";           // SSID of your AP
char pass[] = "Wemos_comm";         // password of your AP
String climate;
String climate2;
String deg1;
String deg2;
String Vdc;
IPAddress server(192, 168, 4, 15);  // IP address of the AP
WiFiClient client;

// Note: AirValue must be about 50 higher than 'dry' and WaterValue about 50 lower than 'soaked' water
// otherwise the program will stop transmitting or the values will be erratic.
const int AirValue = 900;   //you need to replace this value with Value_1 from calibrate sketch
const int WaterValue = 450;  //you need to replace this value with Value_2 from calibrate sketch


int intervals = (AirValue - WaterValue) / 3;
int soilMoistureValue = 0;

// sleep for this many seconds
const int sleepSeconds = 1800;  // deep sleep for 30 minutes

void setup() {
  Serial.begin(9600);

  ads.begin();

  // Start up the Dallas temperature library
  sensors.begin();

  // Connect D0 to RST to enable wake up. Note:Remove jumper from D0 to RST to allow uploading.
  pinMode(D0, WAKEUP_PULLUP);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);     // connects to the WiFi AP
  Serial.println();
  Serial.println("Connection to the AP");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Connected");
  Serial.println("station_bare_01.ino");
  Serial.print("LocalIP:"); Serial.println(WiFi.localIP());
  Serial.println("MAC:" + WiFi.macAddress());
  Serial.print("Gateway:"); Serial.println(WiFi.gatewayIP());
  Serial.print("AP MAC:"); Serial.println(WiFi.BSSIDstr());

  // pinMode(ledPin, OUTPUT);
}

void loop() {

  // Read the 3.3V bus as quasi way of measuring the battery voltage.
  int16_t adc0;  // we read from the ADC, we have a sixteen bit integer as a result
  adc0 = ads.readADC_SingleEnded(0);
  Voltage = (adc0 * 0.1875) / 1000;
  Serial.print("Voltage: ");
  Serial.println (Voltage);

  soilMoistureValue = analogRead(A0);  //Read the capacitive sensor signal connected to analog input pin A0
  // delay(100);
  sensors.requestTemperatures();  //The command to get temperatures
  Serial.println(sensors.getTempFByIndex(0));
  //delay(100);
  Serial.println(soilMoistureValue);
  if (soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals))
  {
    //Serial.println("Soaked ");
    char result[] = " Soaked           ";
    char deg1[] = " F    ";
    deg2 += deg1;
    char volt[] = " V \r";
    Vdc += volt;
    climate += result;
    climate2 = soilMoistureValue + climate + (sensors.getTempFByIndex(0) + deg2) + (Voltage + Vdc);
  }
  else if (soilMoistureValue > (WaterValue + intervals) && soilMoistureValue < (AirValue - intervals))
  {
    // Serial.println("Wet ");
    char result[] = "  WET             ";
    char deg1[] = " F    ";
    deg2 += deg1;
    char volt[] = " V \r";
    Vdc += volt;
    climate += result;
    climate2 = soilMoistureValue + climate + (sensors.getTempFByIndex(0) + deg2) + (Voltage + Vdc);
  }
  else if (soilMoistureValue < AirValue && soilMoistureValue > (AirValue - intervals))
  {
    // Serial.println("Dry ");
    //char result[] = "  Dry \r";
    char result[] = "  DRY             ";
    char deg1[] = " F    ";
    deg2 += deg1;
    char volt[] = " V \r";
    Vdc += volt;
    climate += result;
    climate2 = soilMoistureValue + climate + (sensors.getTempFByIndex(0) + deg2) + (Voltage + Vdc);

  }
  delay(100);  //100 millisecond delay


  client.connect(server, 80);
  // digitalWrite(ledPin, LOW);
  Serial.println("********************************");
  Serial.print("Message sent to the AP: ");
  Serial.println(climate2);
  (client.print(climate2));
  String answer = client.readStringUntil('\r');  //read string until a line feed
  Serial.println("From the AP: " + answer);
  client.flush();
  // digitalWrite(ledPin, HIGH);
  client.stop();
  delay(2000);
  climate = "";  //clears out text from climate string with null character
  deg2 = "";  //clears out text from jim1 string with null character
  // convert to microseconds
  ESP.deepSleep(sleepSeconds * 1000000);
}
