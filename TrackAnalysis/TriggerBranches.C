#include <set>

#include "TriggerBranches.h"
#include "H1Geom/H1TrigInfo.h"
#include "H1Geom/H1DBManager.h"
#include "H1Geom/H1BeamBunch.h"

using namespace std;

TriggerBranches::TriggerBranches(void) {
   HAT_RunType=H1ShortPtr("RunType"); // 0=data, 1=MC, 2=CERN test, 3=CERN MC test
   HAT_BeamX0=H1FloatPtr("BeamX0");          // x position of beam spot (at z=0)
   HAT_BeamY0=H1FloatPtr("BeamY0");          // y position of beam spot (at z=0)
   HAT_beamtiltx0=H1FloatPtr("BeamTiltX0");      // x slope of beam axis
   HAT_beamtilty0=H1FloatPtr("BeamTiltY0");      // y slope of beam axis
   HAT_run=H1IntPtr("RunNumber");
   HAT_evno=H1IntPtr("EventNumber");
   HAT_L4dec=H1IntPtr("L4dec"); // L4 decision word
   HAT_ibg=H1IntPtr("Ibg");  // standard array of background finders from OM group (bit packed)
   HAT_Bunch=H1ShortPtr("Bunch");
   HAT_weight1=H1FloatPtr("Weight1");
   HAT_weight2=H1FloatPtr("Weight2");
   HAT_l1l2l3ac=H1BytePtr("Il1l2l3ac");
   HAT_l1l2rw  =H1BytePtr("Il1l2rw");
   HAT_l1l3rw  =H1BytePtr("Il1l3rw");
   HAT_ibgfm=H1IntPtr("Ibgfm");  // extra finders from OM  (bit packed)
   HAT_ibgam=H1IntPtr("Ibgam");  // background finders from DUK group (bit packed)
   HAT_iqn  =H1IntPtr("Iqn");           // The Lar coherent noise flag
}

void TriggerBranches::fill(int counter,int FSTHVstatus) {
   run=*HAT_run;
   evno=*HAT_evno;
   runType=*HAT_RunType;
   beamspot[0]=*HAT_BeamX0;
   beamspot[1]=*HAT_BeamY0;
   beamtilt[0]=*HAT_beamtiltx0;
   beamtilt[1]=*HAT_beamtilty0;
   L4dec=*HAT_L4dec;
   // L4 weight analysis
   weight1=*HAT_weight1;
   weight2=*HAT_weight2;
   weight=weight1 * weight2;
   H1TrigInfo *trigInfo=dynamic_cast<H1TrigInfo *>
      (H1DBManager::Instance()->GetDBEntry(H1TrigInfo::Class()));
   L4mode=trigInfo->GetL4Mode();
   if((L4mode!=2)&&(runType==0)) {
      // if this is data and L4 was not filtering, set weight to 1
      weight=1.0;
   }

   // L1,L2,L3 weight analysis
   // trigger information
   // prob_rw is the probability that none of the triggers has fired
   double prob_rw=1.0,prob_ac=1.0;
   // save all prescales etc for this run

   set<int> allowedSubtrigger;
   // define list of triggers for prescale weight calculations
   // in the HAT selection, ensure that all those subtriggers
   // are preselected
   allowedSubtrigger.insert(0);
   allowedSubtrigger.insert(1);
   allowedSubtrigger.insert(2);
   allowedSubtrigger.insert(3);
   allowedSubtrigger.insert(61);

   // various triggers based on a coincidence of ETAG6 and something
   allowedSubtrigger.insert(82); // at least one hard track, heavy prescale
   allowedSubtrigger.insert(86); // two hard tracks and LAr
   allowedSubtrigger.insert(89); // track and sihnal in fwd neutron counter
   allowedSubtrigger.insert(113); // track and proton in FPS
   allowedSubtrigger.insert(116); // track and proton in VFPS

   // LAr calorimeter triggers
   // (could be useful if a signal in ETAG6 is asked in the selection)
   allowedSubtrigger.insert(74); // tracks and soft LAR calorimeter condition
   allowedSubtrigger.insert(67); // LAr electron trigger without track
   allowedSubtrigger.insert(75); // LAr electron trigger with track
   allowedSubtrigger.insert(76); // LAr calorimeter high ET
   allowedSubtrigger.insert(77); // LAr calorimeter missing ET

   const Int_t *prescales=trigInfo->GetPrescales();
   const Int_t *enabled=trigInfo->GetEnabledSubTriggers();
   for(int i=0;i<4;i++) {
      l1l2l3ac[i]=0;
      l1l2l3rw[i]=0;
      for(int j=0;j<32;j++) {
         int st=i*32+j;
         if(!enabled[st]) continue;
         if(HAT_l1l2l3ac[st]) {
            l1l2l3ac[i]|=(1<<j);
               
            if(allowedSubtrigger.find(st)!=allowedSubtrigger.end()) {
               prob_ac *= (1.-1./prescales[st]);
            }
         }
         if(HAT_l1l2rw[st] && HAT_l1l3rw[st]) {
            l1l2l3rw[i]|=(1<<j);
            if(allowedSubtrigger.find(st)!=allowedSubtrigger.end()) {
               prob_rw *= (1.-1./prescales[st]);
            }
         }
      }
   }
   // trigWeightRW corrects for prescales using raw trigger selection
   trigWeightRW=(prob_rw<1.0) ? (1./(1.-prob_rw)) : 0.0;
   // trigWeightAC corrects for prescales using actual trigger selection
   trigWeightAC=(prob_ac<1.0) ? (1./(1.-prob_ac)) : 0.0;
   haveSpaCalTrigger=
      (( l1l2l3rw[0] & (1<<1))!=0) &&
      (trigWeightAC>0.); // Spacal trigger S1

   haveEtag6Trigger=0;
   if(trigWeightAC>0.) {
      if(l1l2l3rw[2] & (1<<18)) haveEtag6Trigger+=1; // S82
      if(l1l2l3rw[2] & (1<<22)) haveEtag6Trigger+=2; // S86
   }

   // FST high voltage status
   FSTHV=FSTHVstatus;


   // background and noise flags
   ibg=*HAT_ibg;
   ibgfm=*HAT_ibgfm;
   ibgam=*HAT_ibgam;
   iqn=*HAT_iqn;

   // bunch current analysis
   bunch=*HAT_Bunch;
   H1BeamBunch* beamBunch = H1BeamBunch::Instance();
   bunchType=beamBunch->BunchType(*HAT_Bunch);
   electronCurrent=beamBunch->GetElectronCurrent();
   protonCurrent=beamBunch->GetProtonCurrent();
   electronPilotCurrent=beamBunch->GetElectronPilotCurrent();
   protonPilotCurrent=beamBunch->GetProtonPilotCurrent();
}

