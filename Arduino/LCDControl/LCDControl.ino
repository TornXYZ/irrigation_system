#include <UTFT.h>
#include <URTouch.h>
#include <TimerOne.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "ButtonUtils.h"
#include "PumpControlUtils.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 280
#define ONE_WIRE_BUS A7
#define PUMP_PIN 8

// Initialize display
UTFT myGLCD(ILI9341_16, 38, 39, 40, 41);

// Initialize touchscreen
URTouch myTouch(6, 5, 4, 3, 2);

// Setup a OneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass OneWire reference to Dallas Temperature
DallasTemperature temperatureSensors(&oneWire);

// Initialize pots
flowerpotMetadata pot1{11, A0, 0, true, false, false, 0, 0, 0, 0, 0, 0};
flowerpotMetadata pot2{12, A1, 0, true, false, false, 0, 0, 0, 0, 0, 0};
flowerpotMetadata pot3{13, A2, 0, false, false, false, 0, 0, 0, 0, 0, 0};

const uint8_t numberOfPots = 3;
flowerpotMetadata allPots[numberOfPots] = {pot1, pot2, pot3};
//flowerpotMetadata *allPots[numberOfPots] = {&pot1, &pot2, &pot3};

int8_t currentPotToWater = -1;

uint8_t potsMoistureState = 0; //should be a bitset, which describes which pot needs watering. LSB equals pot 1.

// Defining variables
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
long x, y;

EScreenState screenState = HOME;

uint16_t temperatureMeasureIntervallInSec = 10;
volatile uint16_t temperatureMeasureCounter = 0;

const uint16_t minimumMoisture = 200;
const uint16_t maximumMoisture = 700;

// uint16_t measuredMoistureRaw = 100;

bool pumpIsOn = false;

bool readSensorsWithNextLoopCycle = true;
bool refreshExpectedMoistureDisplayValueOnNextLoopCycle = true;
bool refreshMeasuredMoistureDisplayValueOnNextLoopCycle = true;
bool refreshPumpOnOffButtonOnNextLoopCycle = true;
bool refreshPumpAutoModeButtonOnNextLoopCycle = true;
bool refreshMoistureBarOnNextLoopCycle = true;
bool refreshTemperatureDisplayOnNextLoopCycle = true;
bool refreshThermometerDisplayOnNextLoopCycle = true;
bool refreshActivatePotButtonOnNextLoopCycle = true;

