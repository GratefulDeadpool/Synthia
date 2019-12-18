class Updater
{
  private:
    int commaIndex1;
    int commaIndex2;
    int commaIndex3;
    int commaIndex4;
    int commaIndex5;
    
    String xTemp;
    String yTemp;
    String zTemp;
    
    double x = 0;
    double y = 0;
    double z = 0;
    
    double tiltAdjust(String tilt, double mini, double maxi)
    {
      if (tilt.indexOf("E-", 0) > -1)
      {
        return 0;
      }
      else if (tilt.indexOf("E", 0) > -1)
      {
        if (tilt.indexOf("-", 0) > -1)
        {
          return mini;
        }
        else
        {
          return maxi;
        }
      }
      else
      {
        return constrain(tilt.toFloat(), mini, maxi);
      }
    } 
      
  public:
    void updateTilt(String BTstring, double bR1, double bR2, double bP1, double bP2, double bY1, double bY2)
    {
      commaIndex1 = BTstring.indexOf(",", 0);
      commaIndex2 = BTstring.indexOf(",", commaIndex1 + 1);
      commaIndex3 = BTstring.indexOf(",", commaIndex2 + 1);
      commaIndex4 = BTstring.indexOf(",", commaIndex3 + 1);
      commaIndex5 = BTstring.indexOf(",", commaIndex4 + 1);
      
      xTemp = BTstring.substring(commaIndex2 + 1, commaIndex3);
      yTemp = BTstring.substring(commaIndex3 + 1, commaIndex4);
      zTemp = BTstring.substring(commaIndex4 + 1, commaIndex5);
      
      x = tiltAdjust(xTemp, bR1, bR2);
      y = tiltAdjust(yTemp, bP1, bP2);
      z = tiltAdjust(zTemp, bY1, bY2);
    }
  
    double getX()
    {
      return x;
    }
    double getY()
    {
      return y;
    }
    double getZ()
    {
      return z;
    }
};
