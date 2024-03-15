
double previousTemp;
double previousTarget;
String strActualTemp;
// put function declarations here:

uint32_t runTime = -99999; // time for next update
float reading = 0;         // Value to be displayed

int d = 0; // Variable used for the sinewave test waveform

long getDecimal(float val)
{
    int returnvalue = 0;
    int intPart = int(val);
    long decPart = 1000 * (val - intPart); // I am multiplying by 1000 assuming that the foat values will have a maximum of 3 decimal places.
    // Change to match the number of decimal places you need
    if (decPart > 0)
    {
        returnvalue = (decPart);
    };
    // return the decimal part of float number if it is available

    if (decPart < 0)
    {
        returnvalue = ((-1) * decPart);
    } // if negative, multiply by -1
    if (decPart == 0)
    {
        returnvalue = (0);
    } // return 0 if decimal part of float number is not available

    return returnvalue;
};

void centerText(String dText, int dTextSize, uint16_t dTextColor, int yoffset)
{
    int dTextLen = dText.length();
    tft.setTextSize(dTextSize);
    tft.setTextColor(dTextColor);
    int lineWidth = (dTextLen * dTextSize * 6);
    int lineHeight = (8 * dTextSize);

    int curX = 240 / 2 - lineWidth / 2;
    // int dTextLine = 240 / 2 - lineHeight / 2;
    //  Serial.println(curX);
    //  Serial.println(dTextLine);

    tft.setCursor(curX, yoffset);
    // tft.setTextColor(GC9A01A_BLACK);
    // tft.print(dText);
    tft.setTextColor(dTextColor);
    tft.println(dText);
}
void displayTemp()
{
    // reading = 999.9;
    if (reading != previousTemp)
    {
        String displaypreviousTemp = String(int(previousTemp)) + "." + (String(getDecimal(previousTemp)).substring(0, 1));
        strActualTemp = String(int(reading)) + "." + (String(getDecimal(reading))).substring(0, 1);
        String displayString = String(int(reading)) + "." + (String(getDecimal(reading))).substring(0, 1);
        displaypreviousTemp.trim();
        displayString.trim();
        int yoffset = 130;
        centerText(displaypreviousTemp, 4, GC9A01A_BLACK, yoffset);
        centerText(displayString, 4, GC9A01A_WHITE, yoffset);
        previousTemp = reading;
    }
}

void displayTimeText(uint16_t dTextColor)
{
    int yoffset = 90;
    sprintf(timebuffer, "%02d:%02d:%02d", tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec);
    centerText(timebuffer, 2, dTextColor, yoffset);
}

// void displayTimerRemaining( uint16_t dTextColor){
//        int yoffset=90;
//        timerRemaining=expireTime-currentEpochTime;
//    sprintf(timebuffer,"%02d:%02d:%02d",timerRemaining.tm_hour, timerRemaining.tm_min, timerRemaining.tm_sec);
//      centerText(timebuffer, 2, dTextColor , yoffset);
// }
void displayTargetLabel()
{
    int yoffset = 30;
    centerText("TARGET", 2, GC9A01A_WHITE, yoffset);
}
void displayActualLabel()
{
    int yoffset = 110;
    centerText("ACTUAL", 2, GC9A01A_WHITE, yoffset);
}
void displayTarget()
{
//          Serial.println(" targetTemp_int ");
//          Serial.println(targetTemp_int);
    if (targetTemp_int != previousTarget)
    {
        int dTextSize = 4;

        String displaypreviousTarget = String(int(previousTarget)) + "." + (String(getDecimal(previousTarget)).substring(0, 1));
        String displayTarget = String(int(targetTemp_int)) + "." + (String(getDecimal(targetTemp_int))).substring(0, 1);
        displaypreviousTarget.trim();
        int yoffset = 50;
        centerText(displaypreviousTarget, 4, GC9A01A_BLACK, yoffset);
        centerText(displayTarget, 4, GC9A01A_WHITE, yoffset);
       

    }
}

void displayTempRing()
{

    if (millis() - runTime >= 1000L)
    { // Execute every 2s
        runTime = millis();

        // Test with a slowly changing value from a Sine function
        d += 1;
        if (d >= 360)
            d = 0;

        // Set the the position, gap between meters, and inner radius of the meters
        int xpos = 120 - 120, ypos = 120 - 120, gap = 4, radius = 120;

        // Draw meter and get back x position of next meter

        // Test with Sine wave function, normally reading will be from a sensor
        reading = actualTemp; // 250 + 250 * sineWave(d + 0);
        // Serial.print(actualTemp);
        // Serial.print(" ");
        // Serial.print(250 + 250 * sineWave(d + 0));
        // Serial.print(" ");
        // Serial.println(sineWave(d + 0));

        int width = 10;
        xpos = gap + ringMeter(reading, 50, 200, width, xpos, ypos, radius, "mA", GREEN2GREEN); // Draw analogue meter

        //reading = 20 + 30 * sineWave(d + 60);
        // xpos = gap + ringMeter(reading, -10, 50, xpos, ypos, radius, "degC", BLUE2RED); // Draw analogue meter

        //reading = 50 + 50 * sineWave(d + 120);
        // ringMeter(reading, 0, 100, xpos, ypos, radius, "%RH", BLUE2BLUE); // Draw analogue meter

        // Draw two more larger meters
        //xpos = 20, ypos = 115, gap = 24, radius = 64;

        //reading = 1000 + 150 * sineWave(d + 90);
        // xpos = gap + ringMeter(reading, 850, 1150, xpos, ypos, radius, "mb", BLUE2RED); // Draw analogue meter

        //reading = 15 + 15 * sineWave(d + 150);
        // xpos = gap + ringMeter(reading, 0, 30, xpos, ypos, radius, "Volts", GREEN2GREEN); // Draw analogue meter

        // Draw a large meter
        //xpos = 40, ypos = 5, gap = 15, radius = 120;
        //reading = 175;
        // Comment out above meters, then uncomment the next line to show large meter
        // ringMeter(reading,0,200, xpos,ypos,radius," Watts",GREEN2RED); // Draw analogue meter
    }
}

void displayTimeRing()
{

    if (millis() - runTime >= 1000L)
    { // Execute every 2s
        runTime = millis();

        String timerValue = "00:00:00";
        // Test with a slowly changing value from a Sine function
        d += 1;
        if (d >= 360)
            d = 0;

        // Set the the position, gap between meters, and inner radius of the meters
        int xpos = 120 - 120, ypos = 120 - 120, gap = 4, radius = 120;

        // Draw meter and get back x position of next meter

        int width = 10;
        ringMeter(reading, 50, 200, width, xpos, ypos, radius, "Time", RED2GREEN); // Draw analogue meter

        // Comment out above meters, then uncomment the next line to show large meter
        // ringMeter(reading,0,200, xpos,ypos,radius," Watts",GREEN2RED); // Draw analogue meter
    }
}

void displayTargetLine(){
      int width = 10;
  ringMeterLine(targetTemp_int, 50, 200, width, 120, 120, 120, GC9A01A_RED);
}