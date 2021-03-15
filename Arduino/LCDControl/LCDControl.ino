#include <UTFT.h>
#include <URTouch.h>
#include <TimerOne.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "ButtonUtils.h"
#include "PumpControlUtils.h"
#include "under_construction.c"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 280
#define ONE_WIRE_BUS A7

// Initialize display
UTFT myGLCD(ILI9341_16, 38, 39, 40, 41);

// Initialize touchscreen
URTouch myTouch(6, 5, 4, 3, 2);

// Setup a OneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass OneWire reference to Dallas Temperature
DallasTemperature temperatureSensors(&oneWire);

// Initialize first flowerpot
flowerpotMetadata flowerpot1{1, 8, A0, false, false, 0, 0, 0, 0};

// Defining variables
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
extern const unsigned int under_construction[];
long x, y;
String currentPage = "HOME";

uint16_t temperatureMeasureIntervallInSec = 10; 
uint16_t temperatureMeasureCounter = 0;

const uint16_t minimumMoisture = 200;
const uint16_t maximumMoisture = 700;

uint16_t yi = 100;
uint16_t measuredMoistureRaw = 100;

bool readSensorWithNextLoopCycle = true;
bool refreshExpectedMoistureDisplayValueOnNextLoopCycle = true;
bool refreshMeasuredMoistureDisplayValueOnNextLoopCycle = true;
bool refreshPumpOnOffButtonOnNextLoopCycle = true;
bool refreshPumpAutoModeButtonOnNextLoopCycle = true;
bool refreshMoistureBarOnNextLoopCycle = true;
bool refreshTemperatureDisplayOnNextLoopCycle = true;
bool refreshThermometerDisplayOnNextLoopCycle = true;

bool homeScreenIsInitialized = false;
bool pumpScreen1IsInitialized = false;
bool underConstructionScreenIsInitialized = false;

// Highlights the button when pressed
void drawFrame(int x1, int y1, int x2, int y2)
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect(x1, y1, x2, y2);
  while (myTouch.dataAvailable()) 
  {
    myTouch.read();
  }
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect(x1, y1, x2, y2);
}

void drawFrame(rectXY rect)
{
  drawFrame(rect.X1, rect.Y1, rect.X2, rect.Y2);
}

void drawHeadline(String moduleName)
{
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont);
  myGLCD.print("IRRIGATION SYSTEM", CENTER, 10);
  myGLCD.setFont(SmallFont);
  myGLCD.print(moduleName, CENTER, 40);
  myGLCD.setColor(0, 255, 0);      // Sets color to green
  myGLCD.drawLine(0, 32, 319, 32); // Draws the green line
}

void drawReturnField()
{
  // RETURN FELD
  myGLCD.setColor(0, 255, 0);                                                                                       // Sets color to green
  myGLCD.fillRoundRect(returnButtonLimits.X1, returnButtonLimits.Y1, returnButtonLimits.X2, returnButtonLimits.Y2); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255);                                                                                   // Sets color to white
  myGLCD.drawRoundRect(returnButtonLimits.X1, returnButtonLimits.Y1, returnButtonLimits.X2, returnButtonLimits.Y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setFont(BigFont);                                                                                          // Sets the font to big
  myGLCD.setBackColor(0, 255, 0);                                                                                   // Sets the background color of the area where the text will be printed to green, same as the button
  myGLCD.print("ret", 25, 70);                                                                                      // Prints the string
}

