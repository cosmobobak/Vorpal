#include <iostream>
#include <string>

int main() {
    std::string curr;                                            //this defines a string variable for entry from the user
    std::cout << "Enter the current time in format hh:mm:ss\n";  //just for prettiness
    std::cin >> curr;                                            //user entry happens on this line
    std::string h = curr.substr(0, 2);                           //hours are extracted from string
    std::string m = curr.substr(3, 2);                           //minutes are extracted from string
    std::string s = curr.substr(6, 2);                           //seconds are extracted from string

    std::cout
        << std::stoi(h) << " hours, "
        << std::stoi(m) << " minutes, "
        << std::stoi(s) << " seconds."
        << '\n';  //this is unnecessary but is pretty cool for the user

    int seconds = std::stoi(h) * 3600 + std::stoi(m) * 60 + std::stoi(s);  //calculates the seconds since midnight
    std::cout << seconds << " seconds since midnight.";                    //output
    return 0;                                                              //return for the function
}