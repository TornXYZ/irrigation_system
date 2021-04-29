
struct rectXY
{
  uint16_t X1;
  uint16_t Y1;
  uint16_t X2;
  uint16_t Y2;
};

rectXY soilTooDryDisplayimits{90, 60, 150, 120};
rectXY valveOpenClosedDisplayLimits{155, 60, 215, 120};
rectXY moistureSetterBarButtonLimits{279, 60, 309, 215};
rectXY pot1ButtonLimits{20, 40, 150, 80};
rectXY pot2ButtonLimits{20, 90, 150, 130};
rectXY pot3ButtonLimits{20, 140, 150, 180};
rectXY returnButtonLimits{20, 60, 80, 100};
rectXY activatePotButtonLimits{155, 125, 215, 185};

rectXY temperatureDisplayLimits{170, 190, 170 + 80, 190 + 40};

rectXY thermometerOuterLimits{280, 60, 280 + 20, 60 + 130};
rectXY thermometerInnerLimits{thermometerOuterLimits.X1 + 4, thermometerOuterLimits.Y1, thermometerOuterLimits.X2 - 4, thermometerOuterLimits.Y2};

bool pointIsInsideButtonLimits(uint16_t x, uint16_t y,  rectXY ButtonLimits)
{
  return (x >= ButtonLimits.X1) && (x <= ButtonLimits.X2) && (y >= ButtonLimits.Y1) && (y <= ButtonLimits.Y2);
}