// HOME SCREEN
void initializeHomeScreen()
{
  drawHeadline("");

  // Button 1
  myGLCD.setColor(0, 255, 0);                                                                                   // Sets color to green
  myGLCD.fillRoundRect(pump1ButtonLimits.X1, pump1ButtonLimits.Y1, pump1ButtonLimits.X2, pump1ButtonLimits.Y2); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255);                                                                               // Sets color to white
  myGLCD.drawRoundRect(pump1ButtonLimits.X1, pump1ButtonLimits.Y1, pump1ButtonLimits.X2, pump1ButtonLimits.Y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setFont(BigFont);                                                                                      // Sets the font to big
  myGLCD.setBackColor(0, 255, 0);                                                                               // Sets the background color of the area where the text will be printed to green, same as the button
  myGLCD.print("SENS 1", pump1ButtonLimits.X1 + 10, pump1ButtonLimits.Y1 + 10);                                 // Prints the string

  // Button 2
  myGLCD.setColor(0, 255, 0);                                                                                   // Sets color to green
  myGLCD.fillRoundRect(pump2ButtonLimits.X1, pump2ButtonLimits.Y1, pump2ButtonLimits.X2, pump2ButtonLimits.Y2); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255);                                                                               // Sets color to white
  myGLCD.drawRoundRect(pump2ButtonLimits.X1, pump2ButtonLimits.Y1, pump2ButtonLimits.X2, pump2ButtonLimits.Y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setFont(BigFont);                                                                                      // Sets the font to big
  myGLCD.setBackColor(0, 255, 0);                                                                               // Sets the background color of the area where the text will be printed to green, same as the button
  myGLCD.print("SENS 2", pump2ButtonLimits.X1 + 10, pump2ButtonLimits.Y1 + 10);                                 // Prints the string

  // Button 3
  myGLCD.setColor(0, 255, 0);                                                                                   // Sets color to green
  myGLCD.fillRoundRect(pump3ButtonLimits.X1, pump3ButtonLimits.Y1, pump3ButtonLimits.X2, pump3ButtonLimits.Y2); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255);                                                                               // Sets color to white
  myGLCD.drawRoundRect(pump3ButtonLimits.X1, pump3ButtonLimits.Y1, pump3ButtonLimits.X2, pump3ButtonLimits.Y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setFont(BigFont);                                                                                      // Sets the font to big
  myGLCD.setBackColor(0, 255, 0);                                                                               // Sets the background color of the area where the text will be printed to green, same as the button
  myGLCD.print("SENS 3", pump3ButtonLimits.X1 + 10, pump3ButtonLimits.Y1 + 10);                                 // Prints the string

  // temperature Display
  myGLCD.setColor(0, 255, 0);                                                                               // Sets color to red
  myGLCD.fillRoundRect(temperatureDisplayLimits.X1, temperatureDisplayLimits.Y1, temperatureDisplayLimits.X2, temperatureDisplayLimits.Y2); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255);                                                                           // Sets color to white
  myGLCD.drawRoundRect(temperatureDisplayLimits.X1, temperatureDisplayLimits.Y1, temperatureDisplayLimits.X2, temperatureDisplayLimits.Y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  refreshTemperatureDisplay();

  // thermometer
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRect(thermometerOuterLimits.X1, thermometerOuterLimits.Y1, thermometerOuterLimits.X2, thermometerOuterLimits.Y2);
  myGLCD.fillCircle((thermometerOuterLimits.X1 + thermometerOuterLimits.X2) / 2, thermometerOuterLimits.Y1, (thermometerOuterLimits.X2 - thermometerOuterLimits.X1) / 2);
  myGLCD.fillCircle((thermometerOuterLimits.X1 + thermometerOuterLimits.X2) / 2, thermometerOuterLimits.Y2 + (thermometerInnerLimits.X2 - thermometerInnerLimits.X1), thermometerOuterLimits.X2 - thermometerOuterLimits.X1);
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillCircle((thermometerOuterLimits.X1 + thermometerOuterLimits.X2) / 2, thermometerOuterLimits.Y2 + (thermometerInnerLimits.X2 - thermometerInnerLimits.X1), thermometerOuterLimits.X2 - thermometerOuterLimits.X1 - 4);
  myGLCD.setFont(SmallFont);

  uint8_t tempScale = 0;
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);                                                                               // Sets the background color of the area where the text will be printed to green, same as the button

  for (uint8_t i = thermometerOuterLimits.Y2; i >= thermometerOuterLimits.Y1; i -= (thermometerOuterLimits.Y2 - thermometerOuterLimits.Y1) / 4)
  {
    myGLCD.printNumI(tempScale, thermometerOuterLimits.X1 - 20, i - 5);
    tempScale += 10;
  }

  refreshThermometerDisplay();

  homeScreenIsInitialized = true;
}

