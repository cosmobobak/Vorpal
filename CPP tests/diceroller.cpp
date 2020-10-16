#include <iostream> // for IO
#include <cstdlib>  // for RNG
#include <ctime>    // used to get date and time information

auto roll_dice() -> int // return a random int from 1 -> 6
{
    int roll = 0;          // declare a variable to keep store the random number
    roll = rand() % 6 + 1; // generate a random number between 1 and 6
    return roll;
}

auto main() -> int
{
    srand(time(0));      // initialise random num generator using time
    int currentRoll = 0; // init storage for the function output
    int iterations = 0;  // tally of dice rolled
    int total = 0;       // tally of dice rolled across all runs
    int rerun = 0;       // flag for repeating the run
    int runs = 0;        // tally of runs

    do
    {
        runs++;         // increment runs
        iterations = 0; // reset iterations

        do
        {
            currentRoll = roll_dice(); // roll the dice
            iterations++;              // increment iterations
        } while (currentRoll != 6);    // exit if a six is found

        total += iterations; // add single-run iterations to the total

        std::cout << "Iterations for a six: ";
        std::cout << iterations << '\n';
        std::cout << "Average iterations per six: " << total / runs;
        std::cout << ", based on " << runs << " runs." << '\n';
        std::cout << "Would you like to run again? [positive to continue, 0 to exit]: ";
        std::cin >> rerun; // get from user
    } while (rerun > 0);   // exit if user gives 0
}