//
// Created by Thinker on 11/16/2020.
//

#include <iostream>
#include "myListFuncs.h"

using namespace std;

bool isCircularList(Node* list) {
    Node *p = list;
    if (p == NULL) {
        return false;
    }
    while (p -> next != NULL && p -> next != list) {
        p = p -> next;
    }
    if (p -> next == list) {
        return true;
    }
    return false;
}
int main() {
    Node *theList = NULL;
    mylistInsertLast(theList, 5);
    mylistInsertLast(theList, 9);
    mylistInsertLast(theList, 4);

    std::cout << "Before remove pb9" << std::endl;
    myprintAllList(theList);

    std::cout << "F 16 After s insert pb9" << std::endl;

    myprintAllList(theList);


    return 0;
}