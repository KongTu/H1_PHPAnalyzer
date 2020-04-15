#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <cmath>
#include <iostream>
#include <TChain.h>
#include <TMath.h>
#include <TRandom.h>
#include <TF1.h>
#include <string>
#include "TLorentzVector.h"

using namespace std;

struct MyEvent {
   // general information
   Int_t run_mini,evno_mini; // run and event number
   Float_t w_mini; // event weight
   Int_t primaryVertexFlags_mini;
   Float_t vertex_mini[3];
   Int_t haveEtag6Trigger_mini;
   Float_t eTag6Energy_mini,photonTagEnergy_mini,eTag6pos_mini[2];
   Float_t W_hfs_mini, W_etag6_mini;
   Int_t eventpass_mini[7];
   Float_t etaMin_mini;
   Float_t xGammaREC_mini[3];
   //needs more study on cuts;
   Int_t nRECtrack_mini;
   Int_t totalMultREC_mini;

   enum {
      nRECtrack_MAX=200
   };
   Int_t trackpass_mini[nRECtrack_MAX];
   Int_t trackFlagsREC_mini[nRECtrack_MAX];
   Float_t trackPtREC_mini[nRECtrack_MAX];
   Float_t trackEtaREC_mini[nRECtrack_MAX];
   Float_t trackPhiREC_mini[nRECtrack_MAX];


};

