#ifndef __H__VERTEXBRANCHES__
#define __H__VERTEXBRANCHES__

#include "TTreeBranches.h"

#include "H1Mods/H1SelVertexArrayPtr.h"


class VertexBranches : public TTreeBranches {
 public:
   VertexBranches(void);
   void fill(void);

#define VARIABLES() \
   INT_VAR(primaryVertexFlags);     \
   FLOAT_ARRAY(primaryVertex,3,3); \

#include "Ntuple.h"

 protected:
   static H1SelVertexArrayPtr MODS_vertex; // all good vertices

};

#endif
