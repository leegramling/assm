// library.cpp - Compile as separate object
int mysterious_function(int a, int b) {
    int temp = a * 2;
    if (temp > b) {
        return temp + b;
    } else {
        return temp - b;
    }
}