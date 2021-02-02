#include <iostream>
#include <string>

enum hungerLevels {
    wellFed,
    slightlyPeckish,
    ratherHungery,
    starving,
    dead
};

enum tiredLevels {
    wideAwake,
    awake,
    tired,
    fallingAsleep,
    collapsed
};

std::string hungerLevels[] = {
    "wellFed",
    "slightlyPeckish",
    "ratherHungery",
    "starving",
    "dead"};

std::string tiredLevels[] = {
    "wideAwake",
    "awake",
    "tired",
    "fallingAsleep",
    "collapsed"};

class CyberPet {
   public:
    int hunger = wellFed;
    int tiredness = wideAwake;
    std::string name = "your pet";

    CyberPet() {
        bool ans;
        std::cout << "would you like to name your pet? [1/0]\n--> ";
        std::cin >> ans;
        if (ans) {
            std::cout << "enter your pet's name:\n--> ";
            std::cin >> name;
        }
    }

    void feed() {
        hunger -= 1;
        if (hunger < 0) {
            hunger = 0;
        }
    }

    void sleep() {
        tiredness -= 1;
        if (tiredness < 0) {
            tiredness = 0;
        }
    }

    auto happiness_level() -> int {
        return 8 - hunger - tiredness;
    }

    void show_state() {
        std::cout << name << "'s current hunger is " << hungerLevels[hunger] << '\n';
        std::cout << name << "'s current tiredness is " << tiredLevels[tiredness] << '\n';
    }

    auto is_dead() -> bool {
        int numFives = 0;
        numFives += hunger == 5 ? 1 : 0;
        numFives += tiredness == 5 ? 1 : 0;
        numFives += happiness == 5 ? 1 : 0;
        return numFives > 2;
    }
};

auto main() -> int {
    CyberPet pet;
    pet.show_state();
    int action = 0;
    while (action != 1) {
        std::cout << "choose [1] Quit, [2] Feed, [3] Give a Nap, [4] Display Current State.\n--> ";
        std::cin >> action;
        switch (action) {
            case 2:
                pet.feed();
                break;
            case 3:
                pet.sleep();
                break;
            case 4:
                pet.show_state();
                break;
            default:
                break;
        }
    }

    return 0;
}
