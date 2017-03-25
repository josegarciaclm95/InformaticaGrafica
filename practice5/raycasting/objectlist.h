/* objectlist.h
 *
 * Informatica Grafica / Computer Graphics.
 * Spring 2017.
 *
 * Practice 5.
 * Visible-surface detection in OpenGL.
 *
 * Jose Pascual Molina Masso.
 * Escuela Superior de Ingenieria Informatica de Albacete.
 */

#ifndef _OBJECTLIST_H_
#define _OBJECTLIST_H_

#include "object.h"


/********************/
/* ObjectList class */
/********************/

class ObjectList {

public:

  /* Constructor and destructor */
  ObjectList();
  ~ObjectList();

  /* Functions */
  void Add(Object* optr);  // Insert at the front of the list
  Object* First();         // Returns the first object
  Object* Next();          // Returns the next object
  int Length();            // Length of list  

private:

  class Node {
  public:
    Object* optr;
	Node* next;
  };

  Node *first, *next;
  int count;
};

/* Constructor */
inline ObjectList::ObjectList() {
  first = NULL;
  next = NULL;
  count = 0;
}

/* Destructor */
inline ObjectList::~ObjectList() {
  while (first != NULL) {
	next = first->next;
	delete first;
	first = next;
  }
}

/* Insert at the front of the list */
inline void ObjectList::Add(Object *optr) {
  Node* nptr = new Node;

  nptr->optr = optr;
  nptr->next = first;
  first = nptr;
  count++;
}

/* Returns the first object */
inline Object* ObjectList::First() {
  if (first != NULL)
    next = first->next;
  return first->optr;
}

/* Returns the next object */
inline Object* ObjectList::Next() {
  if (next != NULL) {
    Node* nptr = next;
    next = next->next;
	return nptr->optr;
  }
  return NULL;
}

/* Length of list */
inline int ObjectList::Length() {
  return count;
}
  

#endif  // !defined _OBJECTLIST_H_