bool homeScreenIsInitialized = false;
bool potScreenIsInitialized = false;
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
  myGLCD.fillRoundRect(pot1ButtonLimits.X1, pot1ButtonLimits.Y1, pot1ButtonLimits.X2, pot1ButtonLimits.Y2); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255);                                                                               // Sets color to white
  myGLCD.drawRoundRect(pot1ButtonLimits.X1, pot1ButtonLimits.Y1, pot1ButtonLimits.X2, pot1ButtonLimits.Y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setFont(BigFont);                                                                                      // Sets the font to big
  myGLCD.setBackColor(0, 255, 0);                                                                               // Sets the background color of the area where the text will be printed to green, same as the button
  myGLCD.print("SENS 1", pot1ButtonLimits.X1 + 10, pot1ButtonLimits.Y1 + 10);                                 // Prints the string

  // Button 2
  myGLCD.setColor(0, 255, 0);                                                                                   // Sets color to green
  myGLCD.fillRoundRect(pot2ButtonLimits.X1, pot2ButtonLimits.Y1, pot2ButtonLimits.X2, pot2ButtonLimits.Y2); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255);                                                                               // Sets color to white
  myGLCD.drawRoundRect(pot2ButtonLimits.X1, pot2ButtonLimits.Y1, pot2ButtonLimits.X2, pot2ButtonLimits.Y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setFont(BigFont);                                                                                      // Sets the font to big
  myGLCD.setBackColor(0, 255, 0);                                                                               // Sets the background color of the area where the text will be printed to green, same as the button
  myGLCD.print("SENS 2", pot2ButtonLimits.X1 + 10, pot2ButtonLimits.Y1 + 10);                                 // Prints the string

  // Button 3
  myGLCD.setColor(0, 255, 0);                                                                                   // Sets color to green
  myGLCD.fillRoundRect(pot3ButtonLimits.X1, pot3ButtonLimits.Y1, pot3ButtonLimits.X2, pot3ButtonLimits.Y2); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255);                                                                               // Sets color to white
  myGLCD.drawRoundRect(pot3ButtonLimits.X1, pot3ButtonLimits.Y1, pot3ButtonLimits.X2, pot3ButtonLimits.Y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setFont(BigFont);                                                                                      // Sets the font to big
  myGLCD.setBackColor(0, 255, 0);                                                                               // Sets the background color of the area where the text will be printed to green, same as the button
  myGLCD.print("SENS 3", pot3ButtonLimits.X1 + 10, pot3ButtonLimits.Y1 + 10);                                 // Prints the string

  // temperature Display
  myGLCD.setColor(0, 255, 0);                                                                                                               // Sets color to red
  myGLCD.fillRoundRect(temperatureDisplayLimits.X1, temperatureDisplayLimits.Y1, temperatureDisplayLimits.X2, temperatureDisplayLimits.Y2); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255);                                                                                                           // Sets color to white
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
  myGLCD.setBackColor(0, 0, 0); // Sets the background color of the area where the text will be printed to green, same as the button

  for (uint8_t i = thermometerOuterLimits.Y2; i >= thermometerOuterLimits.Y1; i -= (thermometerOuterLimits.Y2 - thermometerOuterLimits.Y1) / 4)
  {
    myGLCD.printNumI(tempScale, thermometerOuterLimits.X1 - 18, i - 10);
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

    // PRESS BUTTON "POT1"
    if (pointIsInsideButtonLimits(x, y, pot1ButtonLimits))
    {
      drawFrame(pot1ButtonLimits);
      screenState = POT1;
      myGLCD.clrScr();
    }
    // PRESS BUTTON "POT2"
    else if (pointIsInsideButtonLimits(x, y, pot2ButtonLimits))
    {
      drawFrame(pot2ButtonLimits);
      screenState = POT2;
      myGLCD.clrScr();
    }
    // PRESS BUTTON "POT3"
    else if (pointIsInsideButtonLimits(x, y, pot3ButtonLimits))
    {
      drawFrame(pot3ButtonLimits);
      screenState = POT3;
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
  if (temperatureSensors.getTempCByIndex(0) < 0 || temperatureSensors.getTempCByIndex(0) > 40)
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

  for (uint8_t i = thermometerOuterLimits.Y2 - 7; i >= thermometerOuterLimits.Y1 - 7; i -= (thermometerOuterLimits.Y2 - thermometerOuterLimits.Y1) / 9)
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
  underConstructionScreenIsInitialized = true;
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
      screenState = HOME;
      myGLCD.clrScr(); // Clears the screen
    }
    underConstructionScreenIsInitialized = false;
  }
}

// POT CONTROL SCREEN
void initializePotScreen()
{
  drawHeadline("POT CONTROL");

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

  potScreenIsInitialized = true;
}

void resetPotScreenBools()
{
  refreshExpectedMoistureDisplayValueOnNextLoopCycle = true;
  refreshMeasuredMoistureDisplayValueOnNextLoopCycle = true;
  refreshPumpOnOffButtonOnNextLoopCycle = true;
  refreshPumpAutoModeButtonOnNextLoopCycle = true;
  refreshMoistureBarOnNextLoopCycle = true;
}

void handlePotScreenInput(flowerpotMetadata &pot)
{
  if (myTouch.dataAvailable())
  {
    myTouch.read();
    x = myTouch.getX();
    y = myTouch.getY() * (-1) + 240;

    // SET EXPECTED MOISTURE
    if (pointIsInsideButtonLimits(x, y, moistureSetterBarButtonLimits))
    {
      if (y <= moistureSetterBarButtonLimits.Y1)
      {
        pot.expectedMoistureRaw = moistureSetterBarButtonLimits.Y1;
      }
      else if (y >= moistureSetterBarButtonLimits.Y2)
      {
        pot.expectedMoistureRaw = moistureSetterBarButtonLimits.Y2;
      }
      else
      {
        pot.expectedMoistureRaw = y;
      }

      // calculate calibrated moisture values
      pot.expectedMoisture = map(pot.expectedMoistureRaw, moistureSetterBarButtonLimits.Y2, moistureSetterBarButtonLimits.Y1, minimumMoisture, maximumMoisture);
      pot.expectedMoisturePercent = map(pot.expectedMoistureRaw, moistureSetterBarButtonLimits.Y2, moistureSetterBarButtonLimits.Y1, 0, 100);

      refreshExpectedMoistureDisplayValueOnNextLoopCycle = true;
      refreshMoistureBarOnNextLoopCycle = true;
    }

    // PRESS RETURN
    if (pointIsInsideButtonLimits(x, y, returnButtonLimits))
    {
      drawFrame(20, 60, 80, 100); // Custom Function -Highlighs the buttons when it's pressed
      screenState = HOME;
      myGLCD.clrScr(); // Clears the screen
      potScreenIsInitialized = false;
    }

    if (pointIsInsideButtonLimits(x, y, activatePotButtonLimits))
    {
      drawFrame(activatePotButtonLimits.X1, activatePotButtonLimits.Y1, activatePotButtonLimits.X2, activatePotButtonLimits.Y2);
      
      if (pot.potIsActive == false)
      {
        pot.potIsActive = true;
      }
      else
      {
        pot.potIsActive = false;
      }
      refreshActivatePotButtonOnNextLoopCycle = true;
    }
  }
}