void resetHomeScreenBools()
{
  refreshTemperatureDisplayOnNextLoopCycle = true;
}

void handleHomeScreenInput()
{
  if (myTouch.dataAvailable())
  {
    myTouch.read();
    x = myTouch.getX();
    y = myTouch.getY() * (-1) + 240;

    // PRESS BUTTON "MOT1"
    if (pointIsInsideButtonLimits(x, y, pump1ButtonLimits))
    {
      drawFrame(pump1ButtonLimits);
      currentPage = "MOT1";
      myGLCD.clrScr();
    }
    // PRESS BUTTON "MOT2"
    else if (pointIsInsideButtonLimits(x, y, pump2ButtonLimits))
    {
      drawFrame(pump2ButtonLimits);
      currentPage = "MOT2";
      myGLCD.clrScr();
    }
    // PRESS BUTTON "MOT3"
    else if (pointIsInsideButtonLimits(x, y, pump3ButtonLimits))
    {
      drawFrame(pump3ButtonLimits);
      currentPage = "MOT3";
      myGLCD.clrScr();
    }

    homeScreenIsInitialized = false;
  }
}

void refreshTemperatureDisplay()
{
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 255, 0);
  myGLCD.setFont(BigFont);
  myGLCD.printNumF(temperatureSensors.getTempCByIndex(0), 1, temperatureDisplayLimits.X1 + 10, temperatureDisplayLimits.Y1 + 10);
  //myGLCD.print("°C", temperatureDisplayLimits.X1 + 50, temperatureDisplayLimits.Y1 + 10);
  refreshTemperatureDisplayOnNextLoopCycle = false;
}

void refreshThermometerDisplay()
{
  if  (temperatureSensors.getTempCByIndex(0) < 0 || temperatureSensors.getTempCByIndex(0) > 40)
  {
    myGLCD.setColor(0, 0, 255);
    myGLCD.fillRect(thermometerInnerLimits.X1, thermometerInnerLimits.Y1, thermometerInnerLimits.X2, thermometerInnerLimits.Y2);
  }
  else
  {
    uint16_t temperatureTemp = map(temperatureSensors.getTempCByIndex(0), 0, 40, thermometerOuterLimits.Y2, thermometerOuterLimits.Y1);

    myGLCD.setColor(255, 255, 255);
    myGLCD.fillRect(thermometerInnerLimits.X1, thermometerInnerLimits.Y1, thermometerInnerLimits.X2, temperatureTemp);
    
    myGLCD.setColor(255, 0, 0);
    myGLCD.fillRect(thermometerInnerLimits.X1, temperatureTemp, thermometerInnerLimits.X2, thermometerInnerLimits.Y2);
  }

  for (uint8_t i = thermometerOuterLimits.Y2; i >= thermometerOuterLimits.Y1; i -= (thermometerOuterLimits.Y2 - thermometerOuterLimits.Y1) / 9)
  {
    myGLCD.setColor(255, 255, 255);
    myGLCD.fillRect(thermometerOuterLimits.X1, i - 1, thermometerOuterLimits.X1 + 7, i + 1);
  }
  
  refreshThermometerDisplayOnNextLoopCycle = false;
}

void refreshHomeScreen()
{
  if (refreshTemperatureDisplayOnNextLoopCycle)
  {
    refreshTemperatureDisplay();
  }
  if (refreshThermometerDisplayOnNextLoopCycle)
  {
    refreshThermometerDisplay();
  }
  
}

// UNDER CONSTRUCTION SCREEN
void initializeUnderConstructionScreen()
{
  myGLCD.drawBitmap(20,20,100,100, under_construction);
}

void handleUnderConstructionScreenInput()
{
  if (myTouch.dataAvailable())
  {
    myTouch.read();
    x = myTouch.getX();
    y = myTouch.getY() * (-1) + 240;

    if (x != -1 && y != -1)
    {
      currentPage = "HOME";
      myGLCD.clrScr(); // Clears the screen
    }
    underConstructionScreenIsInitialized = false;
  }
}

