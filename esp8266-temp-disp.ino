#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*
*  Display and BME280 (Beide auf ein I2C)
 * change Adafruit_SSD1306.h
 * comment in  #define SSD1306_128_64
 *  
 *  SCL - D1
 *  SDA - D2
 */

// THERE IS NO RESET 
// OLED 128x64 (character 5x8) 64 buchstaben
#define OLED_RESET D0
Adafruit_SSD1306 display(OLED_RESET);

 
#include <DHT.h>
#include <DHT_U.h>


// Temparature sensor
// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
// see https://randomnerdtutorials.com/esp8266-dht11dht22-temperature-and-humidity-web-server-with-arduino-ide/


#define DHTPIN 12    // means D6

#define DHTTYPE DHT11   // DHT 11

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);

float emaLongTemp = -100.0;
float emaShortTemp = -100.0;
float emaLongHum = -100.0;
float emaShortHum = -100.0;

// 300 entspricht 5*300 Sekunden = (Siehe delay)
#define EWSHORT (2.0/(1.0+300.0))
#define EWLONG (2.0/(1.0+20.0))

void setup() {
  Serial.begin(115200);
  Serial.println("Temp/Hum setup");
  delay(100);

  pinMode(DHTPIN, INPUT);
  dht.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}




void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.println(" *C ");

  if (emaLongHum==-100.0) {
    emaLongHum = h;
    emaShortHum = h;
    emaLongTemp = t;
    emaShortTemp = t;
  } else {
    emaLongHum = (h-emaLongHum)*EWLONG + emaLongHum;
    emaShortHum = (h-emaShortHum)*EWSHORT + emaShortHum;
    emaLongTemp = (t-emaLongTemp)*EWLONG + emaLongTemp;
    emaShortTemp = (t-emaShortTemp)*EWSHORT + emaShortTemp;
  }

  displayTempHum(t,h);
}

float d_t, d_h;

void displayTempHum(float t, float h) {
  if (t==d_t && h==d_h) {
    return;
  }
  
  d_t = t;
  d_h = h;
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  // breite 8 Zeichen macht 80 pixel
  display.print("T ");
  display.print(String(t,1));
  display.println("C");
  display.print("H ");
  display.print(String(h,1));
  display.println("%");
  
  //  Zeige tendenz in 50 minuten
  display.setTextSize(1);
  display.setCursor(90,1);
  display.print(String((emaShortTemp-emaLongTemp)*10.0,2));

  display.setCursor(90,17);
  display.print(String((emaShortHum-emaLongHum)*10,2));

  display.setCursor(0,33);
  display.println("Wesolych Swiat");
  display.println("Frohe Weihnachten");

  display.display();  
}