void refreshExpectedMoistureValue(flowerpotMetadata pot)
{
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(20, 120, 120, 179);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont);
  myGLCD.print("exp:", 20, 120);
  myGLCD.printNumI(pot.expectedMoisture, 20, 140, 3);
  myGLCD.printNumI(pot.expectedMoisturePercent, 20, 160, 3);
  myGLCD.printChar('%', 70, 160);

  refreshExpectedMoistureDisplayValueOnNextLoopCycle = false;
}

void refreshMeasuredMoistureValue(flowerpotMetadata pot)
{
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(20, 180, 120, 239);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont);
  myGLCD.print("meas:", 20, 180);
  myGLCD.printNumI(pot.measuredMoisture, 20, 200, 3);
  myGLCD.printNumI(pot.measuredMoisturePercent, 20, 220, 3);
  myGLCD.printChar('%', 70, 220);

  refreshMeasuredMoistureDisplayValueOnNextLoopCycle = false;
}

void refreshMoistureBar(flowerpotMetadata pot)
{
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRect(moistureSetterBarButtonLimits.X1, pot.expectedMoistureRaw + 2, moistureSetterBarButtonLimits.X2, pot.expectedMoistureRaw - 2);
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(moistureSetterBarButtonLimits.X1, pot.measuredMoistureRaw + 2, moistureSetterBarButtonLimits.X2, pot.measuredMoistureRaw - 2);
  myGLCD.setColor(255, 255, 0);
  myGLCD.fillRect(moistureSetterBarButtonLimits.X1, moistureSetterBarButtonLimits.Y1 - 2, moistureSetterBarButtonLimits.X2, min(pot.expectedMoistureRaw, pot.measuredMoistureRaw) - 2);
  myGLCD.fillRect(moistureSetterBarButtonLimits.X1, max(pot.expectedMoistureRaw, pot.measuredMoistureRaw) + 2, moistureSetterBarButtonLimits.X2, moistureSetterBarButtonLimits.Y2 + 2);
  myGLCD.fillRect(moistureSetterBarButtonLimits.X1, min(pot.expectedMoistureRaw, pot.measuredMoistureRaw) + 2, moistureSetterBarButtonLimits.X2, max(pot.expectedMoistureRaw, pot.measuredMoistureRaw) - 2);

  refreshMoistureBarOnNextLoopCycle = false;
}

void refreshValveStateDisplay(flowerpotMetadata pot)
{
  if (pot.valveIsOpen)
  {
    myGLCD.setColor(0, 255, 0);
    myGLCD.fillRoundRect(valveOpenClosedDisplayLimits.X1, valveOpenClosedDisplayLimits.Y1, valveOpenClosedDisplayLimits.X2, valveOpenClosedDisplayLimits.Y2);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect(valveOpenClosedDisplayLimits.X1, valveOpenClosedDisplayLimits.Y1, valveOpenClosedDisplayLimits.X2, valveOpenClosedDisplayLimits.Y2);
  }
  else
  {
    myGLCD.setColor(255, 0, 0);
    myGLCD.fillRoundRect(valveOpenClosedDisplayLimits.X1, valveOpenClosedDisplayLimits.Y1, valveOpenClosedDisplayLimits.X2, valveOpenClosedDisplayLimits.Y2);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect(valveOpenClosedDisplayLimits.X1, valveOpenClosedDisplayLimits.Y1, valveOpenClosedDisplayLimits.X2, valveOpenClosedDisplayLimits.Y2);
  }

  myGLCD.printNumI(currentPotToWater, valveOpenClosedDisplayLimits.X1 + 3, valveOpenClosedDisplayLimits.Y1 + 3);
}

