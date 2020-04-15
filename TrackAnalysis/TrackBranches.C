#include <vector>
#include <TMatrixD.h>

#include "TrackBranches.h"
#include "VertexBranches.h"
#include "H1Mods/H1PartCand.h"
#include "H1Mods/H1PartSelTrack.h"
#include "H1Mods/H1SelVertex.h"
#include "H1Tracks/H1VertexFittedTrack.h"
#include "H1Tracks/H1FSTFittedTrack.h"

using namespace std;

H1PartCandArrayPtr TrackBranches::fPartCands;
H1SelVertexArrayPtr TrackBranches::fVertexPtr; 
H1FSTFittedTrackArrayPtr TrackBranches::fstFittedTracks;

static bool floatEqual(double a,double b) {
   double d1=fabs(a-b);
   double d2=fabs(a+b);
   return (d1<=1.E-5*d2);
}


TrackBranches::TrackBranches(VertexBranches const *vtx) {
   vertex=vtx;
}

void TrackBranches::fill(void) {
   ntrackAllREC=0;
   ntrackREC=0;
   // loop over all particle candidates and all FTS tracks
   // keep tracks which are linked to the primary vertex
   int nPartCand=fPartCands.GetEntries();
   int nPart=nPartCand+fstFittedTracks->GetEntries();
   vector<H1VertexFittedTrack const *> allH1Tracks;
   allH1Tracks.reserve(nPart);
   int nhitWord=0;
   double chi2nv=0.,chi2vtx=0.;
   for(int ipart=0;ipart<nPart;ipart++) {
      H1VertexFittedTrack const *h1track=0;
      int flags=0;
      if(ipart<nPartCand) {
         H1PartCand const *cand=fPartCands[ipart];
         H1PartSelTrack const *selTrack=cand->GetIDTrack();
         if(!selTrack) continue;
         if(selTrack->IsCentralTrk()) {
            flags=0x0001;
         } else if(selTrack->IsCombinedTrk()) {
            flags=0x0002;
         } else if(selTrack->IsForwardTrk()) {
            flags=0x0003;
         }
         nhitWord=selTrack->GetNHitWord();
         
         // require that the track is from the primary vertex
         if(selTrack->IsFromPrimary()) {
            h1track=dynamic_cast<H1VertexFittedTrack const *>
               (cand->GetTrack());
            chi2vtx=h1track->GetFitChi2();
            chi2nv=h1track->GetChi2();
         }
      } else {
         flags=0x0004;
         H1FSTFittedTrack const *fstFittedTrack=fstFittedTracks[ipart-nPartCand];
         nhitWord=fstFittedTrack->GetFitNdf() & 0xffff;
         H1FSTTrack const *fstNvTrack=fstFittedTrack->GetFSTTrack();
         chi2vtx=fstFittedTrack->GetFitChi2XY()+fstFittedTrack->GetFitChi2SZ();
         chi2nv=fstNvTrack->GetChi2XY()+fstNvTrack->GetChi2SZ();
         nhitWord |= (fstNvTrack->GetNdfXY()<<16)
            | (fstNvTrack->GetNdfSZ()<<24);
         H1Vertex const *v=fstFittedTrack->GetVertex();
         if(floatEqual(v->X(),vertex->primaryVertex[0])&&
            floatEqual(v->Y(),vertex->primaryVertex[1])&&
            floatEqual(v->Z(),vertex->primaryVertex[2])) {
            h1track=fstFittedTrack;
            TVector3 p1=fstFittedTrack->GetMomentum();
            TMatrix V1=fstFittedTrack->GetMomentumCovar();
            for(int k=0;k<ntrackREC;k++) {
               int flags=trackFlagsREC[k] & 0x000f;
               if((flags>0)&&(flags<3)) {
                  TVector3 p2(allH1Tracks[k]->GetMomentum());
                  TMatrix V2=allH1Tracks[k]->GetMomentumCovar();
                  TMatrixD sum(V1+V2);
                  TMatrixD Vinv(TMatrixD::kInverted,V1+V2);
                  TVector3 d(p1-p2);
                  double chi2=d.Dot(Vinv*d);
                  if(chi2<30.) {
                     // reject FST track as duplicate
                     h1track=0;
                     break;
                  }
               }
            }
         }
      }
      if(!h1track)  continue;

      ntrackAllREC++;
      if(ntrackREC<MAXTRACK) {
         allH1Tracks.push_back(h1track);
         trackFlagsREC[ntrackREC]=flags;
         TVector3 p=h1track->GetMomentum();
         trackChargeREC[ntrackREC]=h1track->GetCharge();
         trackPxREC[ntrackREC]=p.X();
         trackPyREC[ntrackREC]=p.Y();
         trackPzREC[ntrackREC]=p.Z();
         trackHitWordREC[ntrackREC]=nhitWord;
         trackChi2vtxREC[ntrackREC]=chi2vtx;
         trackChi2vtxREC[ntrackREC]=chi2vtx;
         trackChi2nvREC[ntrackREC]=chi2nv;
         trackDCAprimeREC[ntrackREC]=h1track->GetDcaPrime();
         trackDz0REC[ntrackREC]=h1track->GetDz0Prime();
         trackStartRadREC[ntrackREC]=h1track->GetStartRadius();
         trackEndRadREC[ntrackREC]=h1track->GetEndRadius();
         ntrackREC++;
      }
   }
}

