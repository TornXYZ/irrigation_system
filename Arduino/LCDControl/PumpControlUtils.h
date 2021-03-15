class flowerpotMetadata
{
  public:
  uint8_t pumpNumber;
  uint8_t actorPin;
  uint8_t sensorPin;
  bool isOn;
  bool isInAutoMode;
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
};

void startPump(flowerpotMetadata& pump)
{
  pump.isOn = true;
  digitalWrite(pump.actorPin, HIGH);
}

void stopPump(flowerpotMetadata& pump)
{
  pump.isOn = false;
  digitalWrite(pump.actorPin, LOW);
}

void startAutoMode(flowerpotMetadata& pump)
{
  pump.isInAutoMode = true;
}

void stopAutoMode(flowerpotMetadata& pump)
{
  pump.isInAutoMode = false;
}

void togglePumpState(flowerpotMetadata& pump, bool autoModeRemainsOn)
{
  if (pump.isOn)
  {
    stopPump(pump);
  }
  else
  {
    startPump(pump);
  }

  if (!autoModeRemainsOn && pump.isInAutoMode)
  {
    stopAutoMode(pump);
  }
}

void toggleAutoModeState(flowerpotMetadata& pump, bool pumpRemainsOn)
{
  if (pump.isInAutoMode)
  {
    stopAutoMode(pump);
  }
  else
  {
    startAutoMode(pump);
  }

  if (!pumpRemainsOn && pump.isOn)
  {
    stopPump(pump);
  }
}