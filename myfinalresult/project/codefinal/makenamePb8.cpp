//
// Created by Thinker on 11/15/2020.
//fall 19

#include <iostream>
#include <cstring>
using namespace std;
char * makename(const char * first, const char * middle, const char * last) {
    int firstLen = strlen(first);
    int middleLen = strlen(middle);
    int lastLen = strlen(last);
    char * result = new char[firstLen + middleLen + lastLen];
    strcpy(result, first);
    strcat(result, " ");
    strcat(result, middle);
    strcat(result, " ");
    strcat(result, last);
    return result;
}
int main() {
    char first[50];
    char middle[50];
    char last[50];
    strcpy(first, "Joe");
    strcpy(middle, "P.");
    strcpy(last, "Blow");
    cout <<makename(first, middle, last) << endl;
    cout << "Hello, World! final" << endl;
    return 0;
}
