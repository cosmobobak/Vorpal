#include <iostream>

int raiseP(int base, int exp)
{
    int acc = 1;
    for (int i = 0; i < exp; i++)
    {
        acc *= base;
    }
    return acc;
}

int char_to_int(char *c)
{
    switch (*c)
    {
    case '0':
        return 0;
        break;
    case '1':
        return 1;
        break;
    case '2':
        return 2;
        break;
    case '3':
        return 3;
        break;
    case '4':
        return 4;
        break;
    case '5':
        return 5;
        break;
    case '6':
        return 6;
        break;
    case '7':
        return 7;
        break;
    case '8':
        return 8;
        break;
    case '9':
        return 9;
        break;

    default:
        return 0;
        break;
    }
}

int timehandler_nd(char *start)
{
    int arr[6];
    int counter = 0;
    for (int i = 0; i < 8; i++)
    {
        if (i != 2 && i != 5)
        {
            arr[counter] = char_to_int(start+i);
            counter++;
        }
    }
    int total = 0;
    for (int i = 0; i < 6; i++)
    {
        total += arr[5 - i] * raiseP(10, i);
    }
    return total;
}

// double mapper(int hour)
// {
//     double a = 0.8888889 * hour - 0.2222222 * raiseP(hour, 2) + 0.01234568 * raiseP(hour, 3);
//     return a;
// }

void generate_clock(int time)
{
    int hour = time / 10000;
    int minute = (time - hour) / 100;
    int second = time - minute - hour;
    
    double hourX = 16 * sin(hAngle);
    double hourY = 16 * cos(hAngle);
}

int main()
{
    // char timething[24] = "20-12-10 12:32:56";
    // std::cout << timehandler_nd(timething + 9);
    // generate_clock(30000);
    // generate_clock(90000);
    for (int i = 1; i < 25; i++)
    {
        std::cout << i << " " << i%12 << "\n";
    }
    
}