// PUMP CONTROL SCREEN
void initializePumpControlScreen()
{
  drawHeadline("PUMP CONTROL");

  myGLCD.setColor(255, 255, 0);
  myGLCD.fillRect(moistureSetterBarButtonLimits.X1, moistureSetterBarButtonLimits.Y1, moistureSetterBarButtonLimits.X2, moistureSetterBarButtonLimits.Y2);

  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(SmallFont);
  myGLCD.print("100%-", 225, 45);
  myGLCD.print(" 80%-", 225, 79);
  myGLCD.print(" 60%-", 225, 113);
  myGLCD.print(" 40%-", 225, 147);
  myGLCD.print(" 20%-", 225, 181);
  myGLCD.print("  0%-", 225, 215);

  drawReturnField();

  pumpScreen1IsInitialized = true;
}

void resetPumpControlScreen1Bools()
{
  refreshExpectedMoistureDisplayValueOnNextLoopCycle = true;
  refreshMeasuredMoistureDisplayValueOnNextLoopCycle = true;
  refreshPumpOnOffButtonOnNextLoopCycle = true;
  refreshPumpAutoModeButtonOnNextLoopCycle = true;
  refreshMoistureBarOnNextLoopCycle = true;
}

void handlePumpControlScreen1Input()
{
  if (myTouch.dataAvailable())
  {
    myTouch.read();
    x = myTouch.getX();
    y = myTouch.getY() * (-1) + 240;

    // PRESS PUMPSWITCH BUTTON
    if (pointIsInsideButtonLimits(x, y, pumpOnOffButtonLimits))
    {
      drawFrame(pumpOnOffButtonLimits);
      togglePumpState(flowerpot1, false);
      refreshPumpOnOffButtonOnNextLoopCycle = true;
      refreshPumpAutoModeButtonOnNextLoopCycle = true;
    }

    // PRESS AUTOSWITCH BUTTON
    if (pointIsInsideButtonLimits(x, y, autoOnOffButtonLimits))
    {
      drawFrame(autoOnOffButtonLimits);
      toggleAutoModeState(flowerpot1, false);
      refreshPumpAutoModeButtonOnNextLoopCycle = true;
      refreshPumpOnOffButtonOnNextLoopCycle = true;
    }

    // SET EXPECTED MOISTURE
    if (pointIsInsideButtonLimits(x, y, moistureSetterBarButtonLimits))
    {
      if (y <= moistureSetterBarButtonLimits.Y1)
      {
        yi = moistureSetterBarButtonLimits.Y1;
      }
      else if (y >= moistureSetterBarButtonLimits.Y2)
      {
        yi = moistureSetterBarButtonLimits.Y2;
      }
      else
      {
        yi = y;
      }

      // calculate calibrated moisture values
      flowerpot1.expectedMoisture = map(yi, moistureSetterBarButtonLimits.Y2, moistureSetterBarButtonLimits.Y1, minimumMoisture, maximumMoisture);
      flowerpot1.expectedMoisturePercent = map(yi, moistureSetterBarButtonLimits.Y2, moistureSetterBarButtonLimits.Y1, 0, 100);

      refreshExpectedMoistureDisplayValueOnNextLoopCycle = true;
      refreshMoistureBarOnNextLoopCycle = true;
    }

    // PRESS RETURN
    if (pointIsInsideButtonLimits(x, y, returnButtonLimits))
    {
      drawFrame(20, 60, 80, 100); // Custom Function -Highlighs the buttons when it's pressed
      currentPage = "HOME";
      myGLCD.clrScr();        // Clears the screen
      pumpScreen1IsInitialized = false;
    }
  }
}

