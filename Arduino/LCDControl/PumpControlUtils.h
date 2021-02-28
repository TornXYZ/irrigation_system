struct pumpMetadata
{
  uint8_t pumpNumber;
  uint8_t actorPin;
  uint8_t sensorPin;
  bool isOn;
  bool isInAutoMode;
};

void startPump(pumpMetadata& pump)
{
  pump.isOn = true;
  digitalWrite(pump.actorPin, HIGH);
}

void stopPump(pumpMetadata& pump)
{
  pump.isOn = false;
  digitalWrite(pump.actorPin, LOW);
}

void startAutoMode(pumpMetadata& pump)
{
  pump.isInAutoMode = true;
}

void stopAutoMode(pumpMetadata& pump)
{
  pump.isInAutoMode = false;
}

void togglePumpState(pumpMetadata& pump, bool autoModeRemainsOn)
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

void toggleAutoModeState(pumpMetadata& pump, bool pumpRemainsOn)
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