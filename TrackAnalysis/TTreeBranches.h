#ifndef __H__TTREEBRANCHES__
#define __H__TTREEBRANCHES__

#include <TTree.h>
#include <iostream>
#include <iomanip>

class TTreeBranches {
   // base class to add a chunk of variables to a TTree
   // the idea is that the definition of the variable's name and size
   // is done in one place, using a macro
   // the macro is resolved after including the header file 
   //  "Ntuple.h"
 public:
   virtual void setBranchAddresses(TTree *tree) =0;
   virtual void createBranches(TTree *tree) =0;
   virtual void print(void) const=0;
   virtual ~TTreeBranches() { }
};

/* example code

class Event : public TreeBranches {

#define VARIABLES \
    INT_VAR(run) \
    FLOAT_VAR(ptmiss) \
    FLOAT_ARRAY(momentum,3,3) \
    INT_VAR(ntrack) \
    INT_ARRAY(trackType,100,ntrack) \
    FLOAT_ARRAY(trackPx,100,ntrack) \
    FLOAT_ARRAY(trackPy,100,ntrack) \
    FLOAT_ARRAY(trackPz,100,ntrack) \

#include "Ntuple.h"

};


This sets up a class like this:

class Event : public  NTupleTTree {
public:
  Int_t run;
  Float_t ptmiss;
  Float_t momentum[3];
  Int_t ntrack;
  Int_t trackType[100];
  Float_t trackPx[100];
  Float_t trackPy[100];
  Float_t trackPz[100];
  virtual void attach_for_writing(TTree & tree) const {
    tree.Branch(run,&run,"run/I");
    tree.Branch(ptmiss,&ptmiss,"ptmiss/F");
    tree.Branch(ntrack,&ntrack,"ntrack/I");
    tree.Branch(trackType,trackType,"trackType[ntrack]/I");
    tree.Branch(trackPx,trackPx,"trackPx[ntrack]/F");
    tree.Branch(trackPx,trackPy,"trackPy[ntrack]/F");
    tree.Branch(trackPx,trackPz,"trackPz[ntrack]/F");
  }
  // and a similar method attach_for_reading() activate_branches()

};
*/

#endif
