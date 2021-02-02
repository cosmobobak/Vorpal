#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
using namespace std;

class Player {
   public:
    string name;
    int accuracy;

    Player(string name, int accuracy) {
        this->name = name;
        this->accuracy = accuracy;
    }

    auto calculation() -> int {
        int counter = 0;
        int total = 0;
        while (counter < 10) {
            srand((unsigned)time(0));
            int randomNumber = (rand() % 100) + 1;
            //cout << randomNumber;
            if (randomNumber > accuracy) {
                counter++;
            }
            total++;
        }
        return total;
    }
};

int main() {
    srand(time(NULL));
    Player p1 = Player("Joe", 70);

    cout << "it took " << p1.name << " " << p1.calculation() << " attempts to hit the target 10 times at " << p1.accuracy << "% accuracy" << endl;
}