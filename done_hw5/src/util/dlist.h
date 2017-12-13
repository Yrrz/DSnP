/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const  { return _node->_data; }
      T& operator * ()              { return _node->_data; }
      iterator& operator ++ ()      { _node = _node->_next; return *(this); }
      iterator operator ++ (int)    { iterator it(_node); _node = _node->_next; return it; }
      iterator& operator -- ()      { _node = _node->_prev; return *(this); }
      iterator operator -- (int)    { iterator it(_node); _node = _node->_prev; return it; }

      iterator& operator = (const iterator& i) { _node->_data = i._data;  _node->_prev = i._prev; _node->_next = i._next; return *(this); }

      bool operator != (const iterator& i) const { return (((_node->_data == i._node->_data) && (_node->_prev == i._node->_prev) && (_node->_next == i._node->_next)) ? false : true) ; }
      bool operator == (const iterator& i) const { return (((_node->_data == i._node->_data) && (_node->_prev == i._node->_prev) && (_node->_next == i._node->_next)) ? true : false) ; }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return _head; }
   iterator end() const { return _head->_prev ; }
   bool empty() const { return ((_head->_prev == _head) && (_head->_next == _head) ? true : false) ; }
 
   size_t size() const { 
      if (empty())
         return 0;
      else  {
         DListNode<T>* _tranode = new DListNode<T>(_head->_data,_head->_prev,_head->_next);
         size_t sizecnt = 0;
         while (_tranode->_next != _head)  {
            _tranode = _tranode->_next;
            sizecnt++;
         }
         return sizecnt; 
      }
   }

   void push_back(const T& x) { 
      if (!empty())   { 
         DListNode<T>* _new = new DListNode<T>(x,_head->_prev->_prev,_head->_prev);
         _head->_prev->_prev->_next = _new;
         _head->_prev->_prev = _new;
      }
      else {
         DListNode<T>* _new = new DListNode<T>(x,_head,_head);
         _head->_next = _head->_prev = _new;
         _head = _new;        
      }
   }

   void pop_front() { 
      if (!empty()) {
         DListNode<T>* tmp = _head;
         _head->_prev->_next = _head->_next;
         _head->_next->_prev = _head->_prev;
         _head = _head->_next;
         delete tmp;
      }
   }

   void pop_back() { 
      if (!empty())  {
         DListNode<T>* tmp = _head->_prev->_prev;
         _head->_prev->_prev->_prev->_next = _head->_prev;
         _head->_prev->_prev = _head->_prev->_prev->_prev;
         delete tmp;
      }
   }

   // return false if nothing to erase
   bool erase(iterator pos) { 
      if (!empty())  {
         pos._node->_prev->_next = pos._node->_next;
         pos._node->_next->_prev = pos._node->_prev;
         delete pos._node;
      }
      return false; 
   }
   bool erase(const T& x) { 
      if (!empty()) {
         DListNode<T>* _tranode = _head;
         while (_tranode->_next != _head) {
            if (_tranode->_data == x) {
               _tranode->_prev->_next = _tranode->_next;
               _tranode->_next->_prev = _tranode->_prev;
               delete _tranode;  
               return true;             
            }
            _tranode = _tranode->_next;
         }
      }
      return false;
   }

   void clear() { 
      while(_head->_next->_next != _head) {
         pop_back();
      }
      if (!empty())
         pop_back();
   }  // delete all nodes except for the dummy node

   void sort() const { 
      DListNode<T>* _fir = _head;
      DListNode<T>* _sec = _head->_next;
      for ( ; _fir->_next != _head ; _fir = _fir->_next) {
         for ( _sec = _fir->_next ; _sec->_next != _head ; _sec = _sec->_next ) {
            if (_fir->_data > _sec->_data) {
               T tmp = _fir->_data;
               _fir->_data = _sec->_data;
               _sec->_data = tmp;
            }
         }
      }
      //for ( _fir ; _fir->_next != )
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
