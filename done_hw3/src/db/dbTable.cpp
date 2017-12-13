/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include <climits>
#include <vector>
#include "dbTable.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   for (size_t i=0 ; i<r.size() ; i++) {
      if (r[i]==INT_MAX)
         os << '.';
      else 
         os << r[i];
      if (i!=r.size()-1)
         os << ' ';
   }
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   for (size_t i=0 ; i<t.nRows() ; i++) {
      for (size_t j=0 ; j<t.nCols() ; j++) {
         os << setw(6);
         if (t[i][j]==INT_MAX) 
            os << '.';
         else
            os << t[i][j];
      }
      os << endl;
   }
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells are printed as '.'
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   string line;
   while (getline(ifs,line,'\n')) {
      DBRow r;
      if ( !(int(line.back())==44||isdigit(line.back()))) 
         line.pop_back();
      size_t pos = line.find_first_of(",");
      while (pos!=string::npos) {
         if ((line.substr(0,pos).size())==0) 
            r.addData(INT_MAX);
         else
            r.addData(atoi(line.substr(0,pos).c_str()));
         line.erase(0,pos+1);
         pos = line.find_first_of(",");
      }
      if ((line.substr(0,pos).size())==0) 
         r.addData(INT_MAX);
      else
         r.addData(atoi(line.substr(0,pos).c_str()));
      t.addRow(r);
   }
   // TODO: to read in data from csv file and store them in a table
   // - You can assume the input file is with correct csv file format
   // - NO NEED to handle error file format
   return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
   _data.erase(_data.begin()+c);
   // TODO
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   for (size_t i=0 ; i<_sortOrder.size() ; i++) {
      if (r1[_sortOrder[i]] > r2[_sortOrder[i]])  {
         return 1;
      }
   }
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
   return 0;
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   while(nCols()>0) 
      delRow(0);
   // TODO
}

void
DBTable::addCol(const vector<int>& d)
{
   if (d.size()>nRows()) {}
   else if (d.size() == nRows()) {
      for (size_t i=0 ; i<nRows() ; i++) {
         _table[i].addData(d[i]);
      }
   }
   else {
      for (size_t i=0 ; i<d.size() ; i++) 
         _table[i].addData(d[i]);
      for (size_t i=0 ; i<nRows()-d.size() ; i++)
         _table[i+d.size()].addData(INT_MAX);
   }   
   // TODO: add a column to the right of the table. Data are in 'd'.
}

void
DBTable::delRow(int c)
{
   _table.erase(_table.begin()+c);
   // TODO: delete row #c. Note #0 is the first row.
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
   int goodnum=0,max=INT_MIN;
   for (size_t i=0 ; i<nRows() ; i++) {
      if (_table[i][c] != INT_MAX) {
         goodnum++;
         if (_table[i][c] > max)
            max = _table[i][c];
      } 
   }
   if (goodnum)
      return float(max);
   else 
      return NAN;
   // TODO: get the max data in column #c
}

float
DBTable::getMin(size_t c) const
{
   int goodnum=0,min=INT_MAX-1;
   for (size_t i=0 ; i<nRows() ; i++) {
      if (_table[i][c] != INT_MAX) {
         goodnum++;
         if (_table[i][c] < min)
            min = _table[i][c];
      } 
   }
   if (goodnum)
      return float(min);
   else 
      return NAN;
   // TODO: get the min data in column #c
}

float 
DBTable::getSum(size_t c) const
{
   int goodnum=0,sum=0;
   for (size_t i=0 ; i<nRows() ; i++) {
      if (_table[i][c] != INT_MAX) {
         goodnum++;
         sum+=_table[i][c];
      } 
   }
   if (goodnum)
      return float(sum);
   else 
      return NAN;
   // TODO: compute the sum of data in column #c
}

int
DBTable::getCount(size_t c) const
{
   vector<int> v;
   int goodnum=0;
   bool neew=1;
   for (size_t i=0 ; i<nRows() ; i++) {
      if (_table[i][c]!=INT_MAX) {
         goodnum++;
         for (size_t j=0 ; j<v.size() ; j++) {
            if (_table[i][c] == v[j])
               neew=0;
         }
         if (neew)
            v.push_back(_table[i][c]);
         neew=1;
      }
   }
   if (goodnum)
      return v.size();
   else 
      return INT_MAX;                  //return if nothing?

   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
   
}

float
DBTable::getAve(size_t c) const
{
   int goodnum=0;
   float sum=0.0;
   for (size_t i=0 ; i<nRows() ; i++) {
      if (_table[i][c] != INT_MAX) {
         goodnum++;
         sum+=_table[i][c];
      } 
   }
   if (goodnum)
      return sum/goodnum;
   else 
      return NAN;
   // TODO: compute the average of data in column #c
}

void
DBTable::sort(const struct DBSort& s)
{
   for (size_t i=0 ; i<nRows() ; i++) {
      for (size_t j=i ; j<nRows() ; j++) {
         if (s(_table[i],_table[j]))
            swap(_table[j],_table[j]);
      }
   }
   // TODO: sort the data according to the order of columns in 's'
}

void
DBTable::printCol(size_t c) const
{
   for (size_t i=0 ; i<nRows() ; i++) {
      if (_table[i][c]==INT_MAX)
         cout << '.';
      else
         cout << _table[i][c];
      if (i!=nRows()-1)
         cout << ' ';
   }
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

