enum EScreenState {
  HOME,
  POT1,
  POT2,
  POT3
};

class flowerpotMetadata
{
  public:
  uint8_t potID;
  uint8_t valvePin;
  uint8_t sensorPin;
  uint8_t priority;

  bool potIsActive;
  bool soilNeedsWater;
  bool valveIsOpen;

  uint16_t measuredMoistureRaw;
  uint16_t expectedMoistureRaw;
  uint16_t expectedMoisture;
  uint16_t expectedMoisturePercent;
  uint16_t measuredMoisture;
  uint16_t measuredMoisturePercent;

  flowerpotMetadata(uint8_t ID, uint8_t valvePin, uint8_t sensorPin)
  {
    potID = ID;
    valvePin = valvePin;
    sensorPin = sensorPin;

    priority = 0;
    potIsActive = false;
    soilNeedsWater = false;
    valveIsOpen = false;

    measuredMoistureRaw = 0;
    expectedMoistureRaw = 0;
    expectedMoisture = 0;
    expectedMoisturePercent = 0;
    measuredMoisture = 0;
    measuredMoisturePercent = 0;
  };

  void getAndProceedSensorData()
  {
    measuredMoisture = analogRead(sensorPin);

    measuredMoistureRaw = map(measuredMoisture, config::minimumMoisture, config::maximumMoisture, limits::moistureSetterBarButtonLimits.Y1, limits::moistureSetterBarButtonLimits.Y2);

    if (measuredMoistureRaw < limits::moistureSetterBarButtonLimits.Y1)
    {
      measuredMoistureRaw = limits::moistureSetterBarButtonLimits.Y1;
    }
    else if (measuredMoistureRaw > limits::moistureSetterBarButtonLimits.Y2)
    {
      measuredMoistureRaw = limits::moistureSetterBarButtonLimits.Y2;
    }

    measuredMoisturePercent = map(measuredMoistureRaw, limits::moistureSetterBarButtonLimits.Y2, limits::moistureSetterBarButtonLimits.Y1, 0, 100);
  }

  bool isSoilTooDry()
  {
    if (expectedMoisturePercent < measuredMoisturePercent)
    {
      return false;
    }
    else
    {
      return true;
    }
  }

  void checkSoilState(uint8_t& potsMoistureState)
  {
    if (potIsActive && isSoilTooDry())
    {
      soilNeedsWater = true;
      bitSet(potsMoistureState, potID);
    }
    else
    {
      soilNeedsWater = false;
      bitClear(potsMoistureState, potID);
    }
  }

  void openValve()
  {
    digitalWrite(valvePin, HIGH);

    valveIsOpen = true;
  }

  void closeValve()
  {
    digitalWrite(valvePin, LOW);

    valveIsOpen = false;
  }
  
  void setExpectedMoisture(long x, long y)
  {
    
      if (y <= limits::moistureSetterBarButtonLimits.Y1)
      {
        expectedMoistureRaw = limits::moistureSetterBarButtonLimits.Y1;
      }
      else if (y >= limits::moistureSetterBarButtonLimits.Y2)
      {
        expectedMoistureRaw = limits::moistureSetterBarButtonLimits.Y2;
      }
      else
      {
        expectedMoistureRaw = y;
      }

      // calculate calibrated moisture values
      expectedMoisture = map(expectedMoistureRaw, limits::moistureSetterBarButtonLimits.Y2, limits::moistureSetterBarButtonLimits.Y1, config::minimumMoisture, config::maximumMoisture);
      expectedMoisturePercent = map(expectedMoistureRaw, limits::moistureSetterBarButtonLimits.Y2, limits::moistureSetterBarButtonLimits.Y1, 0, 100);

  }
};