#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>      // <-- ADD THIS LINE for the 'pow' function
#include <algorithm>  // <-- ADD THIS LINE for the 'find' function
using namespace std;

class Activity {
public:
    string name;
    vector<string> prev;
    double a, m, b;
    double te, sigma2;
    double es, ef;
    double ls, lf;
    int slack;
};

class PERT {
private:
    vector<Activity> activities;
    vector<string> criticalPath;
public:
    void inputFromKeyboard();
    void inputFromFile(string filePath);
    void calculateTeSigma2();
    void calculateES_EF();
    void calculateLS_LF();
    void findCriticalPath();
    void outputResult();
    int findActivityIndex(string name);
};