#include <iostream>
#include <string>

template <class T>
void print_array(T arr[], int len)
{
    std::cout << "{ ";
    for (size_t i = 0; i < len; i++)
    {
        std::cout << arr[i] << ", ";
    }
    std::cout << "}" << std::endl;
}

void sort(int arr[], int len)
{
    int holePos;
    int value;
    for (int i = 0; i < len; i++)
    {
        holePos = i;
        value = arr[i];
        while (holePos > 0 && arr[holePos - 1] > value)
        {
            arr[holePos] = arr[holePos - 1];
            holePos--;
        }
        arr[holePos] = value;
    }
}

auto main() -> int
{
    int array[10] = {1, 5, 7, 3, 4, 8, 6, 3, 9, 1};

    print_array(array, 10);

    sort(array, 10);

    print_array(array, 10);

    return 0;
}
