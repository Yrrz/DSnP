/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(1) { _data = new T[_capacity]; }
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const  { return (*this); }
      T& operator * ()              { return (*_node); }
      iterator& operator ++ ()      { ++(this->_node); return *this; }
      iterator operator ++ (int)    { iterator it(_node); ++(this->_node); return it; }
      iterator& operator -- ()      { --(this->_node); return *this; }
      iterator operator -- (int)    { iterator it(_node); --(this->_node); return it; }

      iterator operator + (int i) const { return (_node + i); }
      iterator& operator += (int i) { 
         _node += i;
         return _node; 
      }

      iterator& operator = (const iterator& i) { _node = i._node; }

      bool operator != (const iterator& i) const { return ((_node != i._node) ? true : false ); }
      bool operator == (const iterator& i) const { return ((_node == i._node) ? true : false ); }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_data); }
   iterator end() const { return iterator(_data + _size); }
   bool empty() const { return (_size == 0) ? true : false ; }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return _data[i]; }
   const T& operator [] (size_t i) const { return _data[i]; }

   void push_back(const T& x) { 
      if (_size == _capacity) {
         T* _2size = new T[_capacity];
         for (size_t i=0 ; i < _capacity ; i++)
            _2size[i] = _data[i];
         delete [] _data;
         _data = new T[_capacity * 2];
         for (size_t i = 0; i < _capacity; ++i)  
            _data[i] = _2size[i];
         delete [] _2size;
         _capacity *= 2;
      }
      _data[_size] = x;
      _size++;
   }

   void pop_front() { 
      if (!empty())  {
         _data[0] = _data[_size-1];
         _size--;
      }
   }
   void pop_back() { 
      if (!empty())  
         _size--;
   }

   bool erase(iterator pos) { 
      if(!empty())  {
         *(pos) = _data[_size-1];
         _size --;
         return true;
      } 
      else
         return false;
   }

   bool erase(const T& x) { 
      if (!empty()) {
         for (size_t i=0 ; i<_size ; i++) {
            if (_data[i] == x) {
               _data[i] = _data[_size-1];
               _size--;
               return true;
            }
         }
      }
      return false; 
   }

   void clear() { _size = 0; }

   // [Optional TODO] Feel free to change, but DO NOT change ::sort()
   void sort() const { if (!empty()) ::sort(_data, _data+_size); }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
