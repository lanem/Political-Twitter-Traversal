// LinkedList is a generic outline of a linked list

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "Tweet.h" // Includes information for each Tweet object

template <class T> // Template to be used with any class *(course objective)*
// Defines the Node class we will use to traverse the list
// Value is referred to as tweet for readability of code, but to be truely generic should be renamed
class Node {
public:
	Node<T>* next = NULL; // Next tweet in the list
	T tweet; // Holds information within the node
	Node<T>(T t) { tweet = t; } // Constructor which assigns a given tweet
	Node<T>() { this = NULL; } // Overidden default constructor
};

// Outlines the linked list class
template <class T> // Template to be used with any class
class LinkedList {
protected:
	Node<T>* head; // Contains the first node of the list
public:
	LinkedList() { head = NULL; } // Constructor which creates the head as null
	~LinkedList(); // Destructor which deletes each node individually
	virtual void insert(T); // Inserts a node into the list *(course objective)*
	Node<T>* getHead() { return head; } // Returns the head of the list
};

// Destructor for a linked list which deletes each node
template <class T>
LinkedList<T>::~LinkedList() {
	Node<T> *nodePtr; // To traverse the list
	Node<T> *nextNode; // To point to the next node
	nodePtr = head; // Position nodePtr at the head of the list.

	// While nodePtr is not at the end of the list...
	while (nodePtr != NULL) {
		nextNode = nodePtr->next; // Save a pointer to the next node.
		delete nodePtr; // Delete the current node.
		nodePtr = nextNode; // Position nodePtr at the next node.
	}
}

// Insert inserts a given item of any type
template <class T>
void LinkedList<T>::insert(T item) {
	Node<T> *newNode = new Node<T>(item);
	// If the list is empty, put the new node as the head
	if (head == NULL) {
		head = newNode;
	}
	// Otherwise put it at the front
	else {
		newNode->next = head;
		head = newNode;
	}
}

#endif