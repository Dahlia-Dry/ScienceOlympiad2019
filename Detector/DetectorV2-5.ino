/*
 * Science Olympiad Altoid Tin Temperature Sensor- v2.5
 * @author Dahlia Dry
 * v2 update: display degrees as F and C, in float format
 * v2.5 update: updated conversion function with more sigfigs
 * components:
 * - Arduino Leonardo
 * - 1.44" TFT LCD screen
 * - RGB LED
 * - PT1000 Platinum Resistance Temperature Detector (RTD), 3 wire
 *   Resistance is a function of temperature: 
 *   R = 3.85T + 1000 is the equation given in RTD specs
 *   R = 3.855T + 998.4 is the equation that was experimentally determined
 * - Adafruit MAX31865: PT1000 RTD Temperature Sensor Amplifier
 */
 
//LCD display setup
#include <Adafruit_GFX.h> //include Adafruit graphics library
#include <Adafruit_ST7735.h> //inclue Adafruit ST7735 LCD screen library
#include <SPI.h>
#define TFT_CS     10 //set CS pin to Arduino digital pin 10
#define TFT_RST    9 //set reset pin to Arduino digital pin 9
#define TFT_DC     8 //set DC pin to Arduino digital pin 8
#define TFT_SCLK 7   // set SCLK pin to Arduino digital pin 7
#define TFT_MOSI 6   // set MOSI pin to Arduino digital pin 6

//declare LCD screen object using above pin settings
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

//setup RTD amplifier
#include <Adafruit_MAX31865.h> // Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31865 max = Adafruit_MAX31865(2,3,4,5); 

// The value of the Rref resistor. Use 430.0 4300.0 for PT1000
#define RREF      4300.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 1000.0 for PT1000
#define RNOMINAL  1000.0

//declare additional vars
float temp = 0; //detected temperature
int rotation = 2; //orientation of LCD display
int xPos = 5; //the horizontal pixel count on the LCD
int graphHeightPrevious = 0; //the vertical pixel count on the LCD
int redPin = A0; //initialize red channel of RGB LED to analog pin 0
int greenPin = A1; //initialize green channel of RGB LED to analog pin 1
int bluePin = A2; //initialize blue channel of RGB LED to analog pin 2
int red = 0; //value from 0 to 255 for red channel
int blue = 0; //value from 0 to 255 for blue channel
int green = 0; //value from 0 to 255 for green channel
int resistance = 0; //registered resistance (ohms)
String colors[] = {"blue","green","red"}; //assigned RGB LED colors
float thresholds[] = {20,30}; //threshold temps (C) for RGB to change colors
float tempc = 0; // detected temperature in celsius
String text = ""; //placeholder


void setup() {
  delay(5000); //sometimes necessary to allow proper startup
  max.begin(MAX31865_3WIRE); //initialize the amplifier 
  tft.initR(INITR_144GREENTAB); //initialize the LCD
  uint16_t time = millis(); //start keeping track of time
  tft.setRotation(rotation); //set LCD orientation
  setup_display(); //execute the setup_display function 
}

void loop() {
  uint16_t rtd = max.readRTD(); //read the raw analog RTD value 
  float ratio = rtd;
  ratio /= 32768; //convert to ohms
  resistance = RREF*ratio; //get resistance
  temp=(0.258978* resistance) - 258.483865; //convert resistance to temp (C)
  //equation detemined experimentally with least-squares fit of data
  execute_display(float(temp)); //update the LCD display
  colorIndicator(float(temp), colors, thresholds); //update the LED color
  delay(3000);
}

void drawtext(String text, uint16_t color, uint16_t x0, uint16_t y0){
  tft.setCursor(x0,y0); //start in the upper left corner of the screen
  tft.setTextColor(color); //set the color of the text
  tft.setTextWrap(true);
  tft.print(text); //print the text to the LCD screen
}

//setup live graph of temperature by setting constant label and initial (x,y)
void setup_display(){
  tft.fillScreen(ST7735_WHITE); //erase what was previously on the screen
  drawtext("Temp C/F:",ST7735_BLACK, 3,3); 
  xPos = 0;
  graphHeightPrevious = 0;
}

//execute graph
void execute_display(float temp){
  tft.fillRect(59,3,70,7, ST7735_WHITE); //erase only one area of the screen
  tempc = (9*temp/5) + 32; //convert temp from C to F
  text = String(temp) + "/" + String(tempc);
  drawtext(text,ST7735_BLACK,59,3); //display the temperature in C and F
  int graphHeight = map(temp, 100, 0, 10, 128); //map the temp value to the screen scale
  tft.drawCircle(xPos,graphHeight,2, ST7735_BLUE); //display a dot where the new data point is
  if(xPos > 5){ //draw a line from the previous point to the next point
    tft.drawLine(xPos-5,graphHeightPrevious,xPos,graphHeight,ST7735_RED);
  }
  graphHeightPrevious = graphHeight;
  if(xPos >= 128){ //reset display once graph fills the screen
    xPos = 5;
    tft.fillScreen(ST7735_WHITE);
    drawtext("Temp C/F:",ST7735_BLACK, 3,3);
  }
  else{ //move 5 pixels right for each datapoint
    xPos += 5;
  }
}

//set the color of the RGB LED
void setColor(String color)
{
  //determine values (0-255) for the R, G, B LED channels
  //based on the color name passed in the argument
  if(color == "red"){
    red = 255;
    green = 0;
    blue = 0;
  }
  else if(color == "green"){
    red = 0;
    green = 255;
    blue = 0;
  }
  else if(color == "blue"){
    red = 0;
    green = 0;
    blue = 255;
  }
  else if(color == "yellow"){
    red = 255;
    green = 255;
    blue = 0;
  }
  else if(color == "purple"){
    red = 80;
    green = 0;
    blue = 80;
  }
  else if (color == "aqua"){ 
    red = 0;
    green = 255;
    blue = 255;
  }
  else{
    red=0;
    green=0;
    blue=0;
  }
  red = 255 - red;
  green = 255 - green;
  blue = 255 - blue;
  analogWrite(redPin, red); //adjust the red channel of the RGB LED
  analogWrite(greenPin, green); //adjust the green channel of the RGB LED
  analogWrite(bluePin, blue);  //adjust the blue channel of the RGB LED
}

//decide which temperature thresholds should cause color change in LED
void colorIndicator(float temp, String colors[], float temps[]){
  //test if temp is in lowest range
  if(temp < temps[0]){
    setColor(colors[0]);
  }
  //test if temp is in middle range
  else if(temp > temps[0] and temp < temps[1]){
    setColor(colors[1]);
  }
  //test if temp is in highest range
  else if(temp > temps[1]){
    setColor(colors[2]);
  }
}

