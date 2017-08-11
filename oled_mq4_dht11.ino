#include <ESP8266WiFi.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <gfxfont.h>
#include "DHT.h"

#include <SPI.h> //Library for SPI interface 
#include <Wire.h> //Library for I2C interface 

#define OLED_RESET 0 //Reset pin 
Adafruit_SSD1306 display(OLED_RESET); //Set Reset pin for OLED display

#define SSD1306_LCDHEIGHT 64
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

/*DHT11*/
#define DHTPIN D7
#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);
/*DHT11*/




int led = D2; //LED pin
int buzzer = D5; //Buzzer pin
int gas_sensor = A0; //Sensor pin
float m = -0.318; //Slope
float b = 1.133; //Y-Intercept
float R0 = 11.820; //Sensor Resistance in fresh air from previous code





void setup() {
  
  Serial.begin(9600); //Baud rate
  Wire.begin();
  
/*OLED*/
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //Initialize screen
  display.display();
  delay(1000);
  display.setTextSize(1);
  display.setTextColor(WHITE); //Set text color
/*OLED*/
  
    
  /* Alarm Pins */
  pinMode(led, OUTPUT); //Set LED as output
  digitalWrite(led, LOW); //Turn LED off
  pinMode(buzzer, OUTPUT); //Set buzzer as output
  digitalWrite(buzzer, LOW); // Turn buzzer off
  pinMode(gas_sensor, INPUT); //Set gas sensor as input
  /* Alarm Pins */



void loop() {

  display.clearDisplay(); //Clear display
  display.setCursor(0, 5); //Place cursor in (x,y) location
  
  /*MQ-4*/
  float sensor_volt; //Define variable for sensor voltage
  float RS_gas; //Define variable for sensor resistance
  float ratio; //Define variable for ratio
  float sensorValue = analogRead(gas_sensor); //Read analog values of sensor
  sensor_volt = sensorValue * (5.0 / 1023.0); //Convert analog values to voltage
  RS_gas = ((5.0 * 10.0) / sensor_volt) - 10.0; //Get value of RS in a gas
  ratio = RS_gas / R0;   // Get ratio RS_gas/RS_air

  double ppm_log = (log10(ratio) - b) / m; //Get ppm value in linear scale according to the the ratio value
  double ppm = pow(10, ppm_log); //Convert ppm value to log scale
  double percentage = ppm / 10000; //Convert to percentage
  /*MQ-4*/

  /*DHT-11*/
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

   /*DHT-11*/
 

 
  
  /*serial MQ-4, DHT-11*/
  Serial.print(ppm);
  Serial.println("ppm");
  Serial.print(percentage);
  Serial.println("%");
  delay(1000);
  Serial.print("Current temp: ");
  Serial.print(t);
  Serial.println("C");
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println("%");
 

  

  
/*OLED MQ-4 and DHT-11*/
  display.print(ppm);
  display.println("ppm");
  display.print(percentage); //Load screen buffer with percentage MQ-4 value
  display.println("% CH4"); //Load screen buffer with  MQ-4 "%"
  //display.println("Temp."); //Load screen buffer with  DHT-11 temperature
  display.print("\n");
  display.print(t);
  display.print((char)247);
  display.print(" C  ");
  display.print(h); 
  display.println("% h");
  display.println();
  display.display(); //Flush characters to screen
/*OLED MQ-4 and DHT-11*/

 /* Alarms */
  
  if (ppm > 120) {
    //Check if ppm value is greater than 2000
    //digitalWrite(led, HIGH); //Turn LED on
    digitalWrite(buzzer, HIGH); //Turn buzzer on
  } else {
    //Case ppm is not greater than 2000
   // digitalWrite(led, LOW);
    //Turn LED off
    digitalWrite(buzzer, LOW);
    //Turn buzzer off

     Serial.println();
  }

 if (t > 32) {
    //Check if temp value is greater than 32
    //digitalWrite(led, HIGH); //Turn LED on
    digitalWrite(buzzer, HIGH); //Turn buzzer on
    delay(500);  
    digitalWrite(buzzer, LOW); //Turn buzzer on   
    delay(500);  
  } else {
     //Check if temp value not greater than 32
   // digitalWrite(led, LOW);
    //Turn LED off
    digitalWrite(buzzer, LOW);
    //Turn buzzer off

     Serial.println();
  }
  

   /* Alarms */






}