void refreshSoilStateDisplay(flowerpotMetadata pot)
{
  if (pot.soilNeedsWater)
  {
    myGLCD.setColor(0, 255, 0);
    myGLCD.fillRoundRect(soilTooDryDisplayimits.X1, soilTooDryDisplayimits.Y1, soilTooDryDisplayimits.X2, soilTooDryDisplayimits.Y2);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect(soilTooDryDisplayimits.X1, soilTooDryDisplayimits.Y1, soilTooDryDisplayimits.X2, soilTooDryDisplayimits.Y2);
  }
  else
  {
    myGLCD.setColor(255, 0, 0);
    myGLCD.fillRoundRect(soilTooDryDisplayimits.X1, soilTooDryDisplayimits.Y1, soilTooDryDisplayimits.X2, soilTooDryDisplayimits.Y2);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect(soilTooDryDisplayimits.X1, soilTooDryDisplayimits.Y1, soilTooDryDisplayimits.X2, soilTooDryDisplayimits.Y2);
  }
}

void refreshActivatePotButton(flowerpotMetadata pot)
{
  if (pot.potIsActive)
  {
    myGLCD.setColor(0, 255, 0);
    myGLCD.fillRoundRect(activatePotButtonLimits.X1, activatePotButtonLimits.Y1, activatePotButtonLimits.X2, activatePotButtonLimits.Y2);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect(activatePotButtonLimits.X1, activatePotButtonLimits.Y1, activatePotButtonLimits.X2, activatePotButtonLimits.Y2);
  }
  else
  {
    myGLCD.setColor(255, 0, 0);
    myGLCD.fillRoundRect(activatePotButtonLimits.X1, activatePotButtonLimits.Y1, activatePotButtonLimits.X2, activatePotButtonLimits.Y2);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect(activatePotButtonLimits.X1, activatePotButtonLimits.Y1, activatePotButtonLimits.X2, activatePotButtonLimits.Y2);
  }

  refreshActivatePotButtonOnNextLoopCycle = false;
}

void refreshPotScreen(flowerpotMetadata &pot)
{
  if (refreshExpectedMoistureDisplayValueOnNextLoopCycle)
  {
    refreshExpectedMoistureValue(pot);
  }

  if (refreshMeasuredMoistureDisplayValueOnNextLoopCycle)
  {
    refreshMeasuredMoistureValue(pot);
  }

  if (refreshMoistureBarOnNextLoopCycle)
  {
    refreshMoistureBar(pot);
    refreshValveStateDisplay(pot);
    refreshSoilStateDisplay(pot);
  }

  if (refreshActivatePotButtonOnNextLoopCycle)
  {
    refreshActivatePotButton(pot);
  }
}

void timer_isr()
{
  noInterrupts();
  readSensorsWithNextLoopCycle = true;
  temperatureMeasureCounter++;
  interrupts();
}

void readAllMoistureSensors()
{
  for (uint8_t i = 0; i < numberOfPots; i++)
  {
    if (allPots[i].potIsActive)
    {
      allPots[i].measuredMoisture = analogRead(allPots[i].sensorPin);
    }
  }
}

void proceedSensorData()
{
  for (uint8_t i = 0; i < numberOfPots; i++)
  {
    if (allPots[i].potIsActive)
    {
      allPots[i].measuredMoistureRaw = map(allPots[i].measuredMoisture, minimumMoisture, maximumMoisture, moistureSetterBarButtonLimits.Y1, moistureSetterBarButtonLimits.Y2);

      if (allPots[i].measuredMoistureRaw < moistureSetterBarButtonLimits.Y1)
      {
        allPots[i].measuredMoistureRaw = moistureSetterBarButtonLimits.Y1;
      }
      else if (allPots[i].measuredMoistureRaw > moistureSetterBarButtonLimits.Y2)
      {
        allPots[i].measuredMoistureRaw = moistureSetterBarButtonLimits.Y2;
      }

      allPots[i].measuredMoisturePercent = map(allPots[i].measuredMoistureRaw, moistureSetterBarButtonLimits.Y2, moistureSetterBarButtonLimits.Y1, 0, 100);
    }
  }
}

