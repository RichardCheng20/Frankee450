//
// Created by Thinker on 11/16/2020.
//

#include <iostream>
#include "myListFuncs.h"

using namespace std;

void gut(ListType & list) {
    if(list==NULL){
        return;
    }
    Node *p = list;
    Node *cur = p->next;
    while ( cur!=NULL && cur->next != NULL){
        Node *del = cur;
        cur = cur->next;
        p->next = cur;
        delete del;
    }
}
int main() {
    Node *theList = NULL;
    mylistInsertLast(theList, 2);
    mylistInsertLast(theList, 7);
    mylistInsertLast(theList, 3);
    mylistInsertLast(theList, 5);
    std::cout << "Before remove pb9" << std::endl;
    myprintAllList(theList);
    gut(theList);
    std::cout << "s 17 After s insert pb9" << std::endl;

    myprintAllList(theList);


    return 0;
}