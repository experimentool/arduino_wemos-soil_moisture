/*  This wifi access point sketch is called wemosd1miniaccesspointNEW.ino by JAL 11/12/2018
   The companion wifi station sketch is called wemosd1ministationNEW.ino by JAL 11/12/2018
   These two sketches use wifi to communicate without a router.
   Using WEMOS D1R1 mini esp8266 module and ssd1306 128x64 SPI oled module

    Reference: Accesspoint - Station wifi communication without router
    see: https://github.com/esp8266/Arduino/blob/master/doc/esp8266wifi/soft-access-point-class.rst
         https://github.com/esp8266/Arduino/blob/master/doc/esp8266wifi/soft-access-point-examples.rst
         https://github.com/esp8266/Arduino/issues/504
*/


#include <ESP8266WiFi.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <SPI.h>

// If using software SPI (the default case):
#define OLED_MOSI  13   //wemos d1 mini pin D7/gpio13
#define OLED_CLK   14   //wemos d1 mini pin D5/gpio14
#define OLED_DC    5    //wemos d1 mini pin D1/gpio5
#define OLED_CS    15   //wemos d1 mini pin D8/gpio15
#define OLED_RESET 0    //wemos d1 mini pin D3/gpio0
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

WiFiServer server(80);
IPAddress IP(192, 168, 4, 15);
IPAddress mask = (255, 255, 255, 0);

//byte ledPin = 2;  //the built-in led on the wemos d1 mini module

void setup() {
  Serial.begin(9600);
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  // init done

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  display.setRotation(0); // rotate 0 degrees
  //display.setRotation(1); // rotate 90 degrees
  //display.setRotation(2); // rotate 180 degrees
  // display.setRotation(3); // rotate 270 degree
  display.dim(true); //display.dim(false); puts oled back to full brilliance mode
  display.display();


  // Clear the buffer.
  display.clearDisplay();
  display.display();

  WiFi.mode(WIFI_AP);
  WiFi.softAP("Wemos_AP", "Wemos_comm");
  WiFi.softAPConfig(IP, IP, mask);
  server.begin();
  //pinMode(ledPin, OUTPUT);
  Serial.println();
  Serial.println("accesspoint_bare_01.ino");
  Serial.println("Server started.");
  Serial.print("IP: ");     Serial.println(WiFi.softAPIP());
  Serial.print("MAC:");     Serial.println(WiFi.softAPmacAddress());

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Server started");
  display.setCursor(0, 20);
  display.print("IP:  "); display.println(WiFi.softAPIP());
  display.setCursor(0, 30);
  display.print("MAC "); display.println(WiFi.softAPmacAddress());
  display.display();

  delay(5000);
  display.clearDisplay();
  display.display();

}

void loop() {
  //  digitalWrite(ledPin, LOW);
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  String request = client.readStringUntil('\r');
  display.clearDisplay();
  display.display();
  Serial.println("********************************");
  Serial.println("From the station: " + request);
  display.setTextSize(2);
  display.setCursor(0, 0);
  // display.println("Moisture " + request);
  display.println(request);
  display.display();
  client.flush();
  Serial.print("Byte sent to the station: ");
  Serial.println(client.println(request + "Done" + "\r"));
  // digitalWrite(ledPin, HIGH);

}
