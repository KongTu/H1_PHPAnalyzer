#ifndef __H__TRACKBRANCHES__
#define __H__TRACKBRANCHES__

#include "TTreeBranches.h"

#include "H1Mods/H1PartCandArrayPtr.h"
#include "H1Mods/H1SelVertexArrayPtr.h"
#include "H1Tracks/H1FSTFittedTrackArrayPtr.h"

class VertexBranches;

class TrackBranches : public TTreeBranches {
 public:
   TrackBranches(VertexBranches const *vtx);
   void fill(void);

   enum {
      MAXTRACK=200,
   };

#define VARIABLES()             \
   INT_VAR(ntrackAllREC); \
   INT_VAR(ntrackREC); \
   HEXINT_ARRAY(trackFlagsREC,MAXTRACK,ntrackREC); \
   HEXINT_ARRAY(trackHitWordREC,MAXTRACK,ntrackREC); \
   INT_ARRAY(trackChargeREC,MAXTRACK,ntrackREC); \
   FLOAT_ARRAY(trackPxREC,MAXTRACK,ntrackREC); \
   FLOAT_ARRAY(trackPyREC,MAXTRACK,ntrackREC); \
   FLOAT_ARRAY(trackPzREC,MAXTRACK,ntrackREC); \
   FLOAT_ARRAY(trackChi2vtxREC,MAXTRACK,ntrackREC); \
   FLOAT_ARRAY(trackChi2nvREC,MAXTRACK,ntrackREC); \
   FLOAT_ARRAY(trackDCAprimeREC,MAXTRACK,ntrackREC); \
   FLOAT_ARRAY(trackDz0REC,MAXTRACK,ntrackREC); \
   FLOAT_ARRAY(trackStartRadREC,MAXTRACK,ntrackREC); \
   FLOAT_ARRAY(trackEndRadREC,MAXTRACK,ntrackREC); \

#include "Ntuple.h"

   static H1PartCandArrayPtr fPartCands;
   static H1SelVertexArrayPtr fVertexPtr; 
   static H1FSTFittedTrackArrayPtr fstFittedTracks;
   VertexBranches const *vertex;
};

#endif
