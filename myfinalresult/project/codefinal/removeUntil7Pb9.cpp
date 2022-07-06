//
// Created by Thinker on 11/15/2020.
//

#include <iostream>
#include "myListFuncs.h"

using namespace std;

void removeUntil7(ListType & list) {
    Node * p = list;
    if (list == NULL) {
        return;
    }
    while (p->data != 7 && p != NULL) {
        Node *del = p;
        p = p->next;
        delete del;
    }
    list = p;
}

int main() {
    Node *theList = NULL;
    mylistInsertLast(theList, 5);
    mylistInsertLast(theList, 3);
//    mylistInsertLast(theList, 7);
   mylistInsertLast(theList, 4);
//    mylistInsertLast(theList, 2);
//    mylistInsertLast(theList, 7);
    std::cout << "Before remove pb9" << std::endl;
    myprintAllList(theList);
    std::cout << "After remove pb9" << std::endl;
    removeUntil7(theList);
    myprintAllList(theList);


    std::cout << "Hello, World! pb9" << std::endl;
    return 0;
}