#include <UTFT.h>
#include <URTouch.h>
#include <TimerOne.h>
#include "ButtonUtils.h"
#include "PumpControlUtils.h"
#include "assets/under_construction.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 280

// Initialize display
UTFT myGLCD(ILI9341_16, 38, 39, 40, 41);

// Initialize touchscreen
URTouch myTouch(6, 5, 4, 3, 2);

// Initialize Pump Control Data
pumpMetadata pump1{1, 8, A0, false, false};

// Defining variables
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
extern unsigned int under_construction[];
long x, y;
String currentPage = "HOME";

uint16_t expectedMoisture = 0;
uint16_t expectedMoisturePercent = 0;
uint16_t measuredMoisture = 0;
uint16_t measuredMoisturePercent = 0;
const uint16_t minimumMoisture = 200;
const uint16_t maximumMoisture = 700;

uint16_t yi = 100;
uint16_t measuredMoistureTemp = 100;

bool readSensorWithNextLoopCycle = true;
bool refreshExpectedMoistureDisplayValueOnNextLoopCycle = true;
bool refreshMeasuredMoistureDisplayValueOnNextLoopCycle = true;
bool refreshPumpOnOffButtonOnNextLoopCycle = true;
bool refreshPumpAutoModeButtonOnNextLoopCycle = true;
bool refreshMoistureBarOnNextLoopCycle = true;

bool homeScreenIsInitialized = false;
bool pumpScreen1IsInitialized = false;
bool loveScreenIsInitialized = false;
bool underConstructionScreenIsInitialized = false;

// Highlights the button when pressed
void drawFrame(int x1, int y1, int x2, int y2)
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect(x1, y1, x2, y2);
  while (myTouch.dataAvailable())
    myTouch.read();
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

void initializeLoveScreen()
{
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setColor(255, 0, 0);
  myGLCD.setFont(BigFont);
  myGLCD.print("*lieb*", CENTER, 50);

  myGLCD.setFont(SmallFont);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("*press anywhere to return*", CENTER, 200);

  loveScreenIsInitialized = false;
}

void resetLoveScreenBools()
{
}

void handleLoveScreenInput()
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
    loveScreenIsInitialized = false;
  }
}

void refreshLoveScreen()
{
}

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

  // LOVE BUTTON
  myGLCD.setColor(255, 0, 0);                                                                               // Sets color to red
  myGLCD.fillRoundRect(loveButtonLimits.X1, loveButtonLimits.Y1, loveButtonLimits.X2, loveButtonLimits.Y2); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255);                                                                           // Sets color to white
  myGLCD.drawRoundRect(loveButtonLimits.X1, loveButtonLimits.Y1, loveButtonLimits.X2, loveButtonLimits.Y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setFont(BigFont);                                                                                  // Sets the font to big
  myGLCD.setBackColor(255, 0, 0);                                                                           // Sets the background color of the area where the text will be printed to red, same as the button
  myGLCD.print("<3", loveButtonLimits.X1 + 10, loveButtonLimits.Y1 + 10);                                   // Prints the string

  homeScreenIsInitialized = true;
}

void resetHomeScreenBools()
{
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
    // PRESS LOVE BUTTON
    else if (pointIsInsideButtonLimits(x, y, loveButtonLimits))
    {
      drawFrame(loveButtonLimits);
      currentPage = "LOVE";
      myGLCD.clrScr();
    }

    homeScreenIsInitialized = false;
  }
}

void refreshHomeScreen()
{

}

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

void refreshUnderConstructionScreen()
{

}
  

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
      togglePumpState(pump1, false);
      refreshPumpOnOffButtonOnNextLoopCycle = true;
      refreshPumpAutoModeButtonOnNextLoopCycle = true;
    }

    // PRESS AUTOSWITCH BUTTON
    if (pointIsInsideButtonLimits(x, y, autoOnOffButtonLimits))
    {
      drawFrame(autoOnOffButtonLimits);
      toggleAutoModeState(pump1, false);
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
      expectedMoisture = map(yi, moistureSetterBarButtonLimits.Y2, moistureSetterBarButtonLimits.Y1, minimumMoisture, maximumMoisture);
      expectedMoisturePercent = map(yi, moistureSetterBarButtonLimits.Y2, moistureSetterBarButtonLimits.Y1, 0, 100);

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
  if (pump1.isOn == false)
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
  if (pump1.isInAutoMode == false)
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
  myGLCD.printNumI(expectedMoisture, 20, 140, 3);
  myGLCD.printNumI(expectedMoisturePercent, 20, 160, 3);
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
  myGLCD.printNumI(measuredMoisture, 20, 200, 3);
  myGLCD.printNumI(measuredMoisturePercent, 20, 220, 3);
  myGLCD.printChar('%', 70, 220);

  refreshMeasuredMoistureDisplayValueOnNextLoopCycle = false;
}

