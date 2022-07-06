//
// Created by Thinker on 11/15/2020.
//

//
// Created by Thinker on 11/15/2020.
//

#include <iostream>
#include "myListFuncs.h"

using namespace std;

void insertAt(ListType & list, int val, int loc) {
    int myloc = 0;
    int listlen = 1;
    Node *p = list;
    Node *count = list;
    Node *newNode = new Node(val);
    if (list == NULL) {
        list = newNode;
        return;
    }
    if (loc == 0) {
        newNode -> next = list;
        list = newNode;
        return;
    }
    while (count -> next != NULL) {
        count = count ->next;
        listlen++;
    }
    if (loc >= listlen) {
        count -> next = newNode;
        return;
    }
    while (p != NULL && (myloc < loc - 1)) {
        myloc++;
        p = p -> next;
    }
    newNode->next = p->next;
    p -> next = newNode;

}

//void splice(ListType &list, ListType subList) {
//    Node *p = subList;
//    while (p -> next != NULL) {
//        p = p -> next;
//    }
//    p -> next = list;
//}

int main() {
    Node *theList = NULL;
    mylistInsertLast(theList, 2);
    mylistInsertLast(theList, 7);
    mylistInsertLast(theList, 3);
    std::cout << "Before remove pb9" << std::endl;
    myprintAllList(theList);
    std::cout << "After insert pb9" << std::endl;
    insertAt(theList, 44, 0);
    myprintAllList(theList);


    return 0;
}