//
// Created by Thinker on 11/15/2020.
//

#include <iostream>
#include "myListFuncs.h"

using namespace std;

bool allUnique(ListType list) {
    if (list == NULL || list->next == NULL) {
        return true;
    }
    Node *p = list;
    while (p != NULL) {
        Node *cur = p->next;
        while (cur != NULL) {
            if (cur->data != p->data) {
                cur = cur->next;
            } else {
                return false;
            }
        }
        p = p->next;
    }
    return true;
}

int main() {
    Node *theList = NULL;
    mylistInsertLast(theList, 4);
//    mylistInsertLast(theList, 3);
//    mylistInsertLast(theList, 7);
//    mylistInsertLast(theList, 3);
//    mylistInsertLast(theList, 4);
//    mylistInsertLast(theList, 5);
   // mylistInsertLast(theList, 1);
//    mylistInsertLast(theList, 2);
//    mylistInsertLast(theList, 2);
    std::cout << "Before remove pb9" << std::endl;
    myprintAllList(theList);
    std::cout << "F15 After remove pb9" << std::endl;
    std::cout << "Unique??" << allUnique(theList) << std::endl;
    myprintAllList(theList);


    std::cout << "Hello, World! pb9" << std::endl;
    return 0;
}