//
// Created by Thinker on 11/16/2020.
//

#include <iostream>
#include "myListFuncs.h"

using namespace std;
void removeEveryOther(ListType & list) {
    if (list == NULL) {
        return;
    }
    Node *p = list;
    if (p -> next == NULL) {
        return;
    }
    while (p -> next != NULL) {
        Node *del = p -> next;
        p -> next = p -> next -> next;
        delete del;
        p = p -> next;
    }
}

int main() {
    Node *theList = NULL;
 //   mylistInsertLast(theList, 10);
//    mylistInsertLast(theList, 8);
//    mylistInsertLast(theList, 6);
//    mylistInsertLast(theList, 4);
//    mylistInsertLast(theList, 2);
    std::cout << "Before remove pb9" << std::endl;
    myprintAllList(theList);
    std::cout << "After remove other pb9" << std::endl;
    removeEveryOther(theList);
    myprintAllList(theList);


    return 0;
}