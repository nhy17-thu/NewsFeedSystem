#include <iostream>
using namespace std;

int main()
{
    char **input = new char *;
    cin >> *input;
    char **test = input;
    (*test)[2] = 'k';
    cout << *test << endl << *input;

    return 0;
}
