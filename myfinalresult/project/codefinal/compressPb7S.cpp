//
// Created by Thinker on 11/15/2020.
//

#include <iostream>
#include "myListFuncs.h"

using namespace std;

void compress(ListType & list) {
    if (list == NULL || list -> next == NULL) {
        return;
    }
    Node *p = list;
    Node *cur = p -> next;
    while (cur != NULL) {
        if (cur -> data == p -> data) {
            Node *del = cur;
            cur = cur -> next;
            p -> next = cur;
            delete del;
        } else {
            p = cur;
            cur = cur->next;
        }
    }
}

void func1(ListType list) {
    list = NULL;
}

int main() {
//    int m = 20;
//    int *p = &m;
//    std::cout << "S16 *p: " << *p << std::endl;
    Node *theList = NULL;
    mylistInsertLast(theList, 2);
//    mylistInsertLast(theList, 2);
//    mylistInsertLast(theList, 2);
//    mylistInsertLast(theList, 3);
//    mylistInsertLast(theList, 3);
    mylistInsertLast(theList, 3);
    mylistInsertLast(theList, 1);
//    mylistInsertLast(theList, 2);
//    mylistInsertLast(theList, 2);
    std::cout << "Before remove pb9" << std::endl;
    myprintAllList(theList);
    std::cout << "S16 After fun1 remove pb9" << std::endl;
   // compress(theList);
    func1( theList);
    myprintAllList(theList);


    std::cout << "Hello, World! pb9" << std::endl;
    return 0;
}