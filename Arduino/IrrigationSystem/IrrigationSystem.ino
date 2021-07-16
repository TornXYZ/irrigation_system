#include <UTFT.h>
#include <URTouch.h>
#include <TimerOne.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "config.h"
#include "ButtonUtils.h"
#include "PotUtils.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 280
#define ONE_WIRE_BUS A0
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
flowerpotMetadata pot1(0, 9, A1);
flowerpotMetadata pot2(1, 10, A2);
flowerpotMetadata pot3(2, 11, A3);

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

volatile uint16_t temperatureMeasureCounter = 0;

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

void drawFrame(limits::rectXY rect)
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
  myGLCD.fillRoundRect(limits::returnButtonLimits.X1, limits::returnButtonLimits.Y1, limits::returnButtonLimits.X2, limits::returnButtonLimits.Y2); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255);                                                                                   // Sets color to white
  myGLCD.drawRoundRect(limits::returnButtonLimits.X1, limits::returnButtonLimits.Y1, limits::returnButtonLimits.X2, limits::returnButtonLimits.Y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setFont(BigFont);                                                                                          // Sets the font to big
  myGLCD.setBackColor(0, 255, 0);                                                                                   // Sets the background color of the area where the text will be printed to green, same as the button
  myGLCD.print("ret", 25, 70);                                                                                      // Prints the string
}

