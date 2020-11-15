#include <iostream>
#include <string>

using namespace std;

int main()
{
    int cLights = 0;
    int cDarks = 0;
    int itemTemp = 0;
    int lightsMax = 60;
    int darksMax = 60;
    int washingTemp = 0;
    int maxItems = 10;
    string colour = "";

    while ((colour != "STOP") && (cLights < maxItems) && (cDarks < maxItems))
    {
        cout << "Enter light or dark(case sensitive) or STOP to quit ";
        cin >> colour;
        if (colour == "STOP")
        {
            break;
        }
        cout << "Enter temperature on label ";
        cin >> itemTemp;

        if (colour == "light")
        {
            cLights++;
            if (lightsMax > itemTemp)
            {
                lightsMax = itemTemp;
            }
        }
        else if (colour == "dark")
        {
            cDarks++;
            if (darksMax > itemTemp)
            {
                darksMax = itemTemp;
            }
        }
        else
        {
            //display error message
        }
    } //end while loop, user has entered STOP

    if (cLights > cDarks)
    {
        cout << " wash lights at " << lightsMax;
    }
    else
    {
        cout << " wash darks at " << darksMax;
    }
}