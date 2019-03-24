/*
 * @author Dahlia Dry
 * Science Olympiad Altoid Tin Detector- beta version
 */
//LCD display setup
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#define TFT_CS     10
#define TFT_RST    9 
#define TFT_DC     8
#define TFT_SCLK 7   // set these to be whatever pins you like!
#define TFT_MOSI 6   // set these to be whatever pins you like!
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

//setup temp sensor
#include <Adafruit_MAX31865.h>// Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31865 max = Adafruit_MAX31865(2,3,4,5);
// use hardware SPI, just pass in the CS pin
//Adafruit_MAX31865 max = Adafruit_MAX31865(10);

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      4300.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  1000.0

//declare additional vars
float temp = 0;
float tempprevious = 0;
int rotation = 2; 
int xPos = 5;
int graphHeightPrevious = 0;
int redPin = A0;
int greenPin = A1;
int bluePin = A2;
int red = 0;
int blue = 0;
int green = 0;
String colors[] = {"blue","green","red"};
float thresholds[] = {20,30};


void setup() {
  delay(5000);
  Serial.begin(115200);
  max.begin(MAX31865_3WIRE);  // set to 2WIRE or 4WIRE as necessary
  tft.initR(INITR_144GREENTAB);
  uint16_t time = millis();
  tft.setRotation(rotation);
  setup_display();
}

void loop() {
  uint16_t rtd = max.readRTD();
  float ratio = rtd;
  ratio /= 32768;
  temp = int(max.temperature(RNOMINAL, RREF));
  execute_display(temp);
  tempprevious = temp;
  colorIndicator(float(temp), colors, thresholds);
  delay(3000);
}

void drawtext(String text, uint16_t color, uint16_t x0, uint16_t y0){
  tft.setCursor(x0,y0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

//setup live graph of temperature by setting constant label and initial (x,y)
void setup_display(){
  tft.fillScreen(ST7735_WHITE);
  drawtext("Temperature (C):",ST7735_BLACK, 3,3);
  xPos = 0;
  graphHeightPrevious = 0;
}

//execute graph
void execute_display(int temp){
  tft.fillRect(99,3,40,7, ST7735_WHITE);
  drawtext(String(temp),ST7735_BLACK,99,3);
  int graphHeight = map(temp, 100, 0, 10, 128);
  tft.drawCircle(xPos,graphHeight,2, ST7735_BLUE);
  if(xPos > 5){
    tft.drawLine(xPos-5,graphHeightPrevious,xPos,graphHeight,ST7735_RED);
  }
  graphHeightPrevious = graphHeight;
  if(xPos >= 128){
    xPos = 5;
    tft.fillScreen(ST7735_WHITE);
    drawtext("Temperature (C):",ST7735_BLACK, 0,0);
  }
  else{
    xPos += 5;
  }
}

void setColor(String color)
{
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
  else if(color.equals("purple")){
    red = 80;
    green = 0;
    blue = 80;
  }
  else{
    red = 0;
    green = 255;
    blue = 255;
  }
  red = 255 - red;
  green = 255 - green;
  blue = 255 - blue;
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);  
}

void colorIndicator(float temp, String colors[], float temps[]){
  if(temp < temps[0]){
    Serial.print("yello");
    setColor(colors[0]);
  }
  else if(temp > temps[0] and temp < temps[1]){
    Serial.print("mello");
    setColor(colors[1]);
  }
  else if(temp > temps[1]){
    Serial.println(temp);
    Serial.println(temps[1]);
    Serial.println("jello");
    setColor(colors[2]);
  }
}

