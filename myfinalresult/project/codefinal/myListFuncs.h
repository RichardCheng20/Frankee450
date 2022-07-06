//
// Created by Thinker on 11/15/2020.
//

#ifndef MYCPLUSMULTI_MYLISTFUNCS_H
#define MYCPLUSMULTI_MYLISTFUNCS_H


struct Node {
    int data;
    Node * next;
    Node(int item); // create a Node with the given value and NULL next field
// create a Node with the given data value (item) and next field (n)
    Node(int item, Node * n);
};
typedef Node * ListType;

bool mylistInsertLast(ListType & list, int theValue);
void myprintAllList(ListType &list);
void myinsertFrontList(ListType &list, int theValue);

#endif //MYCPLUSMULTI_MYLISTFUNCS_H
