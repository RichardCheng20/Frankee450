//
// Created by Thinker on 11/16/2020.
//

#include <iostream>
#include "myListFuncs.h"

using namespace std;

int numRuns(ListType list) {
    int nums = 0;
    if (list == NULL) {
        return nums;
    }
    Node *prev = list;
    Node *cur = list->next;
    int numSame = 0;
    while (cur != NULL) {
        if (cur->data == prev->data) {
            while (cur != NULL && cur -> data == prev -> data) {
                prev = cur;
                cur = cur -> next;
                numSame++;
            }
            if (numSame > 0) {
                nums++;
            }
            numSame = 0;
        }
        if (cur != NULL) {
            prev = cur;
            cur = cur -> next;
        }
    }
    return nums;
}


int main() {
    Node *theList = NULL;
    mylistInsertLast(theList, 2);
    mylistInsertLast(theList, 2);
    mylistInsertLast(theList, 7);
    mylistInsertLast(theList, 7);
    mylistInsertLast(theList, 7);
    mylistInsertLast(theList, 4);
    mylistInsertLast(theList, 4);
    mylistInsertLast(theList, 4);
    mylistInsertLast(theList, 4);
//    mylistInsertLast(theList, 4);
//    mylistInsertLast(theList, 4);
    std::cout << "Before remove pb9" << std::endl;
    myprintAllList(theList);
    std::cout << "After numRuns pb9" << std::endl;
    std::cout <<  numRuns(theList) << std::endl;

    return 0;
}