/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar getChar(istream&);


//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
   if (_dofile.is_open()) {
      readCmdInt(_dofile);
      _dofile.close();
   }
   else
      readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
   resetBufAndPrintPrompt();

   while (1) {
      ParseChar pch = getChar(istr);
      if (pch == INPUT_END_KEY) break;
      switch (pch) {
         case LINE_BEGIN_KEY :
         case HOME_KEY       : moveBufPtr(_readBuf); break;
         case LINE_END_KEY   :
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY : 
            if (_readBufPtr>_readBuf){
               _readBufPtr--;
               deleteChar();
               break;          
            }
            else {mybeep();  break;}

         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : addHistory();
                               cout << char(NEWLINE_KEY);
                               resetBufAndPrintPrompt(); break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
         case ARROW_RIGHT_KEY: moveBufPtr(_readBufPtr + 1); break;
         case ARROW_LEFT_KEY : moveBufPtr(_readBufPtr - 1); break;
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        : 
         {
            int pos = _readBufPtr - _readBuf;
            if ( pos%8 == 0) {
               for (int i=0 ; i<8 ; i++) 
                  insertChar(' ');
            }
            else {
               for (int i=0 ; i<8-(pos%8) ; i++) 
                   insertChar(' ');
            }
         }
         break;
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY:   mybeep(); break;
         default:  // printable character
            insertChar(char(pch)); break;
      }
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
}

// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
   if ( ptr<=_readBufEnd && ptr>=_readBuf) {
      _readBufPtr = ptr;
      cout << '\r';
      for (int i=0 ; i<_readBufEnd - _readBuf+1+5 ; i++) 
      // +1 to make sure if we do "backspace", the last char will be cleared. 5 for the "cmd> "
         cout << ' ';
      cout << '\r' << "cmd> ";
      for (int i=0 ; i<_readBufEnd- _readBuf ; i++)
         cout << *(_readBuf+i);
      cout << '\r' << "cmd> ";
      for (int i=0 ; i<_readBufPtr - _readBuf ; i++)
      cout << *(_readBuf+i);
      return true;
   }
   else
      return false;
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
   if (_readBufPtr < _readBufEnd) {
      char* tmp = _readBufPtr;
      for (int i=0 ; i<_readBufEnd - _readBufPtr ; i++) {
         *(tmp+i) = *(tmp+1+i);
      }
      *_readBufEnd = '\0';
      _readBufEnd--;
   cout << '\r';
   for (int i=0 ; i<_readBufEnd - _readBuf+1+5 ; i++) 
   // +1 to make sure if we do "backspace", the last char will be cleared. 5 for the "cmd> "
      cout << ' ';
   cout << '\r' << "cmd> ";
   for (int i=0 ; i<_readBufEnd- _readBuf ; i++)
      cout << *(_readBuf+i);
   cout << '\r' << "cmd> ";
   for (int i=0 ; i<_readBufPtr - _readBuf ; i++)
      cout << *(_readBuf+i);
   }
   // TODO...
   return true;
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void
CmdParser::insertChar(char ch, int repeat)
{
   if (_readBufPtr == _readBufEnd) {  //if cursor is at the end
      _readBufEnd++;
      *_readBufPtr = ch;
      _readBufPtr++;
   cout << '\r';
   for (int i=0 ; i<_readBufEnd - _readBuf+1+5 ; i++) 
   // +1 to make sure if we do "backspace", the last char will be cleared. 5 for the "cmd> "
      cout << ' ';
   cout << '\r' << "cmd> ";
   for (int i=0 ; i<_readBufEnd- _readBuf ; i++)
      cout << *(_readBuf+i);
   cout << '\r' << "cmd> ";
   for (int i=0 ; i<_readBufPtr - _readBuf ; i++)
      cout << *(_readBuf+i);
   }
   if (_readBufPtr < _readBufEnd) {
      int tmp = _readBufEnd - _readBufPtr;
      char* _tmpptr = _readBufEnd;
      for (int i=0 ; i<tmp ; i++) {
          *(_tmpptr+1) = *_tmpptr;
          _tmpptr--;
      }
      *_readBufPtr = ch;
      _readBufEnd++;
      _readBufPtr++;
   cout << '\r';
   for (int i=0 ; i<_readBufEnd - _readBuf+1+5 ; i++) 
   // +1 to make sure if we do "backspace", the last char will be cleared. 5 for the "cmd> "
      cout << ' ';
   cout << '\r' << "cmd> ";
   for (int i=0 ; i<_readBufEnd- _readBuf ; i++)
      cout << *(_readBuf+i);
   cout << '\r' << "cmd> ";
   for (int i=0 ; i<_readBufPtr - _readBuf ; i++)
      cout << *(_readBuf+i);
   }
   // TODO...
   assert(repeat >= 1);
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
   if (_readBufEnd>_readBuf) {
      for (int i=0 ; i< _readBufEnd-_readBuf ; i++) {
         *(_readBuf+i) = '\0';
      }
      cout << '\r';
      for (int i=0 ; i<_readBufEnd - _readBuf+1+5 ; i++)
         cout << ' '; 
      _readBufPtr = _readBufEnd = _readBuf;
      *_readBufPtr = 0;
    }
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
   if (_historyIdx==0 && index<0)
      mybeep();
   else if (_historyIdx==int(_history.size()) && _tempCmdStored==0 && index > int(_history.size()) )
      mybeep(); 
   else if (_historyIdx==int(_history.size())-1 && _tempCmdStored==1 && index >= int(_history.size()) )
      mybeep();
   else {
      if (index >= int(_history.size()))
         index = _history.size() - 1;
      if (index < 0)
         index = 0;
      if (_tempCmdStored == 0) {
         string str(_readBuf);
         _history.push_back(str);
         _tempCmdStored = 1;
      }
      if (_tempCmdStored==1 && _historyIdx == int(_history.size()-1) && index < int(_history.size())) {
         _history.pop_back();
         _history.push_back(_readBuf);
      }
      _historyIdx = index;
      retrieveHistory();
   cout << '\r';
   for (int i=0 ; i<_readBufEnd - _readBuf+1+5 ; i++) 
   // +1 to make sure if we do "backspace", the last char will be cleared. 5 for the "cmd> "
      cout << ' ';
   cout << '\r' << "cmd> ";
   for (int i=0 ; i<_readBufEnd- _readBuf ; i++)
      cout << *(_readBuf+i);
   cout << '\r' << "cmd> ";
   for (int i=0 ; i<_readBufPtr - _readBuf ; i++)
      cout << *(_readBuf+i);
   }
   // TODO...
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void
CmdParser::addHistory()
{
   if (_tempCmdStored) {
      _history.pop_back();
      _tempCmdStored = 0;
   }
   if (_readBufPtr > _readBuf) {
      _readBufPtr = _readBuf;
      while (*_readBufPtr == ' ') 
         deleteChar();
      if (_readBufPtr != _readBufEnd) {
         _readBufPtr = _readBufEnd;
         while (*(_readBufEnd-1) == ' ') {
            _readBufPtr--;
            deleteChar();
         }
         string str(_readBuf);
         _history.push_back(str);
         for (int i=0 ; i< _readBufEnd-_readBuf ; i++) {
            *(_readBuf+i) = '\0';
         }
      }
   }
   _historyIdx = _history.size();
      // TODO...
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}
