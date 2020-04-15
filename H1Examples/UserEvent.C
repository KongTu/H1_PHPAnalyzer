////////////////////////////////////////////////////////////
// Class UserEvent
//
// Author:      Ursula Berthon
// Last update: $Date: 2002/09/12 10:22:43 $
//          by: $Author: flucke $
// 
// Description: Defines characteristics of a user defined 
//
///////////////////////////////////////////////////////////

#include "H1Examples/UserEvent.h"
#include <H1Mods/H1PartSelTrack.h>

ClassImp(UserEvent)

UserEvent::UserEvent()
{
  fIvtyp = 0;
  fVtxZ = 0;
  fHighestPtTrackLorV = new TLorentzVector;
}

UserEvent::~UserEvent()
{
  delete fHighestPtTrackLorV;
  fHighestPtTrackLorV = 0;
}

void UserEvent::Clear()
{
  // all stuff that may NOT be assigned new values in loop
  // (and lists) has to be reset here:

  fHighestPtTrackLorV->SetXYZT(0,0,0,0);
}

void UserEvent::SetBranchAddresses(TTree* tree)
{
// set branch address for own datamembers

  tree->SetBranchAddress("myIvtyp", &fIvtyp);
  tree->SetBranchAddress("myVtxZ", &fVtxZ);
  tree->SetBranchAddress("HighestPtTrackLorV", &fHighestPtTrackLorV);
}

void UserEvent::CreateBranches(TTree* tree)
{
// create branch with addresses for own datamembers

  tree->Branch("myIvtyp", &fIvtyp, "myIvtyp/S"); // it's a short
  tree->Branch("myVtxZ" , &fVtxZ , "myVtxZ/F");  // it's a float

  // even from the pointer we need the address (&fHighest...):
  tree->Branch("HighestPtTrackLorV", "TLorentzVector", &fHighestPtTrackLorV, 32768, 0);
  //                                                               non split-mode: ^^^    
}

