 /****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;

   if (options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");

   bool isarr = false;
   int temp , numobj = -1, arrsz = -1;
   if (options.size() > 3)
      return CmdExec::errorOption(CMD_OPT_EXTRA,options[3]);
   for (size_t i = 0, n = options.size(); i < n; ++i) {
      if (myStr2Int(options[i],temp)) {
         if (isarr) {
            if (myStrNCmp("-Array", options[i-1], 2) == 0) 
               arrsz = temp;
            else
               numobj = temp;
         }
         else if (numobj < 0)
            numobj = temp;
         else
            return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
      }

      else {
         if (myStrNCmp("-Array", options[i], 2) == 0) {
            if (isarr) 
               return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
            isarr = true;
         }
         else 
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
      }           
   }
   if (numobj < 0) 
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   try {
      if (numobj > 0) {
         if (arrsz > 0) 
            mtest.newArrs(numobj,arrsz);
         else
            mtest.newObjs(numobj);
      }
   }
   catch (std::bad_alloc& ba)  {
   }
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;

   if (options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   bool isidx=0, israndom=0 , isarr=0;
   int temp, Idx = -1 ,Randoms = -1;
   if (options.size() > 3)
      return CmdExec::errorOption(CMD_OPT_EXTRA,options[3]);
   for (size_t i = 0, n = options.size(); i < n; ++i) {
      if (myStr2Int(options[i],temp)) {
         if (isidx||israndom) {
            if (myStrNCmp("-Index", options[i-1], 2) == 0) 
               Idx = temp;
            if (myStrNCmp("-Random", options[i-1], 2) == 0)
               Randoms = temp;
         }
         else
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
      }
      else {
         if (myStrNCmp("-Index", options[i], 2) == 0) {
            if (isidx||israndom) 
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
            isidx = true;
         }
         else if (myStrNCmp("-Random", options[i], 2) == 0) {
            if (isidx||israndom) 
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
            israndom = true;
         }
         else if (myStrNCmp("-Array", options[i], 2) == 0) {
            if (isarr) 
               return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
            isarr = true;
         }
         else 
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
      }           
   }
   if (isidx && Idx < 0)
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[Idx]);
   if (Idx >= (int)mtest.getObjListSize() && (!isarr)) {
      cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << Idx << "!!" << endl;
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[Idx]);     
   }
   if (Idx >= (int)mtest.getArrListSize() && isarr ) {
      cerr << "Size of object list (" << mtest.getArrListSize() << ") is <= " << Idx << "!!" << endl;
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[Idx]);     
   }
   if (isidx) {
      if (!isarr) 
         mtest.deleteObj(Idx);
      else 
         mtest.deleteArr(Idx);
   }
   if (israndom) {
      srand(rnGen(0));
      if (!isarr) {
         for (int i=0 ; i<Randoms ; i++) 
            mtest.deleteObj(rand()%mtest.getObjListSize());
      }
      else
         for (int i=0 ; i<Randoms ; i++) 
            mtest.deleteArr(rand()%mtest.getArrListSize());
   }
   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


