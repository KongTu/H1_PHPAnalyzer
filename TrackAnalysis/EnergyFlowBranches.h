#ifndef __H__ENERGYFLOWBRANCHES__
#define __H__ENERGYFLOWBRANCHES__

#include "TTreeBranches.h"

#include "H1Mods/H1PartCandArrayPtr.h"
#include "H1Pointers/H1FloatPtr.h"


class EnergyFlowBranches : public TTreeBranches {
 public:
   EnergyFlowBranches(void);
   void fill(void);

   enum {
      MAXBWD=40
   };

#define VARIABLES()             \
   FLOAT_VAR(eProtonBeam);    \
   FLOAT_VAR(eElectronBeam);  \
   FLOAT_VAR(photonTagEnergy); \
   FLOAT_ARRAY(eTag6pos,2,2);  \
   FLOAT_VAR(eTag6Energy);     \
   FLOAT_ARRAY(eflowREC,4,4);     \
   FLOAT_VAR(eflowETAMAX400REC);  \
   FLOAT_VAR(eflowETAMAX800REC);  \
   FLOAT_VAR(eflowETAMIN400REC);  \
   FLOAT_VAR(eflowETAMIN800REC);  \
   FLOAT_ARRAY(eflowTrackREC,4,4);  \
   FLOAT_ARRAY(eflowMuonREC,4,4);  \
   FLOAT_ARRAY(eflowLArREC,4,4);  \
   FLOAT_ARRAY(eflowHspacalREC,4,4);  \
   FLOAT_ARRAY(eflowEMspacalREC,4,4);  \
   FLOAT_VAR(eflowWgammapREC); \
   INT_VAR(eflowNbwd); \
   FLOAT_ARRAY(eflowBwdPxREC,MAXBWD,eflowNbwd);  \
   FLOAT_ARRAY(eflowBwdPyREC,MAXBWD,eflowNbwd);  \
   FLOAT_ARRAY(eflowBwdPzREC,MAXBWD,eflowNbwd);  \
   FLOAT_ARRAY(eflowBwdEREC,MAXBWD,eflowNbwd);  \
   FLOAT_ARRAY(eflowBwdEtaREC,MAXBWD,eflowNbwd);  \
   FLOAT_ARRAY(eflowBwdMremnantREC,MAXBWD,eflowNbwd);  \
   FLOAT_ARRAY(eflowBwdxgammaREC,MAXBWD,eflowNbwd);  \
   INT_VAR(eflowIdetamaxREC);              \
   INT_VAR(eflowIminus2REC);              \
   INT_VAR(eflowImmax3REC);              \

#include "Ntuple.h"
 protected:
   static H1PartCandArrayPtr fPartCands;
   H1FloatPtr HAT_eBeamP;
   H1FloatPtr HAT_eBeamE;
   H1FloatPtr HAT_Phtag;
   H1FloatPtr HAT_Etag6;
   H1FloatPtr HAT_Xtag6;
   H1FloatPtr HAT_Ytag6;
};

#endif
