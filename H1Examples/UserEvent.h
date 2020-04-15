////////////////////////////////////////////////////////////
// Class UserEvent
//
// Author: Ursula Berthon
// Last update: $Date: 2003/10/09 12:19:14 $
//          by: $Author: wessling $
// 
// Description: Defines characteristics of a user defined 
//              event branches...)
//              Used in H1Examples/write_usertree 
//                and              read_usertree
///////////////////////////////////////////////////////////

#ifndef __USEREVENT_H
#define __USEREVENT_H

#include <TROOT.h>
#include <TLorentzVector.h>

#include "H1Skeleton/H1Event.h"

class UserFiller;
class TTree;

class UserEvent: public H1Event {
  friend class UserFiller;
public:
  UserEvent();
  virtual ~UserEvent();
  
  virtual void   Clear();
  virtual void   CreateBranches(TTree* tree);
  virtual void   SetBranchAddresses(TTree* tree);
  
private:
  // variables to be filled in user tree:
  Short_t             fIvtyp;   
  Float_t             fVtxZ;        
  TLorentzVector*     fHighestPtTrackLorV;

  ClassDef(UserEvent,1) // class to contain data for user tree, 
};

#endif
