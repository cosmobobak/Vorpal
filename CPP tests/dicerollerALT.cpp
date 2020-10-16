#include <cstdlib> // random numbers header file//
#include <iostream>
#include <ctime> // used to get date and time information
using namespace std;
int main()
{

    cout << "Dice rolling program" << endl;

    int replayAnswer;      //Initialise variable that deals with if the user wishes to play again
    int totalAttempts = 0; //Initialise the total number of times dice was rolled
    int totalSuccess = 0;  //Initialise the total times that 6 was recieved

    do
    {
        int attempts = 0; //Initialise the roll attempts at 0
        srand(time(0));   //initialise random num generator using time
        int roll = 0;     //declare a variable to keep store the random number

        do
        {
            roll = rand() % 6 + 1; // generate a random number between 1 and 6
            attempts++;            //Index the attempts as 6 was not rolled
            totalAttempts++;       //Index total number of attempts as 6 was not rolled
        } while (roll != 6);       //Repeat until the random number is a 6

        totalSuccess++;                           //Index the total success variable as since the for loop has been exited a 6 must have been recieved
        float average;                            // Initialise the value for average number of rolls for value of 6
        average = (totalAttempts / totalSuccess); //Calculate average by number of times that 6 was recieved divided by number of rolls

        cout << "Average number of rolls to get 6 so far: " << average << endl;
        cout << "Number of 6's average is based on: " << totalSuccess << endl;
        cout << "Number of times dice rolled before getting a 6: " << attempts << endl;
        cout << "Would you like to try get another 6? (postive number = yes, 0 = no)" << endl;
        cin >> replayAnswer; //Set user input integer to variable replayAnswer

    } while (replayAnswer != 0); //Loop dice rolling game until user inputs 0 at end
}