void refreshMoistureBar()
{

  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRect(moistureSetterBarButtonLimits.X1, yi + 2, moistureSetterBarButtonLimits.X2, yi - 2);
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(moistureSetterBarButtonLimits.X1, measuredMoistureTemp + 2, moistureSetterBarButtonLimits.X2, measuredMoistureTemp - 2);
  myGLCD.setColor(255, 255, 0);
  myGLCD.fillRect(moistureSetterBarButtonLimits.X1, moistureSetterBarButtonLimits.Y1 - 2, moistureSetterBarButtonLimits.X2, min(yi, measuredMoistureTemp) - 2);
  myGLCD.fillRect(moistureSetterBarButtonLimits.X1, max(yi, measuredMoistureTemp) + 2, moistureSetterBarButtonLimits.X2, moistureSetterBarButtonLimits.Y2 + 2);
  myGLCD.fillRect(moistureSetterBarButtonLimits.X1, min(yi, measuredMoistureTemp) + 2, moistureSetterBarButtonLimits.X2, max(yi, measuredMoistureTemp) - 2);

  /*
  if ((expectedMoistureTemp >= measuredMoistureTemp) && (expectedMoistureTemp <= (measuredMoistureTemp + 5)))
  {
    myGLCD.setColor(255, 0, 0);
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, expectedMoistureTemp, moistureSetterBarButtonLimits.X2, (expectedMoistureTemp + 2)); // positioner

    myGLCD.setColor(255, 255, 255);
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, expectedMoistureTemp + 3, moistureSetterBarButtonLimits.X2, (expectedMoistureTemp + 5)); // positioner

    myGLCD.setColor(255, 255, 0);
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, moistureSetterBarButtonLimits.Y1, moistureSetterBarButtonLimits.X2, (expectedMoistureTemp - 1));
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, (expectedMoistureTemp + 6), moistureSetterBarButtonLimits.X2, moistureSetterBarButtonLimits.Y2);
  }
  else if ((measuredMoistureTemp >= expectedMoistureTemp) && (measuredMoistureTemp <= (expectedMoistureTemp + 5)))
  {
    myGLCD.setColor(255, 255, 255);
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, measuredMoistureTemp, moistureSetterBarButtonLimits.X2, (measuredMoistureTemp + 2)); // positioner

    myGLCD.setColor(255, 0, 0);
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, measuredMoistureTemp + 3, moistureSetterBarButtonLimits.X2, (measuredMoistureTemp + 5)); // positioner

    myGLCD.setColor(255, 255, 0);
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, moistureSetterBarButtonLimits.Y1, moistureSetterBarButtonLimits.X2, (measuredMoistureTemp - 1));
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, (measuredMoistureTemp + 6), moistureSetterBarButtonLimits.X2, moistureSetterBarButtonLimits.Y2);
  }
  else
  {
    // Draw bar with positioner
    myGLCD.setColor(255, 255, 255);
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, expectedMoistureTemp, moistureSetterBarButtonLimits.X2, (expectedMoistureTemp + 4)); // positioner

    myGLCD.setColor(255, 0, 0);
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, measuredMoistureTemp, moistureSetterBarButtonLimits.X2, (measuredMoistureTemp + 4)); // positioner

    myGLCD.setColor(255, 255, 0);
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, moistureSetterBarButtonLimits.Y1, moistureSetterBarButtonLimits.X2, (min(measuredMoistureTemp, expectedMoistureTemp)));
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, (min(measuredMoistureTemp, expectedMoistureTemp) + 5), moistureSetterBarButtonLimits.X2, (max(measuredMoistureTemp, expectedMoistureTemp) - 1));
    myGLCD.fillRect(moistureSetterBarButtonLimits.X1, (max(measuredMoistureTemp, expectedMoistureTemp) + 5), moistureSetterBarButtonLimits.X2, moistureSetterBarButtonLimits.Y2 + 5);
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
}

void readSensor()
{
  measuredMoisture = analogRead(pump1.sensorPin);
}

void proceedSensorData()
{
  measuredMoistureTemp = map(measuredMoisture, minimumMoisture, maximumMoisture, moistureSetterBarButtonLimits.Y1, moistureSetterBarButtonLimits.Y2);

  if (measuredMoistureTemp < moistureSetterBarButtonLimits.Y1)
  {
    measuredMoistureTemp = moistureSetterBarButtonLimits.Y1;
  }
  else if (measuredMoistureTemp > moistureSetterBarButtonLimits.Y2)
  {
    measuredMoistureTemp = moistureSetterBarButtonLimits.Y2;
  }

  measuredMoisturePercent = map(measuredMoistureTemp, moistureSetterBarButtonLimits.Y2, moistureSetterBarButtonLimits.Y1, 0, 100);
}

bool isSoilTooDry()
{
  if (expectedMoisturePercent <= measuredMoisturePercent)
  {
    return false;
  }
  else
  {
    return true;
  }
}

void setup()
{
  Serial.begin(9600);

  Timer1.initialize(1000000);
  Timer1.attachInterrupt(timer_isr);

  pinMode(pump1.actorPin, OUTPUT);
  myGLCD.InitLCD();
  myGLCD.clrScr();

  myTouch.InitTouch();
  myTouch.setPrecision(PREC_HI);

  initializeHomeScreen(); // Draws the Home Screen
}

void loop()
{
  if (pump1.isInAutoMode)
  {
    if (isSoilTooDry())
    {
      startPump(pump1);
    }
    else
    {
      stopPump(pump1);
    }
  }

  if (readSensorWithNextLoopCycle)
  {
    readSensor();
    proceedSensorData();
    readSensorWithNextLoopCycle = false;
    refreshMeasuredMoistureDisplayValueOnNextLoopCycle = true;
    refreshMoistureBarOnNextLoopCycle = true;
  }

  // Home Screen
  if (currentPage == "HOME")
  {
    if (!homeScreenIsInitialized)
    {
      initializeHomeScreen();
      //resetHomeScreenBools();
    }

    handleHomeScreenInput();

    //refreshHomeScreen();
  }
  else if (currentPage == "LOVE")
  {
    if (!loveScreenIsInitialized)
    {
      initializeLoveScreen();
      //resetLoveScreenBools();
    }
    
    handleLoveScreenInput();

    //refreshLoveScreen();
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

    refreshUnderConstructionScreen();
  }
}
