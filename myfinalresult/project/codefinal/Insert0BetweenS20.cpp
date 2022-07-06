//
// Created by Thinker on 11/15/2020.
//

#include <iostream>
#include "myListFuncs.h"

using namespace std;

void insert0Between(ListType &list) {
  Node *p = list;
  if (list == NULL || list->next == NULL) {
      return;
  }
  while(p -> next != NULL) {
      Node *zero = new Node(0);
      zero -> next = p -> next;
      p->next = zero;
      p = p->next->next;
  }
}

int main() {
    Node *theList = NULL;
    mylistInsertLast(theList, 4);
    mylistInsertLast(theList, 3);
    mylistInsertLast(theList, 7);
//    mylistInsertLast(theList, 3);
//    mylistInsertLast(theList, 4);
//    mylistInsertLast(theList, 5);
    // mylistInsertLast(theList, 1);
//    mylistInsertLast(theList, 2);
//    mylistInsertLast(theList, 2);
    std::cout << "Before remove pb9" << std::endl;
    myprintAllList(theList);
    std::cout << "S20 After INSERT pb9" << std::endl;
    insert0Between(theList);
    myprintAllList(theList);


    std::cout << "Hello, World! pb9" << std::endl;
    return 0;
}