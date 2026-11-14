#include <iostream>
#include "../include/services/NavigationService.h"
using namespace std;

static void printVector(const vector<int>& v)
{
    if (v.empty())
    {
        cout << "(empty)";
        return;
    }
    for (int x : v)
        cout << x << " ";
}

int main()
{
    cout << "=== BFS shortest route ===" << endl;

    cout << "1 -> 8, expected: 1 5 8" << endl << "actual:   ";
    printVector(NavigationService::findRoute(1, 8));
    cout << endl;

    cout << "7 -> 5, expected: 7 8 5" << endl << "actual:   ";
    printVector(NavigationService::findRoute(7, 5));
    cout << endl;

    cout << "1 -> 1, expected: 1" << endl << "actual:   ";
    printVector(NavigationService::findRoute(1, 1));
    cout << endl;

    cout << "1 -> 99 (invalid), expected: (empty)" << endl << "actual:   ";
    printVector(NavigationService::findRoute(1, 99));
    cout << endl;

    cout << endl << "=== DFS from location 1 ===" << endl;
    cout << "expected: 1 2 3 7 8 5 6 4" << endl << "actual:   ";
    printVector(NavigationService::getReachable(1));
    cout << endl;

    return 0;
}