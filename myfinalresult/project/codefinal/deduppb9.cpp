//
// Created by Thinker on 11/16/2020.
//

#include <iostream>
#include "myListFuncs.h"

using namespace std;

void dupe(ListType & list) {
    if (list == NULL) {
        return;
    }
    Node *p = list;
    while (p != NULL) {
        Node *newNode = new Node( p -> data);
        newNode-> next = p -> next;
        p->next = newNode;
        p = p -> next -> next;
    }
}
int main() {
    Node *theList = NULL;
    mylistInsertLast(theList, 2);
//    mylistInsertLast(theList, 7);
//    mylistInsertLast(theList, 3);
    std::cout << "Before remove pb9" << std::endl;
    myprintAllList(theList);
    std::cout << "After insert pb9" << std::endl;
    dupe(theList);
    myprintAllList(theList);


    return 0;
}