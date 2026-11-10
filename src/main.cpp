//inlcuded libraries
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


//connecting pin 2 to humidity sensor DHT22
#define DHT_PIN 2
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);
float temperature = 0.0;
float humidity = 0.0;
float targetHumidity = 50;

//initializing LED (placeholder atomizer and solenoid)
#define LED_PIN 3
#define LED_SOL_PIN 8

//initializing screen
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//initializing buttons
#define BUTTON_PIN 4
bool buttonState = false;
bool lastButtonState = false;
bool systemOn = false;

#define plusBUTTON_PIN 5
bool plusButtonState = false;
bool plusLastButtonState = false;

#define minusBUTTON_PIN 6
bool minusButtonState = false;
bool minusLastButtonState = false;

#define WATERSENSOR_PIN 7
int waterState = -1;


//declaring fucntions
void powerButtonLogic();
void checkHumidity();
void updateDisplay();
void readSensor();
void targetHumidityController();
void atomizerController();
void solenoidControl ();

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

  pinMode(LED_PIN, OUTPUT); // initializing LED (PIN, mode) "Atomizer"

  pinMode(LED_SOL_PIN, OUTPUT); // initializing LED (PIN,mode) "Solenoid"

  pinMode(BUTTON_PIN, INPUT_PULLUP); // initializing powerbutton (PIN, mode)

  pinMode(plusBUTTON_PIN,INPUT_PULLUP); // plus button

  pinMode(minusBUTTON_PIN,INPUT_PULLUP); // minus button

  pinMode(WATERSENSOR_PIN, INPUT_PULLUP); // water sensor

}


//  Main code begins
void loop() {
  powerButtonLogic();

  if(systemOn == true){ // checks if humidifier is on
    readSensor();
    updateDisplay();
    targetHumidityController();
    solenoidControl ();
    atomizerController();

  } else { // humidifier off
    display.clearDisplay();
    display.display();
    digitalWrite(LED_PIN, LOW); // LED turns off
  }
}
  
void powerButtonLogic(){
  // millis() is used to make sure one button press is counted as one press
  static unsigned long lastButtonCheck = 0; // static -> only sets once
  const unsigned long checkInterval = 50; // interval of checks, in milliseconds
  unsigned long currentMillis = millis(); // milliseconds

  if (currentMillis - lastButtonCheck >= checkInterval){ // checks if time interval has been elapsed
    lastButtonCheck = currentMillis; // set up for next loop
    buttonState = digitalRead(BUTTON_PIN); // reads the state of the button
    if (buttonState == LOW && lastButtonState == HIGH){ // detects the "rising edge" = button press
      if (systemOn == false){ // if humidifier is off
        systemOn = true; // turn humidifier on
      } else { // if humidifier is on
        systemOn = false; //turn humidifier off
    }
  }
  lastButtonState = buttonState; // keeps track of the last button value for loop
  }
}



void updateDisplay(){

display.clearDisplay();

  // displays temperature
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

   // displays target humidity
   display.setCursor(0,32);
   display.print("Target: ");
   display.print(targetHumidity);
   display.print("%");

   // displays water state
   display.setCursor(0,48);
   display.print("Water state: ");
   display.print(waterState);

   display.display();
}

void readSensor(){
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
}

void atomizerController(){
if(humidity < targetHumidity && waterState == HIGH){
    digitalWrite(LED_PIN, HIGH); // LED turns on when humidity below desired limit
  } else if (humidity > targetHumidity + 5){ // 5% hysteresis so atomizer doesn't toggle on/off repeatedly
    digitalWrite(LED_PIN, LOW); // LED turns off when humidity level is reached
  }
}

void targetHumidityController() {
  // millis() is used to make sure one button press is counted as one press
  static unsigned long lastButtonCheck = 0; // static -> only sets once
  const unsigned long checkInterval = 50; // interval of checks, in milliseconds
  unsigned long currentMillis = millis(); // milliseconds

  if (currentMillis - lastButtonCheck >= checkInterval){ // checks if time interval has been elapsed
    lastButtonCheck = currentMillis; // set up for next loop
    plusButtonState = digitalRead(plusBUTTON_PIN); // reads the state of the button
    minusButtonState = digitalRead(minusBUTTON_PIN); // reads the state of the button
    if (plusButtonState == LOW && plusLastButtonState == HIGH){ // detects the "rising edge" = button press
      targetHumidity++; // adds 1 % to the target humidity
    }
  plusLastButtonState = plusButtonState; // keeps track of the last button value for loop

    if (minusButtonState == LOW && minusLastButtonState == HIGH){ // detects the "rising edge" = button press
      targetHumidity--; // adds 1 % to the target humidity
    }
  minusLastButtonState = minusButtonState; // keeps track of the last button value for loop
  }
}

void solenoidControl (){
  waterState = digitalRead(WATERSENSOR_PIN);
  Serial.println(waterState);
  if (waterState == LOW){             // water level is lower than desired
    digitalWrite(LED_SOL_PIN, HIGH);  // open solenoid
  } 
  else if (waterState == HIGH){       // water level is at the desired level
    digitalWrite(LED_SOL_PIN, LOW);   // close solenoid
  }
}