void checkSoilStates()
{
  for (uint8_t i = 0; i < numberOfPots; i++)
  {
    if (allPots[i].potIsActive && allPots[i].isSoilTooDry())
    {
      allPots[i].soilNeedsWater = true;
      bitSet(potsMoistureState, i);
    }
    else
    {
      allPots[i].soilNeedsWater = false;
      bitClear(potsMoistureState, i);
    }
  }
}

uint8_t getPotWithHighestPriority()
{
  uint8_t currentHighestPriority = 0;
  int8_t potID = -1;

  for (uint8_t i = 0; i < numberOfPots; i++)
  {
    if (allPots[i].potIsActive && allPots[i].soilNeedsWater)
    {
      if (allPots[i].priority > currentHighestPriority)
      {
        currentHighestPriority = allPots[i].priority;
        potID = i;
      }
    }
  }

  return potID;
}

void updatePriorities()
{
  for (uint8_t i = 0; i < numberOfPots; i++)
  {
    if (allPots[i].potIsActive)
    {
      if (allPots[i].soilNeedsWater)
      {
        allPots[i].priority++;
      }
      else
      {
        allPots[i].priority = 0;
      }
    }
  }
}

void choosePotToWater()
{
  currentPotToWater = getPotWithHighestPriority();
  Serial.println("Pot chosen:" + (String)currentPotToWater);
}

void manageValves()
{
  for (uint8_t i = 0; i < numberOfPots; i++)
  {
    if (allPots[i].potIsActive)
    {
      if (i == currentPotToWater && allPots[i].valveIsOpen == false)
      {
        allPots[i].openValve();
        Serial.println("Valve opened:" + (String)i);
      }
      else if (i != currentPotToWater && allPots[i].valveIsOpen == true)
      {
        allPots[i].closeValve();
        Serial.println("Valve closed." + (String)i);
      }
    }
  }
}

void startPump()
{
  digitalWrite(PUMP_PIN, HIGH);

  pumpIsOn = true;
}

void stopPump()
{
  digitalWrite(PUMP_PIN, LOW);

  pumpIsOn = false;
}

void managePump()
{
  if (potsMoistureState > 0 && pumpIsOn == false)
  {
    startPump();
    Serial.println("pump started, pumpState = " + (String)pumpIsOn);
  }
  else if (potsMoistureState == 0 && pumpIsOn == true)
  {
    stopPump();
    Serial.println("pump stopped, pumpState = " + (String)pumpIsOn);
  }
  Serial.println("potsMoistureState = " + (String)potsMoistureState);
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
  interrupts();

  myGLCD.InitLCD();
  myGLCD.clrScr();

  myTouch.InitTouch();
  myTouch.setPrecision(PREC_HI);

  initializeHomeScreen(); // Draws the Home Screen
}

void handleHomeScreenIO()
{
  if (!homeScreenIsInitialized)
  {
    initializeHomeScreen();
  }

  handleHomeScreenInput();

  refreshHomeScreen();
}

void handlePotScreenIO(flowerpotMetadata &pot)
{
  if (!potScreenIsInitialized)
  {
    initializePotScreen();
    resetPotScreenBools();
  }

  handlePotScreenInput(pot);

  refreshPotScreen(pot);
}

void loop()
{
  if (readSensorsWithNextLoopCycle)
  {
    readAllMoistureSensors();
    proceedSensorData();
    readSensorsWithNextLoopCycle = false;
    refreshMeasuredMoistureDisplayValueOnNextLoopCycle = true;
    refreshMoistureBarOnNextLoopCycle = true;

    checkSoilStates();

    updatePriorities();

    choosePotToWater();

    manageValves();

    managePump();
  }

  if (temperatureMeasureCounter >= temperatureMeasureIntervallInSec)
  {
    requestTemperature(); // causes constant call of timer_isr. Reason is unknown...
  }

  switch (screenState)
  {
  case HOME:
    handleHomeScreenIO();
    break;

  case POT1:
    handlePotScreenIO(allPots[0]);
    break;

  case POT2:
    handlePotScreenIO(allPots[1]);
    break;

  case POT3:
    handlePotScreenIO(allPots[2]);
    break;

  default:
    break;
  }
}
