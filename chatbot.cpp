#include <iostream>

bool is_valid(char item)
{
    char arr[4] = {'y', 'Y', 'n', 'N'}; //defining set of valid answers
    for (int i = 0; i < 4; i++)         //loop four times
    { 
        if (arr[i] == item)
        {
            return true; //if we have the value -> return true
        }
    }
    return false; //if we don't find it -> return false
}

int main()
{
    char ans;
    std::cout << "Are you having a good day? (Y/N)\n"; //ask question
    std::cin >> ans;                                   //get answer
    while (is_valid(ans) != true)                      //while we don't get something in [yYnN], loop
    {
        std::cout << "Are you having a good day? (Y/N)\n"; //ask question
        std::cin >> ans;                                   //get answer
    }

    if (ans == 'y' || ans == 'Y') //if yes
    {
        std::cout << "That's great to hear\n"; //this is obvious
    }
    else if (ans == 'n' || ans == 'N') //if no
    {
        std::cout << "I'm sorry to hear that\n"; //this is obvious
    }
}