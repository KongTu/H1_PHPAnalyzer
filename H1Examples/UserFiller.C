/////////////////////////////////////////////////////////////////
// Class UserFiller
//
// Author: Ursula Berthon
// Last update: $Date: 2002/08/09 11:07:09 $
//          by: $Author: wessling $
// 
// Description: Defines properties of user defined event filling
//               
////////////////////////////////////////////////////////////////
#include <iostream> 
// Root header
#include <TLorentzVector.h>
// H1Example headers
#include "H1Examples/UserFiller.h"
#include "H1Examples/UserEvent.h"
// OO headers
#include "H1Skeleton/H1Tree.h"
#include "H1Pointers/H1ShortPtr.h"
#include "H1Pointers/H1FloatPtr.h"
#include "H1Mods/H1PartSelTrack.h"
#include "H1Mods/H1PartSelTrackArrayPtr.h"

ClassImp(UserFiller)

void UserFiller::Fill(H1Event *event) 
{
// here we suppose that a H1Tree with HAT is open for reading 
// and that we are looping over it with gH1Tree.Next()

  // the event is a UserEvent
  UserEvent* userEvent = static_cast<UserEvent*> (event);

  // create branch pointers (static => only once!)
  static  H1ShortPtr hatIvTyp("Ivtyp");
  static  H1FloatPtr hatVtxZ("VtxZ");
  static  H1PartSelTrackArrayPtr tracks;

  // Fill event structure
  userEvent->fIvtyp = *hatIvTyp;
  userEvent->fVtxZ  = *hatVtxZ;

  // search for H1PartSelTrack with highest pt:
  Double_t highestPt = 0;
  for(Int_t i = 0; i < tracks.GetEntries(); ++i){
    Double_t pt = tracks[i]->GetPt();
    if(pt > highestPt){
      highestPt = pt;
      // create new FourVector at address of 
      //                       userEvent->fHighestPtTrackLorV
      new(userEvent->fHighestPtTrackLorV) 
	TLorentzVector((tracks[i])->GetFourVector()); 
    }
  }
}
