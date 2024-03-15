#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>

// #define TFT_DC 16
// #define TFT_CS 22
// #define TFT_BL 34
// #define TFT_RS 21


// Meter colour schemes
// #define RED2RED 0
// #define GREEN2GREEN 1
// #define BLUE2BLUE 2
// #define BLUE2RED 3
// #define GREEN2RED 4
// #define RED2GREEN 5

// Adafruit_GC9A01A tft(TFT_CS, TFT_DC,TFT_RS);

// #########################################################################
// Return a 16 bit rainbow colour
// #########################################################################
unsigned int rainbow(byte value)
{
    // Value is expected to be in range 0-127
    // The value is converted to a spectrum colour from 0 = blue through to 127 = red

    byte red = 0;   // Red is the top 5 bits of a 16 bit colour value
    byte green = 0; // Green is the middle 6 bits
    byte blue = 0;  // Blue is the bottom 5 bits

    byte quadrant = value / 32;

    if (quadrant == 0)
    {
        blue = 31;
        green = 2 * (value % 32);
        red = 0;
    }
    if (quadrant == 1)
    {
        blue = 31 - (value % 32);
        green = 63;
        red = 0;
    }
    if (quadrant == 2)
    {
        blue = 0;
        green = 63;
        red = value % 32;
    }
    if (quadrant == 3)
    {
        blue = 0;
        green = 63 - 2 * (value % 32);
        red = 31;
    }
    return (red << 11) + (green << 5) + blue;
}

// #########################################################################
// Return a value in range -1 to +1 for a given phase angle in degrees
// ###########################scheme##############################################
float sineWave(int phase)
{
    return sin(phase * 0.0174532925);
}

// #########################################################################
//  Draw the meter on the screen, returns x coord of righthand side
// #########################################################################
int ringMeter(int value, int vmin, int vmax, int w, int x, int y, int r, char *units, byte scheme)
{
    // Minimum value of r is about 52 before value text intrudes on ring
    // drawing the text first is an option

    x += r;
    y += r; // Calculate coords of centre of ring

    // int w = r / 4; // Width of outer ring is 1/4 of radius

    int angle = 150; // Half the sweep angle of meter (300 degrees)

    int text_colour = 0; // To hold the text colour

    int v = map(value, vmin, vmax, -angle, angle); // Map the value to an angle v

    byte seg = 5; // Segments are 5 degrees wide = 60 segments for 300 degrees
    byte inc = 5; // Draw segments every 5 degrees, increase to 10 for segmented ring

    // Draw colour blocks every inc degrees
    for (int i = -angle; i < angle; i += inc)
    {

        // Choose colour from scheme
        int colour = 0;
        switch (scheme)
        {
        case 0:
            colour = GC9A01A_RED;
            break; // Fixed colour
        case 1:
            colour = GC9A01A_GREEN;
            break; // Fixed colour
        case 2:
            colour = GC9A01A_BLUE;
            break; // Fixed colour
        case 3:
            colour = rainbow(map(i, -angle, angle, 0, 127));
            break; // Full spectrum blue to red
        case 4:
            colour = rainbow(map(i, -angle, angle, 63, 127));
            break; // Green to red (high temperature etc)
        case 5:
            colour = rainbow(map(i, -angle, angle, 127, 63));
            break; // Red to green (low battery etc)
        default:
            colour = GC9A01A_BLUE;
            break; // Fixed colour
        }

        // Calculate pair of coordinates for segment start
        float sx = cos((i - 90) * 0.0174532925);
        float sy = sin((i - 90) * 0.0174532925);
        uint16_t x0 = sx * (r - w) + x;
        uint16_t y0 = sy * (r - w) + y;
        uint16_t x1 = sx * r + x;
        uint16_t y1 = sy * r + y;

        // Calculate pair of coordinates for segment end
        float sx2 = cos((i + seg - 90) * 0.0174532925);
        float sy2 = sin((i + seg - 90) * 0.0174532925);
        int x2 = sx2 * (r - w) + x;
        int y2 = sy2 * (r - w) + y;
        int x3 = sx2 * r + x;
        int y3 = sy2 * r + y;

        if (i < v)
        { // Fill in coloured segments with 2 triangles
            tft.fillTriangle(x0, y0, x1, y1, x2, y2, colour);
            tft.fillTriangle(x1, y1, x2, y2, x3, y3, colour);
            text_colour = colour; // Save the last colour drawn
        }
        else // Fill in blank segments
        {
            tft.fillTriangle(x0, y0, x1, y1, x2, y2, GC9A01A_DARKGREY);
            tft.fillTriangle(x1, y1, x2, y2, x3, y3, GC9A01A_DARKGREY);
        }
    }

    // Convert value to a string
    char buf[10];
    byte len = 4;
    if (value > 999)
        len = 5;
    dtostrf(value, len, 0, buf);

    // Set the text colour to default
    tft.setTextColor(GC9A01A_WHITE, GC9A01A_BLACK);
    // Uncomment next line to set the text colour to the last segment value!
    // tft.setTextColor(text_colour, ILI9341_BLACK);

    // Print value, if the meter is large then use big font 6, othewise use 4
    // if (r > 84) tft.drawCentreString(buf, x - 5, y - 20, 6); // Value in middle
    // else tft.drawCentreString(buf, x - 5, y - 20, 4); // Value in middle

    // Print units, if the meter is large then use big font 4, othewise use 2
    tft.setTextColor(GC9A01A_WHITE, GC9A01A_BLACK);
    // if (r > 84) tft.drawCentreString(units, x, y + 30, 4); // Units display
    // else tft.drawCentreString(units, x, y + 5, 2); // Units display

    // Calculate and return right hand side x coordinate
    return x + r;
}

// #########################################################################
//  Draw the  line on meter on the screen
// #########################################################################
void ringMeterLine(int value, int vmin, int vmax, int width, int xCenter, int yCenter, int radius, uint16_t barColor)
{
    int angle = 150;
    int v = map(value, vmin, vmax, -angle, angle);

    byte seg = 5; // Segments are 5 degrees wide = 60 segments for 300 degrees
    byte inc = 5; // Draw segments every 5 degrees, increase to 10 for segmented ring

    // Calculate pair of coordinates for segment start
    float sx = cos((v - 90) * 0.0174532925);
    float sy = sin((v - 90) * 0.0174532925);
    uint16_t x0 = sx * (radius - width) + xCenter;
    uint16_t y0 = sy * (radius - width) + yCenter;
    uint16_t x1 = sx * radius + xCenter;
    uint16_t y1 = sy * radius + yCenter;

    // Calculate pair of coordinates for segment end
    float sx2 = cos((v + seg - 90) * 0.0174532925);
    float sy2 = sin((v + seg - 90) * 0.0174532925);
    int x2 = sx2 * (radius - width) + xCenter;
    int y2 = sy2 * (radius - width) + yCenter;
    int x3 = sx2 * radius + xCenter;
    int y3 = sy2 * radius + yCenter;

    tft.fillTriangle(x0, y0, x1, y1, x2, y2, barColor);
    tft.fillTriangle(x1, y1, x2, y2, x3, y3, barColor);
}