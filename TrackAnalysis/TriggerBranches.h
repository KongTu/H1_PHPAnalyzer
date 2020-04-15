#ifndef __H__TRIGGERBRANCHES__
#define __H__TRIGGERBRANCHES__

#include "TTreeBranches.h"

#include "H1Pointers/H1IntPtr.h"
#include "H1Pointers/H1BytePtr.h"
#include "H1Pointers/H1ShortPtr.h"
#include "H1Pointers/H1FloatPtr.h"

class TriggerBranches : public TTreeBranches {
 public:
   TriggerBranches(void);
   void fill(int counter,int FSTHVstatus);

#define VARIABLES() \
   INT_VAR(run);     \
   INT_VAR(evno);    \
   INT_VAR(runType);          \
   FLOAT_ARRAY(beamspot,2,2); \
   FLOAT_ARRAY(beamtilt,2,2); \
   INT_VAR(L4dec);            \
   INT_VAR(L4mode);           \
   FLOAT_VAR(weight);         \
   FLOAT_VAR(weight1);        \
   FLOAT_VAR(weight2);        \
   FLOAT_VAR(trigWeightRW);   \
   FLOAT_VAR(trigWeightAC);   \
   HEXINT_ARRAY(l1l2l3rw,4,4);   \
   HEXINT_ARRAY(l1l2l3ac,4,4);    \
   INT_VAR(haveSpaCalTrigger); \
   INT_VAR(haveEtag6Trigger);  \
   INT_VAR(FSTHV);             \
   INT_VAR(ibg);             \
   INT_VAR(ibgfm);            \
   INT_VAR(ibgam);            \
   INT_VAR(iqn);             \
   INT_VAR(bunch);             \
   INT_VAR(bunchType);         \
   FLOAT_VAR(electronCurrent); \
   FLOAT_VAR(protonCurrent);        \
   FLOAT_VAR(electronPilotCurrent); \
   FLOAT_VAR(protonPilotCurrent);   \

#include "Ntuple.h"
 protected:
   H1IntPtr HAT_run;
   H1IntPtr HAT_evno;
   H1ShortPtr HAT_RunType;
   H1FloatPtr HAT_BeamX0;
   H1FloatPtr HAT_BeamY0;
   H1FloatPtr HAT_beamtiltx0;
   H1FloatPtr HAT_beamtilty0;
   H1IntPtr HAT_L4dec;
   H1FloatPtr HAT_weight1;
   H1FloatPtr HAT_weight2;
   H1BytePtr HAT_l1l2l3ac;
   H1BytePtr HAT_l1l2rw;
   H1BytePtr HAT_l1l3rw;
   H1IntPtr HAT_ibg;
   H1IntPtr  HAT_ibgfm;
   H1IntPtr  HAT_ibgam;
   H1IntPtr  HAT_iqn;
   H1ShortPtr HAT_Bunch;

};

#endif