// HOME SCREEN
void initializeHomeScreen()
{
  myGLCD.clrScr();

  drawHeadline("");

  // Button 1
  myGLCD.setColor(0, 255, 0);                                                                                   // Sets color to green
  myGLCD.fillRoundRect(limits::pot1ButtonLimits.X1, limits::pot1ButtonLimits.Y1, limits::pot1ButtonLimits.X2, limits::pot1ButtonLimits.Y2); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255);                                                                               // Sets color to white
  myGLCD.drawRoundRect(limits::pot1ButtonLimits.X1, limits::pot1ButtonLimits.Y1, limits::pot1ButtonLimits.X2, limits::pot1ButtonLimits.Y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setFont(BigFont);                                                                                      // Sets the font to big
  myGLCD.setBackColor(0, 255, 0);                                                                               // Sets the background color of the area where the text will be printed to green, same as the button
  myGLCD.print("SENS 1", limits::pot1ButtonLimits.X1 + 10, limits::pot1ButtonLimits.Y1 + 10);                                 // Prints the string

  // Button 2
  myGLCD.setColor(0, 255, 0);                                                                                   // Sets color to green
  myGLCD.fillRoundRect(limits::pot2ButtonLimits.X1, limits::pot2ButtonLimits.Y1, limits::pot2ButtonLimits.X2, limits::pot2ButtonLimits.Y2); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255);                                                                               // Sets color to white
  myGLCD.drawRoundRect(limits::pot2ButtonLimits.X1, limits::pot2ButtonLimits.Y1, limits::pot2ButtonLimits.X2, limits::pot2ButtonLimits.Y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setFont(BigFont);                                                                                      // Sets the font to big
  myGLCD.setBackColor(0, 255, 0);                                                                               // Sets the background color of the area where the text will be printed to green, same as the button
  myGLCD.print("SENS 2", limits::pot2ButtonLimits.X1 + 10, limits::pot2ButtonLimits.Y1 + 10);                                 // Prints the string

  // Button 3
  myGLCD.setColor(0, 255, 0);                                                                                   // Sets color to green
  myGLCD.fillRoundRect(limits::pot3ButtonLimits.X1, limits::pot3ButtonLimits.Y1, limits::pot3ButtonLimits.X2, limits::pot3ButtonLimits.Y2); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255);                                                                               // Sets color to white
  myGLCD.drawRoundRect(limits::pot3ButtonLimits.X1, limits::pot3ButtonLimits.Y1, limits::pot3ButtonLimits.X2, limits::pot3ButtonLimits.Y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setFont(BigFont);                                                                                      // Sets the font to big
  myGLCD.setBackColor(0, 255, 0);                                                                               // Sets the background color of the area where the text will be printed to green, same as the button
  myGLCD.print("SENS 3", limits::pot3ButtonLimits.X1 + 10, limits::pot3ButtonLimits.Y1 + 10);                                 // Prints the string

  // temperature Display
  myGLCD.setColor(0, 255, 0);                                                                                                               // Sets color to red
  myGLCD.fillRoundRect(limits::temperatureDisplayLimits.X1, limits::temperatureDisplayLimits.Y1, limits::temperatureDisplayLimits.X2, limits::temperatureDisplayLimits.Y2); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255);                                                                                                           // Sets color to white
  myGLCD.drawRoundRect(limits::temperatureDisplayLimits.X1, limits::temperatureDisplayLimits.Y1, limits::temperatureDisplayLimits.X2, limits::temperatureDisplayLimits.Y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  refreshTemperatureDisplay();

  // thermometer
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRect(limits::thermometerOuterLimits.X1, limits::thermometerOuterLimits.Y1, limits::thermometerOuterLimits.X2, limits::thermometerOuterLimits.Y2);
  myGLCD.fillCircle((limits::thermometerOuterLimits.X1 + limits::thermometerOuterLimits.X2) / 2, limits::thermometerOuterLimits.Y1, (limits::thermometerOuterLimits.X2 - limits::thermometerOuterLimits.X1) / 2);
  myGLCD.fillCircle((limits::thermometerOuterLimits.X1 + limits::thermometerOuterLimits.X2) / 2, limits::thermometerOuterLimits.Y2 + (limits::thermometerInnerLimits.X2 - limits::thermometerInnerLimits.X1), limits::thermometerOuterLimits.X2 - limits::thermometerOuterLimits.X1);
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillCircle((limits::thermometerOuterLimits.X1 + limits::thermometerOuterLimits.X2) / 2, limits::thermometerOuterLimits.Y2 + (limits::thermometerInnerLimits.X2 - limits::thermometerInnerLimits.X1), limits::thermometerOuterLimits.X2 - limits::thermometerOuterLimits.X1 - 4);
  myGLCD.setFont(SmallFont);

  uint8_t tempScale = 0;
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0); // Sets the background color of the area where the text will be printed to green, same as the button

  for (uint8_t i = limits::thermometerOuterLimits.Y2; i >= limits::thermometerOuterLimits.Y1; i -= (limits::thermometerOuterLimits.Y2 - limits::thermometerOuterLimits.Y1) / 4)
  {
    myGLCD.printNumI(tempScale, limits::thermometerOuterLimits.X1 - 18, i - 10);
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
    if (pointIsInsideButtonLimits(x, y, limits::pot1ButtonLimits))
    {
      drawFrame(limits::pot1ButtonLimits);
      screenState = POT1;
      myGLCD.clrScr();
    }
    // PRESS BUTTON "POT2"
    else if (pointIsInsideButtonLimits(x, y, limits::pot2ButtonLimits))
    {
      drawFrame(limits::pot2ButtonLimits);
      screenState = POT2;
      myGLCD.clrScr();
    }
    // PRESS BUTTON "POT3"
    else if (pointIsInsideButtonLimits(x, y, limits::pot3ButtonLimits))
    {
      drawFrame(limits::pot3ButtonLimits);
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
  float temperature = temperatureSensors.getTempCByIndex(0);

  if (temperature == -127)
  {
    myGLCD.print("ERR", limits::temperatureDisplayLimits.X1 + 10, limits::temperatureDisplayLimits.Y1 + 10);
  }
  else
  {
  myGLCD.printNumF(temperatureSensors.getTempCByIndex(0), 1, limits::temperatureDisplayLimits.X1 + 10, limits::temperatureDisplayLimits.Y1 + 10);
  //myGLCD.print("°C", temperatureDisplayLimits.X1 + 50, temperatureDisplayLimits.Y1 + 10);
  }
  
  refreshTemperatureDisplayOnNextLoopCycle = false;
}

void refreshThermometerDisplay()
{
  if (temperatureSensors.getTempCByIndex(0) < 0 || temperatureSensors.getTempCByIndex(0) > 40)
  {
    myGLCD.setColor(0, 0, 255);
    myGLCD.fillRect(limits::thermometerInnerLimits.X1, limits::thermometerInnerLimits.Y1, limits::thermometerInnerLimits.X2, limits::thermometerInnerLimits.Y2);
  }
  else
  {
    uint16_t temperatureTemp = map(temperatureSensors.getTempCByIndex(0), 0, 40, limits::thermometerOuterLimits.Y2, limits::thermometerOuterLimits.Y1);

    myGLCD.setColor(255, 255, 255);
    myGLCD.fillRect(limits::thermometerInnerLimits.X1, limits::thermometerInnerLimits.Y1, limits::thermometerInnerLimits.X2, temperatureTemp);

    myGLCD.setColor(255, 0, 0);
    myGLCD.fillRect(limits::thermometerInnerLimits.X1, temperatureTemp, limits::thermometerInnerLimits.X2, limits::thermometerInnerLimits.Y2);
  }

  for (uint8_t i = limits::thermometerOuterLimits.Y2 - 7; i >= limits::thermometerOuterLimits.Y1 - 7; i -= (limits::thermometerOuterLimits.Y2 - limits::thermometerOuterLimits.Y1) / 9)
  {
    myGLCD.setColor(255, 255, 255);
    myGLCD.fillRect(limits::thermometerOuterLimits.X1, i - 1, limits::thermometerOuterLimits.X1 + 7, i + 1);
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

// POT CONTROL SCREEN
void initializePotScreen()
{
  myGLCD.clrScr();

  drawHeadline("POT CONTROL");

  myGLCD.setColor(255, 255, 0);
  myGLCD.fillRect(limits::moistureSetterBarButtonLimits.X1, limits::moistureSetterBarButtonLimits.Y1, limits::moistureSetterBarButtonLimits.X2, limits::moistureSetterBarButtonLimits.Y2);

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
  refreshActivatePotButtonOnNextLoopCycle = true;
}

void handlePotScreenInput(flowerpotMetadata &pot)
{
  if (myTouch.dataAvailable())
  {
    myTouch.read();
    x = myTouch.getX();
    y = myTouch.getY() * (-1) + 240;

    // SET EXPECTED MOISTURE
    if (pointIsInsideButtonLimits(x, y, limits::moistureSetterBarButtonLimits))
    {
      pot.setExpectedMoisture(x, y);
      
      refreshExpectedMoistureDisplayValueOnNextLoopCycle = true;
      refreshMoistureBarOnNextLoopCycle = true;
    }

    // PRESS RETURN
    if (pointIsInsideButtonLimits(x, y, limits::returnButtonLimits))
    {
      drawFrame(20, 60, 80, 100); // Custom Function -Highlighs the buttons when it's pressed
      screenState = HOME;
      myGLCD.clrScr(); // Clears the screen
      potScreenIsInitialized = false;
    }

    if (pointIsInsideButtonLimits(x, y, limits::activatePotButtonLimits))
    {
      drawFrame(limits::activatePotButtonLimits.X1, limits::activatePotButtonLimits.Y1, limits::activatePotButtonLimits.X2, limits::activatePotButtonLimits.Y2);
      
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
  myGLCD.fillRect(limits::moistureSetterBarButtonLimits.X1, pot.expectedMoistureRaw + 2, limits::moistureSetterBarButtonLimits.X2, pot.expectedMoistureRaw - 2);
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(limits::moistureSetterBarButtonLimits.X1, pot.measuredMoistureRaw + 2, limits::moistureSetterBarButtonLimits.X2, pot.measuredMoistureRaw - 2);
  myGLCD.setColor(255, 255, 0);
  myGLCD.fillRect(limits::moistureSetterBarButtonLimits.X1, limits::moistureSetterBarButtonLimits.Y1 - 2, limits::moistureSetterBarButtonLimits.X2, min(pot.expectedMoistureRaw, pot.measuredMoistureRaw) - 2);
  myGLCD.fillRect(limits::moistureSetterBarButtonLimits.X1, max(pot.expectedMoistureRaw, pot.measuredMoistureRaw) + 2, limits::moistureSetterBarButtonLimits.X2, limits::moistureSetterBarButtonLimits.Y2 + 2);
  myGLCD.fillRect(limits::moistureSetterBarButtonLimits.X1, min(pot.expectedMoistureRaw, pot.measuredMoistureRaw) + 2, limits::moistureSetterBarButtonLimits.X2, max(pot.expectedMoistureRaw, pot.measuredMoistureRaw) - 2);

  refreshMoistureBarOnNextLoopCycle = false;
}

void refreshValveStateDisplay(flowerpotMetadata pot)
{
  if (pot.valveIsOpen)
  {
    myGLCD.setColor(0, 255, 0);
    myGLCD.fillRoundRect(limits::valveOpenClosedDisplayLimits.X1, limits::valveOpenClosedDisplayLimits.Y1, limits::valveOpenClosedDisplayLimits.X2, limits::valveOpenClosedDisplayLimits.Y2);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect(limits::valveOpenClosedDisplayLimits.X1, limits::valveOpenClosedDisplayLimits.Y1, limits::valveOpenClosedDisplayLimits.X2, limits::valveOpenClosedDisplayLimits.Y2);
  }
  else
  {
    myGLCD.setColor(255, 0, 0);
    myGLCD.fillRoundRect(limits::valveOpenClosedDisplayLimits.X1, limits::valveOpenClosedDisplayLimits.Y1, limits::valveOpenClosedDisplayLimits.X2, limits::valveOpenClosedDisplayLimits.Y2);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect(limits::valveOpenClosedDisplayLimits.X1, limits::valveOpenClosedDisplayLimits.Y1, limits::valveOpenClosedDisplayLimits.X2, limits::valveOpenClosedDisplayLimits.Y2);
  }

  myGLCD.printNumI(currentPotToWater, limits::valveOpenClosedDisplayLimits.X1 + 3, limits::valveOpenClosedDisplayLimits.Y1 + 3);
}

void refreshSoilStateDisplay(flowerpotMetadata pot)
{
  if (pot.soilNeedsWater)
  {
    myGLCD.setColor(0, 255, 0);
    myGLCD.fillRoundRect(limits::soilTooDryDisplayimits.X1, limits::soilTooDryDisplayimits.Y1, limits::soilTooDryDisplayimits.X2, limits::soilTooDryDisplayimits.Y2);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect(limits::soilTooDryDisplayimits.X1, limits::soilTooDryDisplayimits.Y1, limits::soilTooDryDisplayimits.X2, limits::soilTooDryDisplayimits.Y2);
  }
  else
  {
    myGLCD.setColor(255, 0, 0);
    myGLCD.fillRoundRect(limits::soilTooDryDisplayimits.X1, limits::soilTooDryDisplayimits.Y1, limits::soilTooDryDisplayimits.X2, limits::soilTooDryDisplayimits.Y2);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect(limits::soilTooDryDisplayimits.X1, limits::soilTooDryDisplayimits.Y1, limits::soilTooDryDisplayimits.X2, limits::soilTooDryDisplayimits.Y2);
  }
}

void refreshActivatePotButton(flowerpotMetadata pot)
{
  if (pot.potIsActive)
  {
    myGLCD.setColor(0, 255, 0);
    myGLCD.fillRoundRect(limits::activatePotButtonLimits.X1, limits::activatePotButtonLimits.Y1, limits::activatePotButtonLimits.X2, limits::activatePotButtonLimits.Y2);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect(limits::activatePotButtonLimits.X1, limits::activatePotButtonLimits.Y1, limits::activatePotButtonLimits.X2, limits::activatePotButtonLimits.Y2);
  }
  else
  {
    myGLCD.setColor(255, 0, 0);
    myGLCD.fillRoundRect(limits::activatePotButtonLimits.X1, limits::activatePotButtonLimits.Y1, limits::activatePotButtonLimits.X2, limits::activatePotButtonLimits.Y2);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect(limits::activatePotButtonLimits.X1, limits::activatePotButtonLimits.Y1, limits::activatePotButtonLimits.X2, limits::activatePotButtonLimits.Y2);
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
    allPots[i].getAndProceedSensorData();
  }
}

void checkSoilStates()
{
  for (uint8_t i = 0; i < numberOfPots; i++)
  {
    allPots[i].checkSoilState(potsMoistureState);
  }
}

int8_t getPotWithHighestPriority()
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

    readSensorsWithNextLoopCycle = false;
    refreshMeasuredMoistureDisplayValueOnNextLoopCycle = true;
    refreshMoistureBarOnNextLoopCycle = true;

    checkSoilStates();

    updatePriorities();

    choosePotToWater();

    manageValves();

    managePump();
  }

  if (temperatureMeasureCounter >= config::temperatureMeasureIntervallInSec)
  {
    // requestTemperature();
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
