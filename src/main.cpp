//inlcuded libraries
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


//connecting pin 2 to humidity sensor DHT22
#define DHT_PIN 2
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);

//initializing LED (placeholder atomizer)
#define LED_PIN 3

//initializing screen
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//initializing powerbutton
#define BUTTON_PIN 4
boolean buttonState = false;
boolean lastButtonState = false;
boolean systemOn = false;

//declating fucntions
void powerButtonLogic();

//setup code block, runs on arduino start and reset
void setup() {
  Serial.begin(9600); // serial communication between Arduino and PC

  dht.begin(); // initializing the humidity sensor

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)){ // initiazlizing the screen (power config, I2C address)
    while (true);  // if init fails -> infinite loop, no progression
  }

  display.clearDisplay();
  display.setTextSize(1); //font size
  display.setTextColor(SSD1306_WHITE); //font colour
  display.cp437(true); // enables using cp437 character set (needed for degree sign)

  pinMode(LED_PIN, OUTPUT); // initializing LED (PIN, mode)

  pinMode(BUTTON_PIN, INPUT_PULLUP); // initializing powerbutton (PIN, mode)
}


//  Main code begins
void loop() {
  powerButtonLogic();

  if(systemOn == true){ // checks if humidifier is on
  // declaring variables
  float humidity = dht.readHumidity(); // reads the humidity from the sensor
  float temperature = dht.readTemperature(); // reads the temperature from the sensor

  // displays temperature
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Temperature: ");
  display.print(temperature);
  display.print(" ");
  display.write(0xF8);
  display.print("C");

  // displays humidity
  display.setCursor(0, 16);
  display.print("Humidity: ");
  display.print(humidity);
  display.print("%");
  display.display();

  // 
  if(humidity < 50){
    digitalWrite(LED_PIN, HIGH); // LED turns on when humidity below desired limit
  } else if (humidity > 55){ // 5% hysteresis so atomizer doesn't toggle on/off repeatedly
    digitalWrite(LED_PIN, LOW); // LED turns off when humidity level is reached
  }


  // loops (updates) every 2 seconds
  // delay(2000);
  } else { // humidifier off
    display.clearDisplay();
    display.display();
  }
}  

void powerButtonLogic(){
    // power button code block
  buttonState = digitalRead(BUTTON_PIN); // reads the state of the button
  if (buttonState == LOW && lastButtonState == HIGH){ // detects the "rising edge" = button press
    if (systemOn == false){ // if humidifier is off
      systemOn = true; // turn humidifier on
    } else { // if humidifier is on
      systemOn = false; //turn humidifier off
    }
   delay (50); // makes sure 1 press is counted as 1
  }
  lastButtonState = buttonState; // keeps track of the last button value for loop
}

void checkHumidity(){
 // millis();?
}

void updateDisplay(){

}