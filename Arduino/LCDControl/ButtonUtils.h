
struct rectXY
{
  uint16_t X1;
  uint16_t Y1;
  uint16_t X2;
  uint16_t Y2;
};

rectXY pumpOnOffButtonLimits{90, 60, 150, 120};
rectXY autoOnOffButtonLimits{155, 60, 215, 120};
rectXY moistureSetterBarButtonLimits{279, 60, 309, 215};
rectXY pump1ButtonLimits{20, 40, 150, 80};
rectXY pump2ButtonLimits{20, 90, 150, 130};
rectXY pump3ButtonLimits{20, 140, 150, 180};
rectXY returnButtonLimits{20, 60, 80, 100};
rectXY temperatureDisplayLimits{170, 190, 250, 230};

bool pointIsInsideButtonLimits(uint16_t x, uint16_t y,  rectXY ButtonLimits)
{
  return (x >= ButtonLimits.X1) && (x <= ButtonLimits.X2) && (y >= ButtonLimits.Y1) && (y <= ButtonLimits.Y2);
}