enum EScreenState {
  HOME,
  POT1,
  POT2,
  POT3
};

class flowerpotMetadata
{
  public:
  const uint8_t valvePin;
  const uint8_t sensorPin;
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
  
};