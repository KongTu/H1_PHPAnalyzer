#include "EnergyFlowBranches.h"
#include "H1Mods/H1PartCand.h"

using namespace std;

H1PartCandArrayPtr EnergyFlowBranches::fPartCands;

EnergyFlowBranches::EnergyFlowBranches(void) {
   HAT_eBeamP=H1FloatPtr("EBeamP"); // proton beam energy from DMIS
   HAT_eBeamE=H1FloatPtr("EBeamE"); // electron beam energy from DMIS
   HAT_Phtag=H1FloatPtr("Phtag");;
   HAT_Etag6=H1FloatPtr("Etag6");
   HAT_Xtag6=H1FloatPtr("Xtag6");
   HAT_Ytag6=H1FloatPtr("Ytag6");
}

void EnergyFlowBranches::fill(void) {
   // beam energies
   eProtonBeam=*HAT_eBeamP;
   eElectronBeam=*HAT_eBeamE;

   // electron and photon tagger
   photonTagEnergy=*HAT_Phtag;
   eTag6Energy=*HAT_Etag6;
   eTag6pos[0]=*HAT_Xtag6;
   eTag6pos[1]=*HAT_Ytag6;


   // calroimeters and tracks (H1PartCand)
   for(int i=0;i<4;i++) {
      eflowREC[i]=0.;
      eflowMuonREC[i]=0.;
      eflowTrackREC[i]=0.;
      eflowLArREC[i]=0.;
      eflowHspacalREC[i]=0.;
      eflowEMspacalREC[i]=0.;
   }
   eflowETAMAX400REC=-HUGE_VAL;
   eflowETAMAX800REC=-HUGE_VAL;
   eflowETAMIN400REC=HUGE_VAL;
   eflowETAMIN800REC=HUGE_VAL;
   map<double,multimap<double,H1PartCand *> > idByEtaPt;
   // loop over all particle candidates
   for(int ipart=0;ipart<fPartCands.GetEntries();ipart++) {
      H1PartCand *cand=fPartCands[ipart];
      if(!(cand->IsHFS() ||
           ((cand->IsIsolatedLepton()||cand->IsScatElec())&&
            cand->GetP()>0.))) {
         // keep particles counted as HFS
         // keep leptons if they have a measured momentum
         continue;
      }
      TLorentzVector p=cand->GetFourVector();
      // total energy flow 4-vector
      eflowREC[0]+=p.X();
      eflowREC[1]+=p.Y();
      eflowREC[2]+=p.Z();
      eflowREC[3]+=p.E();
      // etamin, etamax
      double pt=p.Pt();
      if(pt>0.) {
         double eta=p.Eta();
         if(p.E()>0.4) {
            if(eta>eflowETAMAX400REC) eflowETAMAX400REC=eta;
            if(eta<eflowETAMIN400REC) eflowETAMIN400REC=eta;
         }
         if(p.E()>0.8) {
            if(eta>eflowETAMAX800REC) eflowETAMAX800REC=eta;
            if(eta<eflowETAMIN800REC) eflowETAMIN800REC=eta;
         }
         idByEtaPt[eta].insert(make_pair(pt,cand));
      }
      // Energy flow of various detectors
      if(cand->IsSelTrack()) {
         // possibly includes identified muons or EM parts
         eflowTrackREC[0]+=p.X();
         eflowTrackREC[1]+=p.Y();
         eflowTrackREC[2]+=p.Z();
         eflowTrackREC[3]+=p.E();
      } else if(cand->IsMuon()) {
         // muon without track
         eflowMuonREC[0]+=p.X();
         eflowMuonREC[1]+=p.Y();
         eflowMuonREC[2]+=p.Z();
         eflowMuonREC[3]+=p.E();
      } else if(cand->IsCluster()>0.) {
         //TObjArray const *clusters=cand->GetClusters();
         if(cand->GetLArFraction()>0.) {
            eflowLArREC[0]+=p.X();
            eflowLArREC[1]+=p.Y();
            eflowLArREC[2]+=p.Z();
            eflowLArREC[3]+=p.E();
         } else if(cand->GetEMFraction()==1.) {
            eflowEMspacalREC[0]+=p.X();
            eflowEMspacalREC[1]+=p.Y();
            eflowEMspacalREC[2]+=p.Z();
            eflowEMspacalREC[3]+=p.E();
         } else {
            // hadronic spacal
            eflowHspacalREC[0]+=p.X();
            eflowHspacalREC[1]+=p.Y();
            eflowHspacalREC[2]+=p.Z();
            eflowHspacalREC[3]+=p.E();
         }
      }
   }
   eflowNbwd=0;
   double egamma=eElectronBeam-eTag6Energy;
   double eRemnant=egamma-0.5*(eflowREC[3]-eflowREC[2]);
   if(eRemnant<0.0) eRemnant=0.0;
   TLorentzVector remnant(0.,0.,-eRemnant,eRemnant);
   double scaleRemnantXgamma=2.*(egamma-eRemnant)/(eflowREC[3]-eflowREC[2]);
   eflowBwdPxREC[eflowNbwd]=0.;
   eflowBwdPyREC[eflowNbwd]=0.;
   eflowBwdPzREC[eflowNbwd]=remnant.Pz();
   eflowBwdEREC[eflowNbwd]=remnant.E();
   eflowBwdEtaREC[eflowNbwd]=-HUGE_VAL;
   eflowBwdMremnantREC[eflowNbwd]=0.;
   eflowBwdxgammaREC[eflowNbwd]=1.;
   eflowNbwd++;
   for(map<double,multimap<double,H1PartCand *> >::const_iterator
          iEta=idByEtaPt.begin();
       (iEta!=idByEtaPt.end())&&(eflowNbwd<MAXBWD);iEta++) {
      for(multimap<double,H1PartCand *>::const_iterator iPt=
             (*iEta).second.begin();(iPt!=(*iEta).second.end())&&
             (eflowNbwd<MAXBWD);iPt++) {
         H1PartCand const *cand=(*iPt).second;
         remnant +=cand->GetFourVector();

         eflowBwdPxREC[eflowNbwd]=cand->GetPx();
         eflowBwdPyREC[eflowNbwd]=cand->GetPy();
         eflowBwdPzREC[eflowNbwd]=cand->GetPz();
         eflowBwdEREC[eflowNbwd]=cand->GetE();
         eflowBwdEtaREC[eflowNbwd]=(*iEta).first;
         eflowBwdMremnantREC[eflowNbwd]=remnant.M();
         if(egamma!=0.0) {
            eflowBwdxgammaREC[eflowNbwd]=
               1.-scaleRemnantXgamma*(remnant.E()-remnant.Pz())/(2.*egamma);
         } else {
            eflowBwdxgammaREC[eflowNbwd]= -999.0;
         }
         eflowNbwd++;
      }
   }
   // test three methods to reconstruct x_gamma from particles ordered in eta
   eflowIdetamaxREC=0;
   eflowIminus2REC=0;
   eflowImmax3REC=0;
   double detaMax=0.;
   for(int k=1;k<eflowNbwd-1;k++) {
      if(eflowBwdEtaREC[k]>0.) break;
      double deta=eflowBwdEtaREC[k+1]-eflowBwdEtaREC[k];
      if(deta>detaMax) {
         detaMax=deta;
         // select maximum rapidity gap between particles 
         // (and eta<0)
         eflowIdetamaxREC=k;
      }
      if(eflowBwdEtaREC[k]<-1.6) {
         // fixed rapidity separation of photon remnant at eta=-2
         eflowIminus2REC=k;
      }
      if(eflowBwdMremnantREC[k]<3.) {
         // maximum invariant mass of remnant 3 GeV
         eflowImmax3REC=k;
      }
   }
}

