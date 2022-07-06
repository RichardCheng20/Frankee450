//
// Created by Thinker on 11/15/2020.
//

#include <iostream>
#include "myListFuncs.h"

using namespace std;

Node::Node(int item) {
    data = item;
    next = NULL;
}

Node::Node(int item, Node *n) {
    data = item;
    next = n;
}


void myinsertFrontList(ListType &list, int theValue) {
    Node *curNode = new Node(theValue);
    curNode->next = list;
    list = curNode;
}

bool mylistInsertLast(ListType & list, int theValue){
    Node *curPointer = list;
    if (curPointer == NULL) {
        list = new Node(theValue);
        return true;
    }
    while (curPointer->next != NULL) {
        curPointer = curPointer->next;
    }
    Node *newNode = new Node(theValue);
    curPointer->next = newNode;
    return true;
}

void myprintAllList(ListType &list) {
    if (list != NULL) {
        Node *pPrint = list;
        while (pPrint != NULL) {
            cout << pPrint->data << " ";
            pPrint = pPrint->next;
        }
        cout << endl;
    }
}