void fillPHPMiniTree(const int start = 0, int end = -1) {

   //starting TChain;
   TChain* tree = new TChain("properties");
   int dis_events = 0;

   tree->Add("/nfs/dust/h1/group/sschmitt/H1_ForwardMultAnalyzer/batch/data_06/*.root");
   tree->Add("/nfs/dust/h1/group/sschmitt/H1_ForwardMultAnalyzer/batch/data_07/*.root");

   //start to define new miniTree:
   TTree *outtree =new TTree("miniTree","miniTree");
   MyEvent myEvent;

   outtree->Branch("w_mini",&myEvent.w_mini,"w_mini/F");
   outtree->Branch("primaryVertexFlags_mini",&myEvent.primaryVertexFlags_mini,"primaryVertexFlags_mini/I");
   outtree->Branch("vertex_mini",myEvent.vertex_mini,"vertex_mini[3]/F");
   outtree->Branch("haveEtag6Trigger_mini",&myEvent.haveEtag6Trigger_mini,"haveEtag6Trigger_mini/I");
   outtree->Branch("eTag6Energy_mini",&myEvent.eTag6Energy_mini,"eTag6Energy_mini/F");
   outtree->Branch("photonTagEnergy_mini",&myEvent.photonTagEnergy_mini,"photonTagEnergy_mini/F");
   outtree->Branch("eTag6pos_mini",myEvent.eTag6pos_mini,"eTag6pos_mini[2]/F");
   outtree->Branch("W_hfs_mini",&myEvent.W_hfs_mini,"W_hfs_mini/F");
   outtree->Branch("W_etag6_mini",&myEvent.W_etag6_mini,"W_etag6_mini/F");
   outtree->Branch("eventpass_mini",myEvent.eventpass_mini,"eventpass_mini[7]/I");
   outtree->Branch("etaMin_mini",&myEvent.etaMin_mini,"etaMin_mini/F");
   outtree->Branch("xGammaREC_mini",myEvent.xGammaREC_mini,"xGammaREC_mini[3]/F");
   outtree->Branch("nRECtrack_mini",&myEvent.nRECtrack_mini,"nRECtrack_mini/I");
   //tracks
   outtree->Branch("totalMultREC_mini",&myEvent.totalMultREC_mini,"totalMultREC_mini/I");
   outtree->Branch("trackpass_mini",myEvent.trackpass_mini,"trackpass_mini[nRECtrack_mini]/I");
   outtree->Branch("trackFlagsREC_mini",myEvent.trackFlagsREC_mini,"trackFlagsREC_mini[nRECtrack_mini]/I");
   outtree->Branch("trackPtREC_mini",myEvent.trackPtREC_mini,"trackPtREC_mini[nRECtrack_mini]/F");
   outtree->Branch("trackEtaREC_mini",myEvent.trackEtaREC_mini,"trackEtaREC_mini[nRECtrack_mini]/F");
   outtree->Branch("trackPhiREC_mini",myEvent.trackPhiREC_mini,"trackPhiREC_mini[nRECtrack_mini]/F");

   if(tree) {
   //tree branches      
      
      // trigger selection and event weights
      Float_t l4weight;
      Float_t trigWeightRW;
      Int_t haveEtag6Trigger;
      tree->SetBranchAddress("weight",&l4weight);
      tree->SetBranchAddress("trigWeightRW",&trigWeightRW);
      tree->SetBranchAddress("haveEtag6Trigger",&haveEtag6Trigger);
      //  haveEtag6Trigger
      //    0: neither S82 nor S86 have fired
      //    1: S82 has fired but not S86
      //    2: S86 has fired but not S82
      //    3: both S82 and S86 have fired

      // tagged photoproduction: look at electron tagger and photon tagger
      //   eTag6pos[0]: reconstucted x-position in tagger surface
      //     this is correlated with the eTag6Energy
      //   Because of bacground and not fully contained showers one has to ask for
      //    abs(etag6pos[0])<3
      //
      Float_t eTag6Energy,photonTagEnergy,eTag6pos[2];
      tree->SetBranchAddress("eTag6Energy",&eTag6Energy);
      tree->SetBranchAddress("eTag6pos",eTag6pos);
      tree->SetBranchAddress("photonTagEnergy",&photonTagEnergy);

      // event vertex
      //   vertexType>0 : there is a reconstructed vertex
      //   typical vertex cut: abs(vertex[2])<30
      Int_t primaryVertexFlags;
      Float_t vertex[3];
      tree->SetBranchAddress("primaryVertexFlags",&primaryVertexFlags);
      tree->SetBranchAddress("primaryVertex",vertex);

      // beam energies
      //   for nominal operation, the proton energy is 920 GeV
      //   for low-energy running it is 460 GeV
      Float_t eProtonBeam,eElectronBeam;
      tree->SetBranchAddress("eProtonBeam",&eProtonBeam);
      tree->SetBranchAddress("eElectronBeam",&eElectronBeam);

      // total measured final state 4-vector
      //
      Float_t eflowREC[4];
      tree->SetBranchAddress("eflowREC",eflowREC);
      //
      // measurements of most-backwards particles
      //  -> to get hold of egamma and related quantities
      enum {MAX_NBWD=40 };
      Int_t eflowNbwd;
      Float_t eflowBwdEtaREC[MAX_NBWD];
      //Float_t eflowBwdMremnantREC[MAX_NBWD];
      Float_t eflowBwdxgammaREC[MAX_NBWD];
      // number of "most backward" particles analyzed
      // the first particle is a pseudo-particle
      //   calculated from the etagger and the sum of all final state
      //   particles
      //  energy: (Ebeam-Etag)-0.5*(E-pz)_finalState
      //  pz = - energy
      //  further particles of the final-state particle are added,
      // starting with the smallest eta (most backward first)
      //   [0]  -> pseudo-particle
      //   [1]  -> most backward particle
      //   [2]  -> second most backward particle
      //     ...
      tree->SetBranchAddress("eflowNbwd",&eflowNbwd);
      // eta of the backwards particle
      tree->SetBranchAddress("eflowBwdEtaREC",eflowBwdEtaREC);
      // reconstructed xGamma
      // (from sum of backward particles)
      tree->SetBranchAddress("eflowBwdxgammaREC",eflowBwdxgammaREC);

      // pointer to a certain index in the "backward particle" arrays
      // to choose an x_gamma reconstruction method
      Int_t ind_xgamma[3];

      //particles with increasing eta are attributed to the photon remnant
      //  up to an invariant mass of 3 GeV
      tree->SetBranchAddress("eflowImmax3REC",ind_xgamma+0);

      //particles with increasing eta are attributed to the photon remnant
      //  the largest delta Eta between remnant 
      //  and other particles is selected, with the extra condition eta<0
      tree->SetBranchAddress("eflowIdetamaxREC",ind_xgamma+1);

      //all particles with eta<-2 are attributed to the photon remnant
      tree->SetBranchAddress("eflowIminus2REC",ind_xgamma+2);


      // charged particles
      Int_t ntrackREC;
      enum {
         ntrackREC_MAX=200
      };
      Int_t trackFlagsREC[ntrackREC_MAX];
      Float_t trackPxREC[ntrackREC_MAX];
      Float_t trackPyREC[ntrackREC_MAX];
      Float_t trackPzREC[ntrackREC_MAX];
      tree->SetBranchAddress("ntrackREC",&ntrackREC);
      tree->SetBranchAddress("trackFlagsREC",trackFlagsREC);
      tree->SetBranchAddress("trackPxREC",trackPxREC);
      tree->SetBranchAddress("trackPyREC",trackPyREC);
      tree->SetBranchAddress("trackPzREC",trackPzREC);

      //track cuts
      Float_t trackDCAprimeREC[ntrackREC_MAX];
      Float_t trackChi2vtxREC[ntrackREC_MAX];
      Float_t trackChi2nvREC[ntrackREC_MAX];
      Float_t trackDz0REC[ntrackREC_MAX];
      Float_t trackStartRadREC[ntrackREC_MAX];
      Float_t trackEndRadREC[ntrackREC_MAX];
      tree->SetBranchAddress("trackDCAprimeREC",&trackDCAprimeREC);
      tree->SetBranchAddress("trackChi2vtxREC",&trackChi2vtxREC);
      tree->SetBranchAddress("trackChi2nvREC",&trackChi2nvREC);
      tree->SetBranchAddress("trackDz0REC",&trackDz0REC);
      tree->SetBranchAddress("trackStartRadREC",&trackStartRadREC);
      tree->SetBranchAddress("trackEndRadREC",&trackEndRadREC);
      //end tree branch

      cout << "total. number of events = " << tree->GetEntries() << endl;
      if(end == -1 || end >= tree->GetEntries()) {end = tree->GetEntries();}
      cout << "starting events = " << start << endl;
      cout << "ending events = " << end << endl;
      cout << "total events now ~ " << 1.0*(tree->GetEntries()-dis_events)+dis_events << endl;

      int totalEvents = 0;
      for(int i=start;i<end;i++) {
         tree->GetEntry(i);

         //================== event variables used for cuts
         // total 4-vector, including the particle detected as scattered electron
         TLorentzVector hfs(eflowREC[0],eflowREC[1],eflowREC[2],eflowREC[3]);
         // calculate gamma-proton entre-of-mass energy from HFS
         double W_hfs=TMath::Sqrt(2.*(hfs.E()-hfs.Pz())*eProtonBeam);
         // calculate gamma-proton energy from tagged electron
         double W_etag6=TMath::Sqrt(4.*(eElectronBeam-eTag6Energy)*eProtonBeam);

         // etamin of final state particles
         double etaMin=HUGE_VAL;
         if(eflowNbwd>1) etaMin=eflowBwdEtaREC[1];

         bool cutflow[6];
         //================== evaluate cuts for cut-flow studies
         cutflow[0]=(haveEtag6Trigger&1)!=0;
         //cutflow[1]=(TMath::Abs(eTag6pos[0])<3.);
         cutflow[1]=(eTag6pos[0]+0.4*eTag6Energy>0.)&&(TMath::Abs(eTag6pos[0])<3.);
         cutflow[2]=eTag6Energy+photonTagEnergy<20.;
         cutflow[3]= primaryVertexFlags>0;
         cutflow[4]=etaMin <0.;
         cutflow[5]=TMath::Abs(vertex[2])<30.;
         
         // event weight: L4 weight times prescale compensation weight
         double weight=l4weight*trigWeightRW;

         myEvent.w_mini = weight;
         myEvent.haveEtag6Trigger_mini = haveEtag6Trigger;
         myEvent.eTag6Energy_mini = eTag6Energy;
         myEvent.photonTagEnergy_mini = photonTagEnergy;
         myEvent.primaryVertexFlags_mini = primaryVertexFlags;
         myEvent.vertex_mini[0] = vertex[0];
         myEvent.vertex_mini[1] = vertex[1];
         myEvent.vertex_mini[2] = vertex[2];
         myEvent.eTag6pos_mini[0] = eTag6pos[0];
         myEvent.eTag6pos_mini[1] = eTag6pos[1];
         myEvent.W_hfs_mini = W_hfs;
         myEvent.W_etag6_mini = W_etag6;
         for(int icut=0;icut<7;icut++){
            myEvent.eventpass_mini[icut] = 0;
         }
         if( cutflow[0] && cutflow[1] && cutflow[2] && cutflow[3]
         && cutflow[4] && cutflow[5] ) {
            myEvent.eventpass_mini[0] = 1;}
         for(int icut=0;icut<6;icut++){
            if( cutflow[icut] ) myEvent.eventpass_mini[icut+1] = 1;
         }
         myEvent.etaMin_mini = etaMin;
         //3 different method of xgamma reconstruction
         //xGammaREC < 0.5?2:1 where 1 is direct and 2 is resolved, for example
         myEvent.xGammaREC_mini[0]= eflowBwdxgammaREC[ind_xgamma[0]];
         myEvent.xGammaREC_mini[1]= eflowBwdxgammaREC[ind_xgamma[1]];
         myEvent.xGammaREC_mini[2]= eflowBwdxgammaREC[ind_xgamma[2]];

         //total number of tracks
         myEvent.nRECtrack_mini = ntrackREC;

         //loop over reconstructed tracks:
         int Ntracks=0;
         for(int iTrk = 0; iTrk<ntrackREC; iTrk++){
            int type=trackFlagsREC[iTrk];            
            if(!type) continue;
            double pt=TMath::Hypot(trackPxREC[iTrk],trackPyREC[iTrk]);
            if(pt<0.100) continue; // pt>100 MeV
            double p=TMath::Hypot(trackPzREC[iTrk],pt);
            double phi=TMath::ATan2(trackPyREC[iTrk],trackPxREC[iTrk]);
            double eta=0.5*TMath::Log((p+trackPzREC[iTrk])/(p-trackPzREC[iTrk]));           
            TVector3 vect;vect.SetPtEtaPhi(pt,eta,phi);
            double theta = vect.Theta();  
            int trackpass=1;
            if(type==1){//central tracks
               if(TMath::Abs(trackDCAprimeREC[iTrk]*TMath::Sin(theta))>0.2) trackpass=0;
               if( trackStartRadREC[iTrk]>5. ) trackpass=0;
               if((trackEndRadREC[iTrk]-trackStartRadREC[iTrk])<5.) trackpass=0;
            }
            else if(type==2){//combined tracks
               if(p<0.5) trackpass=0;
               if(TMath::Abs(trackDCAprimeREC[iTrk])>5.) trackpass=0;
               if(trackStartRadREC[iTrk]>50.) trackpass=0;
               if(trackChi2vtxREC[iTrk]>50.) trackpass=0;
            }
            else{
               //don't cut on FST tracks for now.
               trackpass=1;
            }
            cout << "type " << type << endl;
            cout << "trackpass " << trackpass << endl;
            cout << "TMath::Abs(trackDCAprimeREC[iTrk]*TMath::Sin(theta)) " << TMath::Abs(trackDCAprimeREC[iTrk]*TMath::Sin(theta)) << endl;
            cout << "trackStartRadREC[iTrk] " << trackStartRadREC[iTrk] << endl;
            cout << "(trackEndRadREC[iTrk]-trackStartRadREC[iTrk]) " << (trackEndRadREC[iTrk]-trackStartRadREC[iTrk]) << endl;
            myEvent.trackpass_mini[iTrk] = trackpass;
            
            if( trackpass==1 ){if(TMath::Abs(eta)<2.0) Ntracks++;} 
            myEvent.trackFlagsREC_mini[iTrk] = trackFlagsREC[iTrk];
            myEvent.trackPtREC_mini[iTrk] = pt;
            myEvent.trackEtaREC_mini[iTrk] = eta;
            myEvent.trackPhiREC_mini[iTrk] = phi;

         }
         myEvent.totalMultREC_mini = Ntracks;
         
         outtree->Fill();

         totalEvents++;
         if( totalEvents%100000 == 0 )cout << "Events ~ " << totalEvents << endl;

      }
      cout << "Number of events processed ~ " << totalEvents << endl;  
   }

   TString outfile_name;
   outfile_name = Form("../miniTree/data_PHP_0607_Tree_%d.root",start);
   TFile outfile( outfile_name, "RECREATE");
   
   outtree->Write();

   
}