void refreshPumpControlButton()
{
  if (flowerpot1.isOn == false)
  {
    myGLCD.setColor(255, 0, 0);                                                                                                   // Sets color to red
    myGLCD.fillRoundRect(pumpOnOffButtonLimits.X1, pumpOnOffButtonLimits.Y1, pumpOnOffButtonLimits.X2, pumpOnOffButtonLimits.Y2); // Draws filled rounded rectangle
    myGLCD.setColor(255, 255, 255);                                                                                               // Sets color to white
    myGLCD.drawRoundRect(pumpOnOffButtonLimits.X1, pumpOnOffButtonLimits.Y1, pumpOnOffButtonLimits.X2, pumpOnOffButtonLimits.Y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
    myGLCD.setFont(SmallFont);                                                                                                    // Sets the font
    myGLCD.setBackColor(255, 0, 0);                                                                                               // Sets the background color of the area where the text will be printed to green, same as the button
    myGLCD.print("PUMP", 95, 65);
    myGLCD.print("OFF", 95, 95); // Prints the string
  }
  else
  {
    myGLCD.setColor(0, 255, 0);                                                                                                   // Sets color to green
    myGLCD.fillRoundRect(pumpOnOffButtonLimits.X1, pumpOnOffButtonLimits.Y1, pumpOnOffButtonLimits.X2, pumpOnOffButtonLimits.Y2); // Draws filled rounded rectangle
    myGLCD.setColor(255, 255, 255);                                                                                               // Sets color to white
    myGLCD.drawRoundRect(pumpOnOffButtonLimits.X1, pumpOnOffButtonLimits.Y1, pumpOnOffButtonLimits.X2, pumpOnOffButtonLimits.Y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
    myGLCD.setFont(SmallFont);                                                                                                    // Sets the font
    myGLCD.setBackColor(0, 255, 0);                                                                                               // Sets the background color of the area where the text will be printed to green, same as the button
    myGLCD.print("PUMP", 95, 65);
    myGLCD.print("ON", 95, 95); // Prints the string
  }

  refreshPumpOnOffButtonOnNextLoopCycle = false;
}

void refreshAutoModeButton()
{
  if (flowerpot1.isInAutoMode == false)
  {
    myGLCD.setColor(255, 0, 0);                                                                                                   // Sets color to red
    myGLCD.fillRoundRect(autoOnOffButtonLimits.X1, autoOnOffButtonLimits.Y1, autoOnOffButtonLimits.X2, autoOnOffButtonLimits.Y2); // Draws filled rounded rectangle
    myGLCD.setColor(255, 255, 255);                                                                                               // Sets color to white
    myGLCD.drawRoundRect(autoOnOffButtonLimits.X1, autoOnOffButtonLimits.Y1, autoOnOffButtonLimits.X2, autoOnOffButtonLimits.Y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
    myGLCD.setFont(SmallFont);                                                                                                    // Sets the font
    myGLCD.setBackColor(255, 0, 0);                                                                                               // Sets the background color of the area where the text will be printed to green, same as the button
    myGLCD.print("AUTO", 160, 65);
    myGLCD.print("OFF", 160, 95); // Prints the string
  }
  else
  {
    myGLCD.setColor(0, 255, 0);                                                                                                   // Sets color to green
    myGLCD.fillRoundRect(autoOnOffButtonLimits.X1, autoOnOffButtonLimits.Y1, autoOnOffButtonLimits.X2, autoOnOffButtonLimits.Y2); // Draws filled rounded rectangle
    myGLCD.setColor(255, 255, 255);                                                                                               // Sets color to white
    myGLCD.drawRoundRect(autoOnOffButtonLimits.X1, autoOnOffButtonLimits.Y1, autoOnOffButtonLimits.X2, autoOnOffButtonLimits.Y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
    myGLCD.setFont(SmallFont);                                                                                                    // Sets the font
    myGLCD.setBackColor(0, 255, 0);                                                                                               // Sets the background color of the area where the text will be printed to green, same as the button
    myGLCD.print("AUTO", 160, 65);
    myGLCD.print("ON", 160, 95); // Prints the string
  }

  refreshPumpAutoModeButtonOnNextLoopCycle = false;
}

void refreshExpectedMoistureValue()
{
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(20, 120, 120, 179);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont);
  myGLCD.print("expected:", 20, 120);
  myGLCD.printNumI(flowerpot1.expectedMoisture, 20, 140, 3);
  myGLCD.printNumI(flowerpot1.expectedMoisturePercent, 20, 160, 3);
  myGLCD.printChar('%', 70, 160);

  refreshExpectedMoistureDisplayValueOnNextLoopCycle = false;
}

void refreshMeasuredMoistureValue()
{
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(20, 180, 120, 239);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont);
  myGLCD.print("measured:", 20, 180);
  myGLCD.printNumI(flowerpot1.measuredMoisture, 20, 200, 3);
  myGLCD.printNumI(flowerpot1.measuredMoisturePercent, 20, 220, 3);
  myGLCD.printChar('%', 70, 220);

  refreshMeasuredMoistureDisplayValueOnNextLoopCycle = false;
}

void refreshMoistureBar()
{

  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRect(moistureSetterBarButtonLimits.X1, yi + 2, moistureSetterBarButtonLimits.X2, yi - 2);
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(moistureSetterBarButtonLimits.X1, measuredMoistureRaw + 2, moistureSetterBarButtonLimits.X2, measuredMoistureRaw - 2);
  myGLCD.setColor(255, 255, 0);
  myGLCD.fillRect(moistureSetterBarButtonLimits.X1, moistureSetterBarButtonLimits.Y1 - 2, moistureSetterBarButtonLimits.X2, min(yi, measuredMoistureRaw) - 2);
  myGLCD.fillRect(moistureSetterBarButtonLimits.X1, max(yi, measuredMoistureRaw) + 2, moistureSetterBarButtonLimits.X2, moistureSetterBarButtonLimits.Y2 + 2);
  myGLCD.fillRect(moistureSetterBarButtonLimits.X1, min(yi, measuredMoistureRaw) + 2, moistureSetterBarButtonLimits.X2, max(yi, measuredMoistureRaw) - 2);

  /*
  if ((expectedMoistureTemp >= measuredMoistureRaw) && (expectedMoistureTemp <= (measuredMoistureRaw + 5)))
  {
    myGLCD.setColor(255, 0, 0);
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, expectedMoistureTemp, moistureSetterBarButtonLimits.X2, (expectedMoistureTemp + 2)); // positioner

    myGLCD.setColor(255, 255, 255);
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, expectedMoistureTemp + 3, moistureSetterBarButtonLimits.X2, (expectedMoistureTemp + 5)); // positioner

    myGLCD.setColor(255, 255, 0);
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, moistureSetterBarButtonLimits.Y1, moistureSetterBarButtonLimits.X2, (expectedMoistureTemp - 1));
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, (expectedMoistureTemp + 6), moistureSetterBarButtonLimits.X2, moistureSetterBarButtonLimits.Y2);
  }
  else if ((measuredMoistureRaw >= expectedMoistureTemp) && (measuredMoistureRaw <= (expectedMoistureTemp + 5)))
  {
    myGLCD.setColor(255, 255, 255);
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, measuredMoistureRaw, moistureSetterBarButtonLimits.X2, (measuredMoistureRaw + 2)); // positioner

    myGLCD.setColor(255, 0, 0);
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, measuredMoistureRaw + 3, moistureSetterBarButtonLimits.X2, (measuredMoistureRaw + 5)); // positioner

    myGLCD.setColor(255, 255, 0);
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, moistureSetterBarButtonLimits.Y1, moistureSetterBarButtonLimits.X2, (measuredMoistureRaw - 1));
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, (measuredMoistureRaw + 6), moistureSetterBarButtonLimits.X2, moistureSetterBarButtonLimits.Y2);
  }
  else
  {
    // Draw bar with positioner
    myGLCD.setColor(255, 255, 255);
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, expectedMoistureTemp, moistureSetterBarButtonLimits.X2, (expectedMoistureTemp + 4)); // positioner

    myGLCD.setColor(255, 0, 0);
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, measuredMoistureRaw, moistureSetterBarButtonLimits.X2, (measuredMoistureRaw + 4)); // positioner

    myGLCD.setColor(255, 255, 0);
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, moistureSetterBarButtonLimits.Y1, moistureSetterBarButtonLimits.X2, (min(measuredMoistureRaw, expectedMoistureTemp)));
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, (min(measuredMoistureRaw, expectedMoistureTemp) + 5), moistureSetterBarButtonLimits.X2, (max(measuredMoistureRaw, expectedMoistureTemp) - 1));
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, (max(measuredMoistureRaw, expectedMoistureTemp) + 5), moistureSetterBarButtonLimits.X2, moistureSetterBarButtonLimits.Y2 + 5);
  }
  */
  refreshMoistureBarOnNextLoopCycle = false;
}

void refreshPumpControlScreen()
{
  if (refreshPumpOnOffButtonOnNextLoopCycle)
  {
    refreshPumpControlButton();
  }

  if (refreshPumpAutoModeButtonOnNextLoopCycle)
  {
    refreshAutoModeButton();
  }

  if (refreshExpectedMoistureDisplayValueOnNextLoopCycle)
  {
    refreshExpectedMoistureValue();
  }

  if (refreshMeasuredMoistureDisplayValueOnNextLoopCycle)
  {
    refreshMeasuredMoistureValue();
  }

  if (refreshMoistureBarOnNextLoopCycle)
  {
    refreshMoistureBar();
  }
}

void timer_isr()
{
  readSensorWithNextLoopCycle = true;
  temperatureMeasureCounter++;
}

void readMoistureSensor()
{
  flowerpot1.measuredMoisture = analogRead(flowerpot1.sensorPin);
}

void proceedSensorData()
{
  measuredMoistureRaw = map(flowerpot1.measuredMoisture, minimumMoisture, maximumMoisture, moistureSetterBarButtonLimits.Y1, moistureSetterBarButtonLimits.Y2);

  if (measuredMoistureRaw < moistureSetterBarButtonLimits.Y1)
  {
    measuredMoistureRaw = moistureSetterBarButtonLimits.Y1;
  }
  else if (measuredMoistureRaw > moistureSetterBarButtonLimits.Y2)
  {
    measuredMoistureRaw = moistureSetterBarButtonLimits.Y2;
  }

  flowerpot1.measuredMoisturePercent = map(measuredMoistureRaw, moistureSetterBarButtonLimits.Y2, moistureSetterBarButtonLimits.Y1, 0, 100);
}

void requestTemperature()
{
  temperatureSensors.requestTemperatures();

  refreshTemperatureDisplayOnNextLoopCycle = true;
  refreshThermometerDisplayOnNextLoopCycle = true;
  temperatureMeasureCounter = 0;
}

void setup()
{
  Serial.begin(9600);
  temperatureSensors.begin();
  temperatureSensors.requestTemperatures();

  Timer1.initialize(1000000);
  Timer1.attachInterrupt(timer_isr);

  pinMode(flowerpot1.actorPin, OUTPUT);
  myGLCD.InitLCD();
  myGLCD.clrScr();

  myTouch.InitTouch();
  myTouch.setPrecision(PREC_HI);

  initializeHomeScreen(); // Draws the Home Screen
}

void loop()
{
  if (flowerpot1.isInAutoMode)
  {
    if (flowerpot1.isSoilTooDry())
    {
      startPump(flowerpot1);
    }
    else
    {
      stopPump(flowerpot1);
    }
  }

  if (readSensorWithNextLoopCycle)
  {
    readMoistureSensor();
    proceedSensorData();
    readSensorWithNextLoopCycle = false;
    refreshMeasuredMoistureDisplayValueOnNextLoopCycle = true;
    refreshMoistureBarOnNextLoopCycle = true;
  }

  if (temperatureMeasureCounter >= temperatureMeasureIntervallInSec)
  {
    requestTemperature();
  }

  // Home Screen
  if (currentPage == "HOME")
  {
    if (!homeScreenIsInitialized)
    {
      initializeHomeScreen();
    }

    handleHomeScreenInput();

    refreshHomeScreen();
  }
  
  else if (currentPage == "MOT1")
  {
    if (!pumpScreen1IsInitialized)
    {
      initializePumpControlScreen();
      resetPumpControlScreen1Bools();
    }

    handlePumpControlScreen1Input();

    refreshPumpControlScreen();
  }

  else if (currentPage == "MOT2" || currentPage == "MOT3")
  {
    if (!underConstructionScreenIsInitialized)
    {
      initializeUnderConstructionScreen();
    }

    handleUnderConstructionScreenInput();
  }
}
