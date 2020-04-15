// This file was automatically generated by the H1PtrGenerator class.
// ($Id: H1PtrGenerator.C,v 1.16 2005/12/02 11:22:34 katzy Exp $)

#ifndef __TLORENTZVECTORPTR_H
#define __TLORENTZVECTORPTR_H

#ifndef __H1OBJECTPTR_H
#include  "H1Pointers/H1ObjectPtr.h"
#endif

class TLorentzVector;


class TLorentzVectorPtr: public H1ObjectPtr {
  public:
    // con- and destructor
    TLorentzVectorPtr(const char* branchName = NULL);

    // access to pointer
    virtual Bool_t SetPointer(const char* branchName);
    TLorentzVector& operator *() const;
    TLorentzVector* operator ->() const;

    ClassDef(TLorentzVectorPtr,0)  // data access class for TLorentzVector
};

#endif