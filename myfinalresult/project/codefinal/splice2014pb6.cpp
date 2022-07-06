//
// Created by Thinker on 11/15/2020.
//

#include <iostream>
#include "myListFuncs.h"

using namespace std;

void splice(ListType & list, int value, ListType subList) {
    if (list == NULL) {
        list = subList;
    }
    if (subList == NULL) {
        return;
    }
    if (value == 0) {
        Node *subP = subList;
        while (subP -> next != NULL) {
            subP = subP -> next;
        }
        subP -> next = list;
        list = subList;
        return;
    }

    //k is large, add at the end of list
    Node *p = list;
    int countlen = 0;
    while (p -> next != NULL) {
        p = p -> next;
        countlen++;
    }
    if (value >= countlen + 1) {
        p -> next = subList;
        return;
    }

    Node *pmid = list;
    int count = 0;
    while (pmid != NULL && (count != value - 1)) {
        pmid = pmid -> next;
        count++;
    }
    Node *subP = subList;
    while (subP -> next != NULL) {
        subP = subP -> next;
    }
    subP -> next = pmid -> next;
    pmid -> next = subList;
}


int main() {
    Node *theList = NULL;
    mylistInsertLast(theList, 2);
    mylistInsertLast(theList, 3);
    mylistInsertLast(theList, 8);
    mylistInsertLast(theList, 3);
    mylistInsertLast(theList, 9);
    Node *subList = NULL;
    mylistInsertLast(subList, 6);
    mylistInsertLast(subList, 6);
    mylistInsertLast(subList, 6);
//    mylistInsertLast(theList, 2);
    std::cout << "Before remove pb9" << std::endl;
    myprintAllList(theList);


    std::cout << "2014 After fun1 remove pb9" << std::endl;
    splice(theList, 5, subList);

    myprintAllList(theList);


    std::cout << "Hello, World! pb9" << std::endl;
    return 0;
}