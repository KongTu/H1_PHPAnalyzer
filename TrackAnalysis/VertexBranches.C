#include "VertexBranches.h"

#include "H1Mods/H1SelVertex.h"

H1SelVertexArrayPtr VertexBranches::MODS_vertex; 

VertexBranches::VertexBranches(void) {
}

void VertexBranches::fill(void) {
   primaryVertexFlags=-1;
   for(int i=0;i<MODS_vertex.GetEntries();i++) {
      Int_t type=MODS_vertex[i]->GetVertexType();
      if(type==H1SelVertex::vtPrimary) {
         primaryVertexFlags=0;
         TVector3 pv=MODS_vertex[i]->GetPosition();
         primaryVertex[0]=pv.X();
         primaryVertex[1]=pv.Y();
         primaryVertex[2]=pv.Z();
      } else if(type==H1SelVertex::vtCJC) {
         primaryVertexFlags |=1;
      } else if(type==H1SelVertex::vtFTD) {
         primaryVertexFlags |=2;
      } else if(type==H1SelVertex::vtBST) {
         primaryVertexFlags |=4;
      } else if(type==H1SelVertex::vtFST) {
         primaryVertexFlags |=8;
      }
   }
}

