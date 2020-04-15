///////////////////////////////////////////////////////
//
// Select charged particles in tagged photoproduction 
//
///////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <set>

// ROOT includes
#include <TFile.h>
#include <TROOT.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TMatrixD.h>
#include <TMatrixDSym.h>
#include <TMatrixDSymEigen.h>

// for testing
#include "H1Skeleton/H1EventFiller.h"

// H1 OO includes
#include "H1Skeleton/H1Tree.h"
#include "H1Steering/H1StdCmdLine.h"
#include "H1Arrays/H1ArrayF.h"
#include "H1Pointers/H1IntPtr.h"
#include "H1Pointers/H1BytePtr.h"
#include "H1Pointers/H1ShortPtr.h"
#include "H1Pointers/H1FloatPtr.h"
#include "H1Mods/H1PartMCArrayPtr.h"
#include "H1Mods/H1PartMC.h"
#include "H1Mods/H1GetPartMCId.h"
#include "H1Mods/H1SelVertexArrayPtr.h"
#include "H1Mods/H1SelVertex.h"
#include "H1Mods/H1PartCandArrayPtr.h"
#include "H1Mods/H1PartCand.h"
#include "H1Mods/H1PartEm.h"
#include "H1Geom/H1BeamBunch.h"
#include "H1Mods/H1PartSelTrack.h"
#include "H1Mods/H1InclHfsIterator.h"
#include "H1PhysUtils/H1NuclIACor.h"
#include "H1Mods/H1PartEmArrayPtr.h"

#include "H1Geom/H1DetectorStatus.h"
#include "H1Geom/H1DBManager.h"

#include "H1Tracks/H1FSTFittedTrack.h"
#include "H1Tracks/H1FSTFittedTrackArrayPtr.h"

#include "H1Tracks/H1CombinedFittedTrack.h"
#include "H1Tracks/H1CombinedFittedTrackArrayPtr.h"

#include "H1Tracks/H1ForwardFittedTrack.h"
#include "H1Tracks/H1ForwardFittedTrackArrayPtr.h"

//#include "H1Tracks/H1FSTTrackArrayPtr.h"

//#include "H1Mods/H1GkiInfoArrayPtr.h"
//#include "H1Mods/H1GkiInfo.h"
#include <TLorentzRotation.h>
#include "H1HadronicCalibration/H1HadronicCalibration.h"
#include "H1PhysUtils/H1MakeKine.h"

using namespace std;

#define PI 3.1415926

static double const ME=0.0005109989461;
static double const M_CHARGED_PION=0.13957061;

static double const ELEC_ISOLATION_CONE=0.1;

bool floatEqual(double a,double b) {
   double d1=fabs(a-b);
   double d2=fabs(a+b);
   return (d1<=1.E-5*d2);
}

TLorentzRotation BoostToHCM(TLorentzVector const &eBeam_lab,
                            TLorentzVector const &pBeam_lab,
                            TLorentzVector const &eScat_lab) {
   TLorentzVector q_lab=eBeam_lab - eScat_lab;
   TLorentzVector p_plus_q=pBeam_lab + q_lab;
   // boost to HCM
   TLorentzRotation boost=TLorentzRotation(-1.0*p_plus_q.BoostVector());
   TLorentzVector pBoost=boost*pBeam_lab;
   TVector3 axis=pBoost.BoostVector();
   // rotate away y-coordinate
   boost.RotateZ(-axis.Phi());
   // rotate away x-coordinate
   boost.RotateY(M_PI-axis.Theta());
   return boost;
}
//boost to HCM frame with kinematics from e-sigma method
TLorentzRotation BoostToHCM_es(TLorentzVector const &eBeam_lab,
                               TLorentzVector const &pBeam_lab,
                               TLorentzVector const &eScat_lab, 
                               double Q2_es, 
                               double y_es) {

   double escat_lab_es_E = (Q2_es)/(4.*eBeam_lab.E()) + eBeam_lab.E()*(1.-y_es);
   double b_par = 4.*eBeam_lab.E()*eBeam_lab.E()*(1.-y_es)/(Q2_es);
   double escat_lab_es_theta = TMath::ACos((1.-b_par)/(1.+b_par));
   
   double escat_lab_es_pz = sqrt(escat_lab_es_E*escat_lab_es_E - ME*ME)*TMath::Cos(escat_lab_es_theta);
   double escat_lab_es_pt = sqrt(escat_lab_es_E*escat_lab_es_E - ME*ME - escat_lab_es_pz*escat_lab_es_pz);
   double escat_lab_es_eta = -TMath::Log(TMath::Tan(escat_lab_es_theta/2.));
   double phi_elec = eScat_lab.Phi();

   TLorentzVector eScat_lab_ES;
   eScat_lab_ES.SetPtEtaPhiE(escat_lab_es_pt, escat_lab_es_eta, phi_elec, escat_lab_es_E);

   //same as before
   TLorentzVector q_lab=eBeam_lab - eScat_lab_ES;
   TLorentzVector p_plus_q=pBeam_lab + q_lab;

   // boost to HCM
   TLorentzRotation boost=TLorentzRotation(-1.0*p_plus_q.BoostVector());
   TLorentzVector pBoost=boost*pBeam_lab;
   TVector3 axis=pBoost.BoostVector();
   // rotate away y-coordinate
   boost.RotateZ(-phi_elec);
   // rotate away x-coordinate
   boost.RotateY(M_PI-axis.Theta());

   return boost;

}

double deltaPhi( double phi_1, double phi_2 ){

   double relAngle = 0.;
   if( phi_1 > phi_2 ){
      relAngle = phi_1 - phi_2;
      if( relAngle > PI ) relAngle = 2*PI - relAngle; 
   }
   else{
      relAngle = phi_1 - phi_2;
      if( relAngle > -PI ) relAngle = -relAngle;
      else relAngle = 2*PI + relAngle;
   }
   return relAngle;
}

void GetKinematics(TLorentzVector const &ebeam,TLorentzVector const &pbeam,
                   TLorentzVector const &escat,
                   Float_t *x,Float_t *y,Float_t *Q2) {
   TLorentzVector q=ebeam-escat;
   *Q2= -q.Mag2();
   double pq=pbeam.Dot(q);
   *y= pq/pbeam.Dot(ebeam);
   *x= *Q2/(2.*pq);
}


struct MyEvent {
   // general information
   Int_t run,evno; // run and event number
   Float_t w; // event weight

      // trigger information
   UInt_t l1l2l3ac[4];
   UInt_t l1l2l3rw[4];
   UInt_t haveSpacalTrigger;
   UInt_t haveEtag6Trigger;
   Float_t trigWeightRW;
   Float_t trigWeightAC;
   Float_t pPilotWeight;
   Float_t ePilotWeight;

   // background finders
   UInt_t ibg;
   UInt_t ibgfm;
   UInt_t ibgam;
   UInt_t iqn;

   // event quality .. not complete yet
   Int_t vertexType;
   Int_t FSTHV;
   Float_t vertex[3];
   Float_t beamSpot[2];
   Float_t beamTilt[2];
   Float_t eProtonBeam,eElectronBeam;

   Float_t eTag6Energy;
   Float_t eTag6X,eTag6Y;
   Float_t photonTagEnergy; 
   Float_t etaMaxHat,etaMaxHfs400,etaMaxHfs800;

   // Monte Carlo information
   Float_t eProtonBeamMC;
   Float_t eElectronBeamMC;
   Float_t simvertex[3];
   Float_t xGKI,yGKI,Q2GKI;

   UInt_t haveScatteredElectron;
   Float_t elecPxMC,elecPyMC,elecPzMC,elecEMC,elecEradMC; // scattered electron
   Float_t elecEcraREC;
   Float_t xMC,yMC,Q2MC;
   Float_t xMC_es,yMC_es,Q2MC_es;

   enum {
      nMCtrack_MAX=400
   };
   // if there is no MC info, nMCtrack is set to zero
   Int_t nMCtrackAll;
   Int_t nMCtrack;
   Int_t idMC[nMCtrack_MAX];
   Int_t idxRad;

   Float_t pxMC[nMCtrack_MAX];
   Float_t pyMC[nMCtrack_MAX];
   Float_t pzMC[nMCtrack_MAX];
   Float_t etaMC[nMCtrack_MAX];
   Float_t chargeMC[nMCtrack_MAX];

   Float_t ptStarMC[nMCtrack_MAX];
   Float_t etaStarMC[nMCtrack_MAX];
   Float_t phiStarMC[nMCtrack_MAX];
   Float_t ptStar2MC[nMCtrack_MAX];
   Float_t etaStar2MC[nMCtrack_MAX];
   Float_t phiStar2MC[nMCtrack_MAX];

   Float_t log10zMC[nMCtrack_MAX];
   Int_t imatchMC[nMCtrack_MAX];

   // reconstructed quantities
   Int_t elecChargeREC; //scattered electron alone
   Float_t elecPxREC,elecPyREC,elecPzREC,elecEREC,elecEradREC; // scattered electron + neutrals
   Float_t elecXclusREC,elecYclusREC, elecThetaREC,elecEnergyREC,elecEfracREC,elecHfracREC;
   Int_t elecTypeREC;

   Float_t xREC,yREC,Q2REC;
   Float_t xREC_es,yREC_es,Q2REC_es;
   Float_t hfsPxREC,hfsPyREC,hfsPzREC,hfsEREC; // hadronic final state
   Float_t hfsPxHadSpa,hfsPyHadSpa,hfsPzHadSpa,hfsEHadSpa; // hadr SpaCal contribution
   Float_t hfsPxEMSpa,hfsPyEMSpa,hfsPzEMSpa,hfsEEMSpa; // EM SpaCal contribution
   enum {
      nRECtrack_MAX=200
   };
   // if there is no scattered electron, no tracks are saved either
   //  (nRECtrack=0)
   Int_t nRECtrackAll;
   Int_t nRECtrack;
   Int_t typeChgREC[nRECtrack_MAX];

   Float_t pxREC[nRECtrack_MAX];
   Float_t pyREC[nRECtrack_MAX];
   Float_t pzREC[nRECtrack_MAX];
   Float_t pREC[nRECtrack_MAX];
   Float_t peREC[nRECtrack_MAX];
   Float_t etaREC[nRECtrack_MAX];

   Float_t ptStarREC[nRECtrack_MAX];
   Float_t etaStarREC[nRECtrack_MAX];
   Float_t phiStarREC[nRECtrack_MAX];
   Float_t ptStar2REC[nRECtrack_MAX];
   Float_t etaStar2REC[nRECtrack_MAX];
   Float_t phiStar2REC[nRECtrack_MAX];
   Float_t chi2vtxREC[nRECtrack_MAX];
   Int_t   vtxNdfREC[nRECtrack_MAX];
   Int_t   vtxNHitsREC[nRECtrack_MAX];
   Float_t vtxTrackLengthREC[nRECtrack_MAX];
   Float_t dcaPrimeREC[nRECtrack_MAX];
   Float_t dz0PrimeREC[nRECtrack_MAX];

   //non vertex fitted parameter not used
   Float_t chi2nvREC[nRECtrack_MAX]; 
   Int_t   nvNdfREC[nRECtrack_MAX];
   Int_t   nvNHitsREC[nRECtrack_MAX];
   Float_t nvTrackLengthREC[nRECtrack_MAX];
   
   Float_t startHitsRadiusREC[nRECtrack_MAX];
   Float_t endHitsRadiusREC[nRECtrack_MAX];
   Float_t trkThetaREC[nRECtrack_MAX];
   Float_t chi2TrkREC[nRECtrack_MAX];
   Int_t   ndfTrkREC[nRECtrack_MAX];
   Float_t zLengthHitREC[nRECtrack_MAX];
   Float_t chi2LinkREC[nRECtrack_MAX];
   Int_t ndfLinkREC[nRECtrack_MAX];
   Float_t rZeroREC[nRECtrack_MAX];

   Float_t log10zREC[nRECtrack_MAX];
   Float_t nucliaREC[nRECtrack_MAX];
   Float_t dmatchREC[nRECtrack_MAX];
   Int_t imatchREC[nRECtrack_MAX];

   // FST tracks
   Int_t nRECfstFitted;

   // auxillary information, not to be saved
   H1PartMC const *partMC[nMCtrack_MAX];
   TVector3 momREC[nRECtrack_MAX];
   TMatrix covREC[nRECtrack_MAX];
};

class DummyFiller : public H1EventFiller {
public:
   virtual void   Fill(H1Event* event) { }
};

int main(int argc, char* argv[]) {
   // parse the command line
   H1StdCmdLine opts;
   opts.Parse(&argc, argv);

   //open run selection and detector status file
   TString goodRunFileName("SelectedRuns_0607-and-lowE.root");
   TFile goodRunFile(goodRunFileName);
   if(!goodRunFile.IsOpen()) {
      cerr<<"Error: could not open file "<<goodRunFileName<<"\n";
      return 2;
   }
   H1RunList* goodRunList
      = (H1RunList*) goodRunFile.Get("H1RunList");
   if(!goodRunList) {
      cerr<<"Error: no runlist in file - return!\n";
      return 2;
   }
   H1DetectorStatus *detectorStatus
      = (H1DetectorStatus*)goodRunFile.Get("MyDetectorStatus");
   if(!detectorStatus) {
      cerr<<"Error: no detector status in file - return!\n";
      return 3;
   }
   H1DetectorStatus detectorStatusFST(*detectorStatus);
   detectorStatusFST.SetFSTOn();

   // Load mODS/HAT files
   H1Tree::Instance()->Open();            // this statement must be there!

   //cout << H1Tree::Instance()->SelectHat("NumJPsi>0")
   //     << " events selected " << endl;

   TFile *file=new TFile(opts.GetOutput(), "RECREATE");


   TTree *output=new TTree("properties","properties");
   MyEvent myEvent;
   output->Branch("run",&myEvent.run,"run/I");
   output->Branch("evno",&myEvent.evno,"evno/I");
   output->Branch("w",&myEvent.w,"w/F");
   output->Branch("FSTHV",&myEvent.FSTHV,"FSTHV/I");
   output->Branch("vertexType",&myEvent.vertexType,"vertexType/I");
   output->Branch("vertex",myEvent.vertex,"vertex[3]/F");
   output->Branch("beamSpot",myEvent.beamSpot,"beamSpot[2]/F");
   output->Branch("beamTilt",myEvent.beamTilt,"beamTilt[2]/F");
   output->Branch("eProtonBeam",&myEvent.eProtonBeam,"eProtonBeam/F");
   output->Branch("eElectronBeam",&myEvent.eElectronBeam,"eElectronBeam/F");
   output->Branch("eProtonBeamMC",&myEvent.eProtonBeamMC,"eProtonBeamMC/F");
   output->Branch("eElectronBeamMC",&myEvent.eElectronBeamMC,"eElectronBeamMC/F");
   output->Branch("photonTagEnergy",&myEvent.photonTagEnergy,"photonTagEnergy/F");
   output->Branch("eTag6Energy",&myEvent.eTag6Energy,"eTag6Energy/F");
   output->Branch("eTag6X",&myEvent.eTag6X,"eTag6X/F");
   output->Branch("eTag6Y",&myEvent.eTag6Y,"eTag6Y/F");
   output->Branch("etaMaxHat",&myEvent.etaMaxHat,"etaMaxHat/F");
   output->Branch("etaMaxHfs400",&myEvent.etaMaxHfs400,"etaMaxHfs400/F");
   output->Branch("etaMaxHfs800",&myEvent.etaMaxHfs800,"etaMaxHfs800/F");
   
   output->Branch("l1l2l3ac",myEvent.l1l2l3ac,"l1l2l3ac[4]/i");
   output->Branch("l1l2l3rw",myEvent.l1l2l3ac,"l1l2l3rw[4]/i");
   output->Branch("haveSpacalTrigger",&myEvent.haveSpacalTrigger,"haveSpacalTrigger/i");
   output->Branch("haveEtag6Trigger",&myEvent.haveEtag6Trigger,"haveEtag6Trigger/i");
   output->Branch("trigWeightAC",&myEvent.trigWeightAC,"trigWeightAC/F");
   output->Branch("trigWeightRW",&myEvent.trigWeightRW,"trigWeightRW/F");

   output->Branch("ibg",&myEvent.ibg,"ibg/I");
   output->Branch("ibgfm",&myEvent.ibgfm,"ibgfm/I");
   output->Branch("ibgam",&myEvent.ibgam,"ibgam/I");
   output->Branch("iqn",&myEvent.iqn,"iqn/I");
   output->Branch("pPilotWeight",&myEvent.pPilotWeight,"pPilotWeight/F");
   output->Branch("ePilotWeight",&myEvent.ePilotWeight,"ePilotWeight/F");

   output->Branch("simvertex",myEvent.simvertex,"simvertex[3]/F");
   output->Branch("elecEradMC",&myEvent.elecEradMC,"elecEradMC/F");
   output->Branch("elecPxMC",&myEvent.elecPxMC,"elecPxMC/F");
   output->Branch("elecPyMC",&myEvent.elecPyMC,"elecPyMC/F");
   output->Branch("elecPzMC",&myEvent.elecPzMC,"elecPzMC/F");
   output->Branch("elecEMC",&myEvent.elecEMC,"elecEMC/F");
   output->Branch("xGKI",&myEvent.xGKI,"xGKI/F");
   output->Branch("yGKI",&myEvent.yGKI,"yGKI/F");
   output->Branch("Q2GKI",&myEvent.Q2GKI,"Q2GKI/F");
   output->Branch("xMC",&myEvent.xMC,"xMC/F");
   output->Branch("yMC",&myEvent.yMC,"yMC/F");
   output->Branch("Q2MC",&myEvent.Q2MC,"Q2MC/F");
   output->Branch("xMC_es",&myEvent.xMC_es,"xMC_es/F");
   output->Branch("yMC_es",&myEvent.yMC_es,"yMC_es/F");
   output->Branch("Q2MC_es",&myEvent.Q2MC_es,"Q2MC_es/F");

   output->Branch("nMCtrackAll",&myEvent.nMCtrackAll,"nMCtrackAll/I");
   output->Branch("nMCtrack",&myEvent.nMCtrack,"nMCtrack/I");
   output->Branch("idMC",myEvent.idMC,"idMC[nMCtrack]/I");
   output->Branch("idxRad",&myEvent.idxRad,"idxRad/I");

   output->Branch("pxMC",myEvent.pxMC,"pxMC[nMCtrack]/F");
   output->Branch("pyMC",myEvent.pyMC,"pyMC[nMCtrack]/F");
   output->Branch("pzMC",myEvent.pzMC,"pzMC[nMCtrack]/F");
   output->Branch("etaMC",myEvent.etaMC,"etaMC[nMCtrack]/F");
   output->Branch("chargeMC",myEvent.chargeMC,"chargeMC[nMCtrack]/F");
   output->Branch("ptStarMC",myEvent.ptStarMC,"ptStarMC[nMCtrack]/F");
   output->Branch("etaStarMC",myEvent.etaStarMC,"etaStarMC[nMCtrack]/F");
   output->Branch("phiStarMC",myEvent.phiStarMC,"phiStarMC[nMCtrack]/F");
   output->Branch("ptStar2MC",myEvent.ptStar2MC,"ptStar2MC[nMCtrack]/F");
   output->Branch("etaStar2MC",myEvent.etaStar2MC,"etaStar2MC[nMCtrack]/F");
   output->Branch("phiStar2MC",myEvent.phiStar2MC,"phiStar2MC[nMCtrack]/F");
   
   output->Branch("log10zMC",myEvent.log10zMC,"log10zMC[nMCtrack]/F");
   output->Branch("imatchMC",myEvent.imatchMC,"imatchMC[nMCtrack]/I");

   output->Branch("haveScatteredElectron",&myEvent.haveScatteredElectron,"haveScatteredElectron/i");

   output->Branch("elecChargeREC",&myEvent.elecChargeREC,"elecChargeREC/I");
   output->Branch("elecEradREC",&myEvent.elecEradREC,"elecEradREC/F");
   output->Branch("elecPxREC",&myEvent.elecPxREC,"elecPxREC/F");
   output->Branch("elecPyREC",&myEvent.elecPyREC,"elecPyREC/F");
   output->Branch("elecPzREC",&myEvent.elecPzREC,"elecPzREC/F");
   output->Branch("elecEREC",&myEvent.elecEREC,"elecEREC/F");
   output->Branch("elecEcraREC",&myEvent.elecEcraREC,"elecEcraREC/F");
   output->Branch("elecXclusREC",&myEvent.elecXclusREC,"elecXclusREC/F");
   output->Branch("elecYclusREC",&myEvent.elecYclusREC,"elecYclusREC/F");
   output->Branch("elecThetaREC",&myEvent.elecThetaREC,"elecThetaREC/F");
   output->Branch("elecTypeREC",&myEvent.elecTypeREC,"elecTypeREC/I");
   output->Branch("elecEnergyREC",&myEvent.elecEnergyREC,"elecEnergyREC/F");
   output->Branch("elecEfracREC",&myEvent.elecEfracREC,"elecEfracREC/F");
   output->Branch("elecHfracREC",&myEvent.elecHfracREC,"elecHfracREC/F");

   output->Branch("xREC",&myEvent.xREC,"xREC/F");
   output->Branch("yREC",&myEvent.yREC,"yREC/F");
   output->Branch("Q2REC",&myEvent.Q2REC,"Q2REC/F");
   output->Branch("xREC_es",&myEvent.xREC_es,"xREC_es/F");
   output->Branch("yREC_es",&myEvent.yREC_es,"yREC_es/F");
   output->Branch("Q2REC_es",&myEvent.Q2REC_es,"Q2REC_es/F");
   output->Branch("hfsPxREC",&myEvent.hfsPxREC,"hfsPxREC/F");
   output->Branch("hfsPyREC",&myEvent.hfsPyREC,"hfsPyREC/F");
   output->Branch("hfsPzREC",&myEvent.hfsPzREC,"hfsPzREC/F");
   output->Branch("hfsEREC",&myEvent.hfsEREC,"hfsEREC/F");
   output->Branch("hfsPxHadSpa",&myEvent.hfsPxHadSpa,"hfsPxHadSpa/F");
   output->Branch("hfsPyHadSpa",&myEvent.hfsPyHadSpa,"hfsPyHadSpa/F");
   output->Branch("hfsPzHadSpa",&myEvent.hfsPzHadSpa,"hfsPzHadSpa/F");
   output->Branch("hfsEHadSpa",&myEvent.hfsEHadSpa,"hfsEHadSpa/F");
   output->Branch("hfsPxEMSpa",&myEvent.hfsPxEMSpa,"hfsPxEMSpa/F");
   output->Branch("hfsPyEMSpa",&myEvent.hfsPyEMSpa,"hfsPyEMSpa/F");
   output->Branch("hfsPzEMSpa",&myEvent.hfsPzEMSpa,"hfsPzEMSpa/F");
   output->Branch("hfsEEMSpa",&myEvent.hfsEEMSpa,"hfsEEMSpa/F");

   output->Branch("nRECtrackAll",&myEvent.nRECtrackAll,"nRECtrackAll/I");
   output->Branch("nRECtrack",&myEvent.nRECtrack,"nRECtrack/I");
   output->Branch("typeChgREC",myEvent.typeChgREC,"typeChgREC[nRECtrack]/I");
   
   output->Branch("pxREC",myEvent.pxREC,"pxREC[nRECtrack]/F");
   output->Branch("pyREC",myEvent.pyREC,"pyREC[nRECtrack]/F");
   output->Branch("pzREC",myEvent.pzREC,"pzREC[nRECtrack]/F");
   output->Branch("pREC",myEvent.pREC,"pREC[nRECtrack]/F");
   output->Branch("peREC",myEvent.peREC,"peREC[nRECtrack]/F");
   output->Branch("etaREC",myEvent.etaREC,"etaREC[nRECtrack]/F");

   /*output->Branch("ptStarREC",myEvent.ptStarREC,"ptStarREC[nRECtrack]/F");
   output->Branch("etaStarREC",myEvent.etaStarREC,"etaStarREC[nRECtrack]/F");
   output->Branch("phiStarREC",myEvent.phiStarREC,"phiStarREC[nRECtrack]/F");
   output->Branch("ptStar2REC",myEvent.ptStar2REC,"ptStar2REC[nRECtrack]/F");
   output->Branch("etaStar2REC",myEvent.etaStar2REC,"etaStar2REC[nRECtrack]/F");
   output->Branch("phiStar2REC",myEvent.phiStar2REC,"phiStar2REC[nRECtrack]/F");
   output->Branch("log10zREC",myEvent.log10zREC,"log10zREC[nRECtrack]/F"); */
   output->Branch("chi2vtxREC",myEvent.chi2vtxREC,"chi2vtxREC[nRECtrack]/F");
   output->Branch("chi2nvREC",myEvent.chi2nvREC,"chi2nvREC[nRECtrack]/F");
   output->Branch("vtxNdfREC",myEvent.vtxNdfREC,"vtxNdfREC[nRECtrack]/I");
   output->Branch("nvNdfREC",myEvent.nvNdfREC,"nvNdfREC[nRECtrack]/I");
   output->Branch("vtxNHitsREC",myEvent.vtxNHitsREC,"vtxNHitsREC[nRECtrack]/I");
   output->Branch("nvNHitsREC",myEvent.nvNHitsREC,"nvNHitsREC[nRECtrack]/I");
   output->Branch("vtxTrackLengthREC",myEvent.vtxTrackLengthREC,"vtxTrackLengthREC[nRECtrack]/F");
   output->Branch("nvTrackLengthREC",myEvent.nvTrackLengthREC,"nvTrackLengthREC[nRECtrack]/F");
   output->Branch("dcaPrimeREC",myEvent.dcaPrimeREC,"dcaPrimeREC[nRECtrack]/F");
   output->Branch("dz0PrimeREC",myEvent.dz0PrimeREC,"dz0PrimeREC[nRECtrack]/F");
   
   output->Branch("startHitsRadiusREC",myEvent.startHitsRadiusREC,"startHitsRadiusREC[nRECtrack]/F");
   output->Branch("endHitsRadiusREC",myEvent.endHitsRadiusREC,"endHitsRadiusREC[nRECtrack]/F");
   output->Branch("trkThetaREC",myEvent.trkThetaREC,"trkThetaREC[nRECtrack]/F");
   output->Branch("chi2TrkREC",myEvent.chi2TrkREC,"chi2TrkREC[nRECtrack]/F");
   output->Branch("ndfTrkREC",myEvent.ndfTrkREC,"ndfTrkREC[nRECtrack]/I");
   output->Branch("zLengthHitREC",myEvent.zLengthHitREC,"zLengthHitREC[nRECtrack]/F");
   output->Branch("chi2LinkREC",myEvent.chi2LinkREC,"chi2LinkREC[nRECtrack]/F");
   output->Branch("ndfLinkREC",myEvent.ndfLinkREC,"ndfLinkREC[nRECtrack]/I");
   output->Branch("rZeroREC",myEvent.rZeroREC,"rZeroREC[nRECtrack]/F");

   output->Branch("nucliaREC",myEvent.nucliaREC,"nucliaREC[nRECtrack]/F");
   output->Branch("dmatchREC",myEvent.dmatchREC,"dmatchREC[nRECtrack]/F");
   output->Branch("imatchREC",myEvent.imatchREC,"imatchREC[nRECtrack]/I");

   output->Branch("nRECfstFitted",&myEvent.nRECfstFitted,"nRECfstFitted/I");

   H1ShortPtr runtype("RunType"); // 0=data, 1=MC, 2=CERN test, 3=CERN MC test
   H1FloatPtr beamx0("BeamX0");          // x position of beam spot (at z=0)
   H1FloatPtr beamy0("BeamY0");          // y position of beam spot (at z=0)

   H1FloatPtr eBeamP("EBeamP"); // proton beam energy from DMIS
   H1FloatPtr eBeamE("EBeamE"); // electron beam energy from DMIS
   H1FloatPtr beamtiltx0("BeamTiltX0");      // x slope of beam axis
   H1FloatPtr beamtilty0("BeamTiltY0");      // y slope of beam axis
   H1IntPtr run("RunNumber");
   H1IntPtr evno("EventNumber");
  
   H1BytePtr l1l2l3ac("Il1l2l3ac");
   H1BytePtr l1l2rw("Il1l2rw");
   H1BytePtr l1l3rw("Il1l3rw");

   H1FloatPtr Phtag("Phtag");;
   H1FloatPtr Etag6("Etag6");
   H1FloatPtr Xtag6("Xtag6");
   H1FloatPtr Ytag6("Ytag6");
   H1FloatPtr EtaMax("EtaMax");
   H1FloatPtr EtaMaxClus("EtaMaxClus");

   H1IntPtr   ibg("Ibg");  // standard array of background finders from OM group (bit packed)
   H1IntPtr  ibgfm("Ibgfm");  // extra finders from OM  (bit packed)
   H1IntPtr  ibgam("Ibgam");  // background finders from DUK group (bit packed)
   H1IntPtr  iqn("Iqn");           // The Lar coherent noise flag
   H1ShortPtr  Bunch("Bunch");           // The Lar coherent noise flag

   H1FloatPtr Q2Gki("Q2Gki");
   H1FloatPtr xGki("XGki");
   H1FloatPtr yGki("YGki");
   H1FloatPtr weight1("Weight1");
   H1FloatPtr weight2("Weight2");

   H1FloatPtr genEnElec("GenEnElec"); //   Electron energy, combined with photon for FSR
   H1FloatPtr genPhElec("GenPhElec"); //   Electron phi, combined with photon for FSR
   H1FloatPtr genThElec("GenThElec"); //   Electron theta, combined with photon for FSR

   H1ShortPtr ivtyp("Ivtyp"); // vertex type
   H1SelVertexArrayPtr vertex; // all good vertices
   //H1PartCandArrayPtr partCand; // all good tracks
   H1PartCandArrayPtr partCandArray; // all good tracks

   H1PartMCArrayPtr mcpart;

   H1FSTFittedTrackArrayPtr fstFittedTrack;
   //H1FSTTrackArrayPtr fstNonFittedTrack;

   Int_t eventCounter = 0;

   H1HadronicCalibration *hadronicCalibration=H1HadronicCalibration::Instance();
   hadronicCalibration->SetCalibrationMethod(H1HadronicCalibration::eIterative);
   //hadronicCalibration->SetCalibrationMethod(H1HadronicCalibration::eHighPtJet);
   hadronicCalibration->ApplyHadronicCalibration(kTRUE);

   // Loop over events
   static int print=10;
   while (gH1Tree->Next() && !opts.IsMaxEvent(eventCounter)) {
      ++eventCounter;

      // skip runs not in list of good runs
      if(!goodRunList->FindRun(*run)) continue;
      // skip data events with bad detector status
      if(!detectorStatus->IsOn()) continue;

      double w=*weight1 * *weight2;
      if(print || ((eventCounter %10000)==0))  { 
         cout<<eventCounter
             <<" event "<<*run<<" "<<*evno<<" type="<<*runtype<<" weight="<<w<<"\n";
         if(!print) print=1; //print this event
      }
      myEvent.run=*run;
      myEvent.evno=*evno;
      myEvent.w=w;

      if(*runtype==1) {
         // handle MC information
         if(print) {
            // kinematic variables from GKI bank
            cout<<" xGKI="<<*xGki<<" Q2GKI="<<*Q2Gki
                <<" y="<<*yGki
                <<" w="<<w<<"\n";
         }
         myEvent.xGKI = *xGki;
         myEvent.yGKI = *yGki;
         myEvent.Q2GKI = *Q2Gki;

         double _Q2GKI = myEvent.Q2GKI;
         double _yGKI = myEvent.yGKI;
         double _xGKI = myEvent.xGKI;

         H1GetPartMCId mcPartId(&*mcpart);
         mcPartId.Fill();

         //protection over the gen only MC, nonradiative MC
         if(mcpart.GetEntries() <= 0){
            cout << "empty events!"; 
            continue;
         }

         TLorentzVector ebeam_MC_lab
            (mcpart[mcPartId.GetIdxBeamElectron()]->GetFourVector());
         TLorentzVector pbeam_MC_lab
            (mcpart[mcPartId.GetIdxBeamProton()]->GetFourVector());

         myEvent.eProtonBeamMC=pbeam_MC_lab.E();
         myEvent.eElectronBeamMC=ebeam_MC_lab.E();

         TLorentzVector escat0_MC_lab
            (mcpart[mcPartId.GetIdxScatElectron()]->GetFourVector());

         /*begin scattered electron and radiative photons*/
         TLorentzVector radPhot_MC_lab;
         if( mcPartId.GetIdxRadPhoton() >= 0 ){
            radPhot_MC_lab = mcpart[mcPartId.GetIdxRadPhoton()]->GetFourVector();
      
            double delta_phi = escat0_MC_lab.Phi() - radPhot_MC_lab.Phi();
         
            /*if( mcPartId.GetRadType() == 0 ){
               h_dPhi_theta_noR->Fill(escat0_MC_lab.Theta(), delta_phi );
            }
            else if( mcPartId.GetRadType() == 1 ){
               h_dPhi_theta_ISR->Fill(escat0_MC_lab.Theta(), delta_phi );
            }
            else if( mcPartId.GetRadType() == 2 ){
               h_dPhi_theta_FSR->Fill(escat0_MC_lab.Theta(), delta_phi );
            }
            else{
               cout << "something is wrong!" << endl;
               } */
         }

         //HFS 4-vectors
         //TLorentzVector hfs_MC_lab = ebeam_MC_lab+pbeam_MC_lab-escat0_MC_lab;
         double hfs_MC_E_lab = 0.;
         double hfs_MC_pz_lab = 0.;
         for(int i=0;i<mcpart.GetEntries();i++) {
            H1PartMC *part=mcpart[i];
            int pdgid = part->GetPDG();
            int status=part->GetStatus();
            float charge=part->GetCharge();
            int elec_id = mcPartId.GetIdxScatElectron();
            if( status != 0 || i == elec_id ) continue;

            hfs_MC_E_lab += part->GetE();
            hfs_MC_pz_lab += part->GetPz();
         }

         double sigma = hfs_MC_E_lab - hfs_MC_pz_lab;

         H1MakeKine makeKin_es;
         makeKin_es.MakeESig(escat0_MC_lab.E(), escat0_MC_lab.Theta(),sigma, ebeam_MC_lab.E(), pbeam_MC_lab.E());
         
         double Q2_esigma = makeKin_es.GetQ2es();
         double y_esigma = makeKin_es.GetYes();
         double x_esigma = makeKin_es.GetXes();

         myEvent.Q2MC_es = Q2_esigma;
         myEvent.yMC_es = y_esigma;
         myEvent.xMC_es = x_esigma;

         H1MakeKine makeKin_ISR;
         H1MakeKine makeKin_FSR;
         H1MakeKine makeKin_noR;

         double Q2_ISR=Q2_esigma;
         double y_ISR=y_esigma;
         double x_ISR=x_esigma;

         double Q2_FSR=Q2_esigma;
         double y_FSR=y_esigma;
         double x_FSR=x_esigma;

         double Q2_noR=Q2_esigma;
         double y_noR=y_esigma;
         double x_noR=x_esigma;

         myEvent.idxRad = mcPartId.GetRadType();

         if( mcPartId.GetIdxRadPhoton() >= 0 ){
            
            radPhot_MC_lab = mcpart[mcPartId.GetIdxRadPhoton()]->GetFourVector();
            
            if( mcPartId.GetRadType() == 1 ){
               makeKin_ISR.MakeESig(escat0_MC_lab.E(), escat0_MC_lab.Theta(), sigma, (ebeam_MC_lab).E(), pbeam_MC_lab.E());
               Q2_ISR=makeKin_ISR.GetQ2es();
               y_ISR=makeKin_ISR.GetYes();
               x_ISR=makeKin_ISR.GetXes();

               /* h_ISR_Q2diff->Fill( Q2_ISR - _Q2GKI );
               h_ISR_Ydiff->Fill( y_ISR - _yGKI );
               h_ISR_Xdiff->Fill( x_ISR - _xGKI ); */

            }
            else if( mcPartId.GetRadType() == 2 ){
               makeKin_FSR.MakeESig((escat0_MC_lab).E(), (escat0_MC_lab).Theta(), sigma, ebeam_MC_lab.E(), pbeam_MC_lab.E());
               Q2_FSR=makeKin_FSR.GetQ2es();
               y_FSR=makeKin_FSR.GetYes();
               x_FSR=makeKin_FSR.GetXes();

               /* h_FSR_Q2diff->Fill( Q2_FSR - _Q2GKI );
               h_FSR_Ydiff->Fill( y_FSR - _yGKI );
               h_FSR_Xdiff->Fill( x_FSR - _xGKI ); */
            }
         }
         else{
            makeKin_noR.MakeESig(escat0_MC_lab.E(), escat0_MC_lab.Theta(),sigma, ebeam_MC_lab.E(), pbeam_MC_lab.E());
            Q2_noR=makeKin_noR.GetQ2es();
            y_noR=makeKin_noR.GetYes();
            x_noR=makeKin_noR.GetXes();

            /* h_noR_Q2diff->Fill( Q2_noR - _Q2GKI );
            h_noR_Ydiff->Fill( y_noR - _yGKI );
            h_noR_Xdiff->Fill( x_noR - _xGKI );          */
         }
         //end test

         // add radiative photon(s) in a cone
         TLorentzVector escatPhot_MC_lab(escat0_MC_lab);
         set<int> isElectron;
         isElectron.insert(mcPartId.GetIdxScatElectron());
         for(int i=0;i<mcpart.GetEntries();i++) {
            H1PartMC *part=mcpart[i];
            int status=part->GetStatus();
            if((status==0 )&&(part->GetPDG()==22)) {
               TLorentzVector p(part->GetFourVector());
               if(p.DeltaR(escat0_MC_lab)<ELEC_ISOLATION_CONE) {
                  // this photon counts with the electron
                  isElectron.insert(i);
                  escatPhot_MC_lab += p;
               }
            }
         }
         myEvent.elecEradMC=escatPhot_MC_lab.E()-escat0_MC_lab.E();
         myEvent.elecPxMC=escatPhot_MC_lab.X();
         myEvent.elecPyMC=escatPhot_MC_lab.Y();
         myEvent.elecPzMC=escatPhot_MC_lab.Z();
         myEvent.elecEMC=escatPhot_MC_lab.E();

         if(print) {
            cout<<"MC scattered electron is made of "<<isElectron.size()<<" particle(s)\n";
         }

         //H1MakeKine maybe helpful
         GetKinematics(ebeam_MC_lab,pbeam_MC_lab,escatPhot_MC_lab,
                       &myEvent.xMC,&myEvent.yMC,&myEvent.Q2MC);
         TLorentzRotation boost_MC_HCM = BoostToHCM(ebeam_MC_lab,pbeam_MC_lab,escatPhot_MC_lab);
         TLorentzVector q_MC_lab(ebeam_MC_lab-escatPhot_MC_lab);
         //New boost using the e-Sigma method, scattered electrons are without radiative photon
         TLorentzRotation boost_MC_HCM_es = BoostToHCM_es(ebeam_MC_lab,pbeam_MC_lab,escat0_MC_lab,Q2_esigma,y_esigma);

         //difference with respect to GKI values:
         /* h_Xdiff->Fill( x_esigma - _xGKI );
         h_Q2diff->Fill( Q2_esigma - _Q2GKI );
         h_Ydiff->Fill( y_esigma - _yGKI ); */

         // final state particles
         //bool haveElectron=false;
         myEvent.nMCtrackAll=0;
         myEvent.nMCtrack=0;
         for(int i=0;i<mcpart.GetEntries();i++) {
            
            H1PartMC *part=mcpart[i];
            if(print) {
               //cout << i << " " ; part->Print();
            }
            // skip particles counted as electron
            if(isElectron.find(i)!=isElectron.end()) continue;

            int status=part->GetStatus();
            if(status==0) {
               // generator "stable" particles
               // if((!haveElectron)&&
               //    ((part->GetPDG()==11)||(part->GetPDG()== -11))) {
               //    haveElectron=true;               
               // } else 
               if(part->GetCharge()!=0.) {
                  // other charged particles
                  TLorentzVector h=part->GetFourVector();
                  double log10z=TMath::Log10((h*pbeam_MC_lab)/(q_MC_lab*pbeam_MC_lab));
                  // boost to hadronic-centre-of-mass frame
                  TLorentzVector hStar = boost_MC_HCM*h;
                  TLorentzVector hStar2 = boost_MC_HCM_es*h;
                  double etaStar=hStar.Eta();
                  double ptStar=hStar.Pt();
                  double phiStar=hStar.Phi();

                  double etaStar2=hStar2.Eta();
                  double ptStar2=hStar2.Pt();
                  double phiStar2=hStar2.Phi();

                  if(print && etaStar2 < -20) {
                     //cout << i << " " ; part->Print();
                     cout<<"MCpart "<<myEvent.nMCtrackAll
                         <<" "<<part->GetPDG()
                         <<" etaLab="<<h.Eta()
                         <<" ptLab="<<h.Pt()
                         <<" phiLab="<<h.Phi()
                         <<" ptStar="<<ptStar
                         <<" etaStar="<<etaStar
                         <<" phiStar="<<phiStar
                         <<" ptStar2="<<ptStar2
                         <<" etaStar2="<<etaStar2
                         <<" phiStar2="<<phiStar2
                         <<" Boost px "<<hStar2.Px()
                         <<" Boost py "<<hStar2.Py()
                         <<" Boost pz "<<hStar2.Pz()
                         <<" log10(z)="<<log10z<<"\n";
                  }
                  myEvent.nMCtrackAll++;
                  if(myEvent.nMCtrack<MyEvent::nMCtrack_MAX) {
                     int k=myEvent.nMCtrack;
                     myEvent.idMC[k]=part->GetPDG();
                     myEvent.pxMC[k]=h.X();
                     myEvent.pyMC[k]=h.Y();
                     myEvent.pzMC[k]=h.Z();
                     myEvent.etaMC[k]=h.Eta();
                     myEvent.chargeMC[k]=part->GetCharge();

                     myEvent.ptStarMC[k]=hStar.Pt();
                     myEvent.etaStarMC[k]=hStar.Eta();
                     myEvent.phiStarMC[k]=hStar.Phi();

                     myEvent.ptStar2MC[k]=hStar2.Pt();
                     myEvent.etaStar2MC[k]=hStar2.Eta();
                     myEvent.phiStar2MC[k]=hStar2.Phi();

                     myEvent.log10zMC[k]=log10z;
                     myEvent.imatchMC[k]=-1;
                     myEvent.partMC[k]=part;
                     myEvent.nMCtrack=k+1;
                  }
               }
            } // end loop over stable particles
         }
      }//end of MC particles

      // define initial state particle four-vectors
      double ee=*eBeamE;
      double pe= sqrt((ee+ME)*(ee-ME));
#ifdef CORRECT_FOR_TILT
      double pxe= - *beamtiltx0 *pe;
      double pye= - *beamtilty0 *pe;
#else
      double pxe= 0.;
      double pye= 0.;
#endif
      double pze = - sqrt(pe*pe-pxe*pxe-pye*pye);

      double ep=*eBeamP;
      static double const MP=0.9382720813;
      double pp= sqrt((ep+MP)*(ep-MP));
#ifdef CORRECT_FOR_TILT
      double pxp= *beamtiltx0 *pp;
      double pyp= *beamtilty0 *pp;
#else
      double pxp= 0.;
      double pyp= 0.;
#endif
      double pzp= sqrt(pp*pp-pxp*pxp-pyp*pyp);

      myEvent.eProtonBeam=*eBeamP;
      myEvent.eElectronBeam=*eBeamE;

      TLorentzVector ebeam_REC_lab(pxe,pye,pze,ee);
      TLorentzVector pbeam_REC_lab(pxp,pyp,pzp,ep);

      if(print) {
         cout<<"HERA beam energies: "<<ee<<" "<<ep<<" beam tilt: "<<*beamtiltx0<<" "<<*beamtilty0<<"\n";
         /* cout<<"Beam proton beam 4-vector\n";
         pbeam_REC_lab.Print();
         cout<<"Beam electron beam 4-vector\n";
         ebeam_REC_lab.Print(); */
      }

      // check HV conditions for FST
      myEvent.FSTHV=detectorStatusFST.IsOn();

      // electron and photon tagger
      myEvent.photonTagEnergy=*Phtag;
      myEvent.eTag6Energy=*Etag6;
      myEvent.eTag6X=*Xtag6;
      myEvent.eTag6Y=*Ytag6;

      // etamax from hat
      myEvent.etaMaxHat=*EtaMax;
      
      // trigger information
      // prob_rw is the probability that none of the triggers has fired
      double prob_rw=1.0,prob_ac=1.0;
      H1TrigInfo *trigInfo=dynamic_cast<H1TrigInfo *>
         (H1DBManager::Instance()->GetDBEntry(H1TrigInfo::Class()));
      // save all prescales etc for this run
      if(!trigInfo) {
         cout<<"TrigInfo not found!!!\n";
      }

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
         myEvent.l1l2l3ac[i]=0;
         myEvent.l1l2l3rw[i]=0;
         for(int j=0;j<32;j++) {
            int st=i*32+j;
            if(!enabled[st]) continue;
            if(l1l2l3ac[st]) {
               myEvent.l1l2l3ac[i]|=(1<<j);
               
               if(allowedSubtrigger.find(st)!=allowedSubtrigger.end()) {
                  prob_ac *= (1.-1./prescales[st]);
               }
            }
            if(l1l2rw[st] && l1l3rw[st]) {
               myEvent.l1l2l3rw[i]|=(1<<j);
               if(allowedSubtrigger.find(st)!=allowedSubtrigger.end()) {
                  prob_rw *= (1.-1./prescales[st]);
               }
            }
         }
      }

      // trigWeightRW corrects for prescales using raw trigger selection
      myEvent.trigWeightRW=(prob_rw<1.0) ? (1./(1.-prob_rw)) : 0.0;
      // trigWeightAC corrects for prescales using acrual trigger selection
      myEvent.trigWeightAC=(prob_ac<1.0) ? (1./(1.-prob_ac)) : 0.0;

      // trigger selection:
      //   require trigWeightAC>0
      //     -> one of the actual subtriggers has fired
      //   use event weight 
      //      trigWeightRW*w
      //   select on yor favourite subtrigger
      //    e.g. S1 
      //      ( l1l2l3rw[0] & (1<<1) )!=0
      //    or S82
      //      ( l1l2l3rw[2] & (1<<18) )!=0

      myEvent.haveSpacalTrigger=
         (( myEvent.l1l2l3rw[0] & (1<<1))!=0) &&
         (myEvent.trigWeightAC>0.); // Spacal trigger S1

      myEvent.haveEtag6Trigger=0;
      if(myEvent.trigWeightAC>0.) {
         if(myEvent.l1l2l3rw[2] & (1<<18)) myEvent.haveEtag6Trigger+=1; // S82
         if(myEvent.l1l2l3rw[2] & (1<<22)) myEvent.haveEtag6Trigger+=2; // S86
      }
      // background and noise finders
      myEvent.ibg=*ibg;
      myEvent.ibgfm=*ibgfm;
      myEvent.ibgam=*ibgam;
      myEvent.iqn=*iqn;

      // proton or electron pilot bunch weight
      // To determine beamgas background from data
      H1BeamBunch* beamBunch = H1BeamBunch::Instance();
      myEvent.pPilotWeight=0.;
      myEvent.ePilotWeight=0.;
      H1BeamBunch::EBunchType bunchType=beamBunch->BunchType(*Bunch);
      if(bunchType==H1BeamBunch::kPilotP) {
         // event weight is given by ratio of proton  current ratio
         myEvent.pPilotWeight=
            (beamBunch->GetProtonCurrent()-beamBunch->GetProtonPilotCurrent())/
            beamBunch->GetProtonPilotCurrent();
      } else if(bunchType==H1BeamBunch::kPilotE) {
         // event weight is given by ratio of proton  current ratio
         myEvent.ePilotWeight=
            (beamBunch->GetElectronCurrent()-beamBunch->GetElectronPilotCurrent())/
            beamBunch->GetElectronPilotCurrent();
      }

      // find primary vertex
      TVector3 beamSpot(*beamx0,*beamy0,0.);
      bool havePrimaryVertex=false;
      TVector3 primaryVertex(beamSpot);
      bool haveSimulatedVertex=false;
      TVector3 simulatedVertex(beamSpot);
      for(int i=0;i<vertex.GetEntries();i++) {
         Int_t type=vertex[i]->GetVertexType();
         if(type==0) {
            havePrimaryVertex=true;
            primaryVertex=vertex[i]->GetPosition();
         } else if(type==1) {
            haveSimulatedVertex=true;
            simulatedVertex=vertex[i]->GetPosition();
         }
      }
      myEvent.vertexType = *ivtyp;
      if(havePrimaryVertex) {
         myEvent.vertex[0]=primaryVertex.X();
         myEvent.vertex[1]=primaryVertex.Y();
         myEvent.vertex[2]=primaryVertex.Z();
      } else {
         myEvent.vertexType=-1;
         myEvent.vertex[0]=-999.;
         myEvent.vertex[1]=-999.;
         myEvent.vertex[2]=-999.;
      }
      if(haveSimulatedVertex) {
         myEvent.simvertex[0]=simulatedVertex.X();
         myEvent.simvertex[1]=simulatedVertex.Y();
         myEvent.simvertex[2]=simulatedVertex.Z();
      } else {
         myEvent.simvertex[0]=-999.;
         myEvent.simvertex[1]=-999.;
         myEvent.simvertex[2]=-999.;
      }
      myEvent.beamSpot[0]=beamSpot.X();
      myEvent.beamSpot[1]=beamSpot.Y();
      myEvent.beamTilt[0]=*beamtiltx0;
      myEvent.beamTilt[1]=*beamtilty0;
      if(print) {
         cout<<"ivtyp="<<*ivtyp
             <<" beam spot: "<<beamSpot.X()<<" "<<beamSpot.Y();
         if(havePrimaryVertex) {
            cout<<" reconstructed primary vertex:"
                <<" "<<primaryVertex.X()
                <<" "<<primaryVertex.Y()
                <<" "<<primaryVertex.Z();
         }
         if(haveSimulatedVertex) {
             cout<<" simulated vertex:"
                 <<" "<<simulatedVertex.X()
                 <<" "<<simulatedVertex.Y()
                 <<" "<<simulatedVertex.Z();
         }
         cout<<"\n";
         cout<<"number of part cand: "<<partCandArray.GetEntries()<<"\n";
      }

      H1FloatPtr ElecE("ElecE"); //energy of scattered electron from e-finder

      // find scattered electron
      myEvent.haveScatteredElectron=0;
      TLorentzVector escat0_REC_lab;
      int scatteredElectron=-1;
      int scatteredElectronCharge=9;// 9 is default to not be confused with 0
      double ptMax=0;
      for(int i=0;i<partCandArray.GetEntries();i++) {
        H1PartCand *cand=partCandArray[i];
        H1PartEm const *elec=cand->GetIDElec();
        if(elec && cand->IsScatElec()) {
           // if (myElecCut.goodElec(elec,*run)!=1) continue;
           H1Track const *scatElecTrk=cand->GetTrack();//to match a track
           TLorentzVector p= elec->GetFourVector();
           if(p.Pt()>ptMax) {
              escat0_REC_lab = p;
              scatteredElectron=i;
              myEvent.haveScatteredElectron=1;
              if(scatElecTrk) scatteredElectronCharge=scatElecTrk->GetCharge();
              ptMax=p.Pt();
           }   
        }
      }
      //adding a charge variable to later decide if it matched to a track with its charge
      myEvent.elecChargeREC=scatteredElectronCharge;
      
      //add energy scale by 1%
      escat0_REC_lab.SetE( 1.005*escat0_REC_lab.E() );

      // add EM particles and neutrals in a cone around the electron 
      TLorentzVector escatPhot_REC_lab(escat0_REC_lab);
      set<int> isElectron;
      if(scatteredElectron>=0) {
         isElectron.insert(scatteredElectron);
         for(int i=0;i<partCandArray.GetEntries();i++) {
            if(i==scatteredElectron) continue;
            H1PartCand *cand=partCandArray[i];
            H1PartEm const *elec=cand->GetIDElec();
            if(elec) {
               TLorentzVector p= elec->GetFourVector();
               if(p.DeltaR(escat0_REC_lab)<ELEC_ISOLATION_CONE) {
                  escatPhot_REC_lab += p;
                  isElectron.insert(i);
               }
            } else if(!cand->GetTrack()) {
               TLorentzVector p= cand->GetFourVector();
               if(p.DeltaR(escat0_REC_lab)<ELEC_ISOLATION_CONE) {
                  escatPhot_REC_lab += p;
                  isElectron.insert(i);
               }
            }
         }
      }

      myEvent.elecEradREC=escatPhot_REC_lab.E()-escat0_REC_lab.E();
      myEvent.elecPxREC=escatPhot_REC_lab.X();
      myEvent.elecPyREC=escatPhot_REC_lab.Y();
      myEvent.elecPzREC=escatPhot_REC_lab.Z();
      myEvent.elecEREC=escatPhot_REC_lab.E();

      // auxillary variables: cluster radius etc
      if(scatteredElectron>=0) {
         H1PartEm const *partEM=partCandArray[scatteredElectron]->GetIDElec();
         myEvent.elecEcraREC=partEM->GetEcra();
         myEvent.elecXclusREC=partEM->GetXClus();
         myEvent.elecYclusREC=partEM->GetYClus();
         myEvent.elecThetaREC=partEM->GetTheta();
         myEvent.elecTypeREC=partEM->GetType();
         myEvent.elecEnergyREC=partEM->GetE();
         myEvent.elecEfracREC=partEM->GetEaem();
         myEvent.elecHfracREC=partEM->GetEnHadSpac();
      } else {
         myEvent.elecEcraREC=-1;
      }

      GetKinematics(ebeam_REC_lab,pbeam_REC_lab,escatPhot_REC_lab,
                    &myEvent.xREC,&myEvent.yREC,&myEvent.Q2REC);

      TLorentzRotation boost_REC_HCM=BoostToHCM(ebeam_REC_lab,pbeam_REC_lab,escatPhot_REC_lab);
      TLorentzVector q_REC_lab(ebeam_REC_lab-escatPhot_REC_lab);

      // calculate inclusive HFS 4-vector and track selection
      // exclude particles counted as electron
      TLorentzVector hfs,hfsHadSpa,hfsEMSpa;
      myEvent.nRECtrackAll=0;
      myEvent.nRECtrack=0;

      myEvent.nRECfstFitted=fstFittedTrack.GetEntries();

      vector<int> trackType(10);

      H1InclHfsIterator inclHfs;
      int nPart=inclHfs.GetEntries();
      nPart += fstFittedTrack.GetEntries();

      /*
      Start new kinematics and boost here
      */
      TLorentzVector hfs_count;//for hfs e-sigma method
      myEvent.etaMaxHfs400=-HUGE_VAL;
      myEvent.etaMaxHfs800=-HUGE_VAL;
      for(int i =0;i<inclHfs.GetEntries();i++){
         H1PartCand *cand=0;
         cand=inclHfs[i];
         TLorentzVector p;
         p=cand->GetFourVector();
         // ignore particles counted with scattered electron
         if(cand && isElectron.find(i)!=isElectron.end()) continue;

         // exclude particles close to electron
         if(myEvent.haveScatteredElectron &&
            (p.DeltaR(escat0_REC_lab)<ELEC_ISOLATION_CONE)) continue;

         if(cand) {
            // only particle candidates belong to the calibrated HFS
            hfs_count += p;

            double eta=p.Eta();
            if((p.E()>0.4)&&(eta>myEvent.etaMaxHfs400)) {
               myEvent.etaMaxHfs400=eta;
            }
            if((p.E()>0.8)&&(eta>myEvent.etaMaxHfs800)) {
               myEvent.etaMaxHfs800=eta;
            }
         }
      }

      //add energy scale by 2%
      // hfs_count.SetE(0.98*hfs_count.E());

      double sigma_REC = hfs_count.E()-hfs_count.Pz();//not use for Elec method
      
      H1MakeKine makeKin_esREC;
      makeKin_esREC.MakeESig(escat0_REC_lab.E(), escat0_REC_lab.Theta(), sigma_REC, ebeam_REC_lab.E(), pbeam_REC_lab.E());
      
      double Q2_esigma_REC = makeKin_esREC.GetQ2es();
      double y_esigma_REC = makeKin_esREC.GetYes();
      double x_esigma_REC = makeKin_esREC.GetXes();

      myEvent.Q2REC_es = Q2_esigma_REC;
      myEvent.yREC_es = y_esigma_REC;
      myEvent.xREC_es = x_esigma_REC;

      //New boost using the e-Sigma method
      TLorentzRotation boost_MC_HCM_esREC = BoostToHCM_es(ebeam_REC_lab,pbeam_REC_lab,escat0_REC_lab,Q2_esigma_REC,y_esigma_REC);
      //end new boost

      for(int i=0;i<nPart;i++) {
         H1PartCand *cand=0;
         //H1FSTTrack *fstTrack=0;
         H1FSTFittedTrack *fstTrack=0;
         TLorentzVector p;
         // if(i<partCand.GetEntries()) {
         //    cand=partCand[i];
         if(i<inclHfs.GetEntries()){
            cand=inclHfs[i];
            p=cand->GetFourVector();
   
         } else {
            //fstTrack=fstNonFittedTrack[i-partCand.GetEntries()];
            fstTrack=fstFittedTrack[i-inclHfs.GetEntries()];
            p=fstTrack->GetFourVector(M_CHARGED_PION);
         }

         // ignore particles counted with scattered electron
         if(cand && isElectron.find(i)!=isElectron.end()) continue;

         // exclude particles close to electron
         if(myEvent.haveScatteredElectron &&
            (p.DeltaR(escat0_REC_lab)<ELEC_ISOLATION_CONE)) continue;

         if(cand) {
            // only particle candidates belong to the calibrated HFS
            //add energy scale by 2%
            // p.SetE(0.98*p.E());
            hfs += p;
            if(cand->GetLArFraction()==0.) {
               // rough cuts to make sure it is really in the SpaCal
               // no muon
               // pz must be a sizable negative number
               // pt can not be non-zero
               if((!cand->IsMuon())&&
                  (p.Pz()<-0.5*p.E()) &&
                  (p.Pt()>0.02*p.E())) {
                  if(cand->GetEMFraction()<0.5) {
                     hfsHadSpa +=p;
                  } else {
                     hfsEMSpa +=p;
                  }
               }
            }
         }
         

         H1PartSelTrack const *track=0;
         if(cand) track=cand->GetIDTrack();
         if(track || fstTrack) {
            //if(myEvent.haveScatteredElectron) {
            TLorentzVector h=p;
            if(track) h=track->GetFourVector();
            double log10z=TMath::Log10((h*pbeam_REC_lab)/(q_REC_lab*pbeam_REC_lab));
            // boost to hadronic-centre-of-mass frame
            TLorentzVector hStar = boost_REC_HCM*h;
            TLorentzVector hStar2 = boost_MC_HCM_esREC*h;
            double etaStar=hStar.Eta();
            double ptStar=hStar.Pt();
            double phiStar=hStar.Phi();
            double etaStar2=hStar2.Eta();
            double ptStar2=hStar2.Pt();
            double phiStar2=hStar2.Phi();
            int vtxNHits = 0;
            int nvNHits = 0;
            int type=0;
            int charge=0;
            double chi2vtx=-1.;
            int    vtxNdf=-1;
            double chi2nv=-1.;
            int    nvNdf=-1;
            double vtxTrackLength=-1.;
            double nvTrackLength=-1.;
            double dcaPrime=-1.;
            double dz0Prime=-1.;
            float track_p = -1.; 
            float track_err_p = -1.;
               
            float startHitsRadius = -1;
            float endHitsRadius = -1;
            float trkTheta = -1;
            float chi2Trk = -1;
            int ndfTrk = -1;
            float zLengthHit = -1;
            float chi2Link = -1;
            int ndfLink = -1;
            float rZero = -1;

            if(track){
               if(track->IsCentralTrk()) type =1;
               else if(track->IsCombinedTrk()) type=2;
               else if(track->IsForwardTrk()) type =3;
               else if(track->IsFSTTrk()) type=4;
               else if(track->IsBSTTrk()) type =5;

               //momentum of track
               track_p = track->GetP();
               track_err_p = track->GetDp();
               trkTheta = track->GetTheta();
               charge=track->GetCharge();
               
               H1VertexFittedTrack const *h1track=
                  dynamic_cast<H1VertexFittedTrack const *>
                  (cand->GetTrack());
               if(h1track) {
                     
                  chi2vtx=h1track->GetFitChi2();
                  vtxNdf=h1track->GetFitNdf();
                  chi2Trk=h1track->GetChi2();
                  ndfTrk=h1track->GetNdf();
                  vtxNHits=h1track->GetNHit(H1Track::tdCJC);
                  vtxTrackLength=h1track->GetLength();
                  dcaPrime=h1track->GetDcaPrime();
                  dz0Prime=h1track->GetDz0Prime();
                  startHitsRadius=h1track->GetStartRadius();
                  endHitsRadius=h1track->GetEndRadius();
                  TVector3 vect_start_hit = h1track->GetStartHit();
                  TVector3 vect_end_hit = h1track->GetEndHit();
                  zLengthHit = vect_start_hit.z()-vect_end_hit.z();
                  
                  H1NonVertexFittedTrack const *nvtrack=
                     h1track-> GetNonVertexFittedTrack();
                  if(nvtrack) {
                     //do non vertex fitted tracks here
                  }
                  
                  H1CombinedFittedTrack const *combtrack=
                     dynamic_cast<H1CombinedFittedTrack const *>
                     (cand->GetTrack());  
                  if(track->IsCombinedTrk() ){
                     chi2Link=combtrack->GetLinkChi2();
                     ndfLink=combtrack->GetLinkNdf();
                  }
                  
                  H1ForwardFittedTrack const *fwdtrack=
                     dynamic_cast<H1ForwardFittedTrack const *>
                     (cand->GetTrack());
                  if(track->IsForwardTrk()){
                     rZero = fwdtrack->GetR0();
                  }
                  
                  H1Vertex const *v=h1track->GetVertex();
                  if(floatEqual(v->X(),myEvent.vertex[0])&&
                     floatEqual(v->Y(),myEvent.vertex[1])&&
                     floatEqual(v->Z(),myEvent.vertex[2])) {
                  } else {
                     type=0;
                  }
                  
               } else {
                  type=0;
               }
               
            } else if(fstTrack) {

               //NHits = fstTrack->GetFSTTrack()->GetNHit();
               chi2vtx=fstTrack->GetFitChi2XY()+fstTrack->GetFitChi2SZ();
               chi2nv=fstTrack->GetFSTTrack()->GetChi2XY()+
                  fstTrack->GetFSTTrack()->GetChi2XY();
               
               vtxNdf=fstTrack->GetFitNdf();
               nvNdf=fstTrack->GetFSTTrack()->GetNdfXY()+fstTrack->GetFSTTrack()->GetNdfSZ();

               // do some track selection here
               // (1) tracks shall be a primary track
               H1Vertex const *v=fstTrack->GetVertex();
               if(floatEqual(v->X(),myEvent.vertex[0])&&
                  floatEqual(v->Y(),myEvent.vertex[1])&&
                  floatEqual(v->Z(),myEvent.vertex[2])) {
                  type=4;
               }
               // (2) minimum transverse momentum of 0.1 GeV
               // if(fstTrack->GetPt()<0.1) {
               //    type=0;
               // }
               // else{ type = 4;}
               // (3) momentum vector shall be incompatible with 
               //  any other central, combined or forward track, any other
               //  HFS track
               if(type) {
                  charge=fstTrack->GetCharge();
                  TVector3 p1=fstTrack->GetMomentum();
                  TMatrix V1=fstTrack->GetMomentumCovar();
                  // for(int j=0;j<partCand.GetEntries();j++) {
                  //     H1PartCand *candJ=partCand[j];
                  for(int j=0;j<inclHfs.GetEntries();j++) {
                     H1PartCand *candJ=inclHfs[j];
                     H1PartSelTrack const *selTrackJ=candJ->GetIDTrack();
                     H1PartCand const *partCandJ=
                        selTrackJ ? (selTrackJ->GetParticle()) : 0;
                     H1Track const *trackJ=partCandJ ? partCandJ->GetTrack() : 0;
                     if(trackJ) {
                        TVector3 p2=trackJ->GetMomentum();
                        TMatrix V2=trackJ->GetMomentumCovar();
                        TMatrixD sum(V1+V2);
                        TMatrixD Vinv(TMatrixD::kInverted,V1+V2);
                        TVector3 d(p1-p2);
                        double chi2=d.Dot(Vinv*d);
                        //if(print) cout<<i<<" "<<j<<" "<<chi2;
                        if(chi2<30.) {
                           //if(print) cout<<" [reject]";
                           type=0;
                        }
                        //if(print) cout<<"\n";
                     }
                  }
               }
               // if(type) {
               //    myEvent.nRECfstSelected++;
               // }
            }
            trackType[type]++;
            if(type && (myEvent.nRECtrack<MyEvent::nRECtrack_MAX)) {
               if(print) {
                  cout<<i<<" Track "<<myEvent.nRECtrackAll
                      <<" "<<charge*type
                      <<" etaLab="<<h.Eta()
                      <<" ptLab="<<h.Pt()
                      <<" phiLab="<<h.Phi()
                      <<" ptStar="<<ptStar
                      <<" etaStar="<<etaStar
                      <<" phiStar="<<phiStar
                      <<" ptStar2="<<ptStar2
                      <<" etaStar2="<<etaStar2
                      <<" phiStar2="<<phiStar2
                      <<" log10(z)="<<log10z
                      <<" chi2vtx="<<chi2vtx
                      <<" chi2nv="<<chi2nv
                      <<"\n";
               }
               myEvent.nRECtrackAll++;
               int k=myEvent.nRECtrack;
               myEvent.typeChgREC[k]=charge*type;
               myEvent.pxREC[k]=h.X();
               myEvent.pyREC[k]=h.Y();
               myEvent.pzREC[k]=h.Z();
               myEvent.pREC[k]=track_p;
               myEvent.peREC[k]=track_err_p;
               myEvent.etaREC[k]=h.Eta();
               
               myEvent.ptStarREC[k]=hStar.Pt();
               myEvent.etaStarREC[k]=hStar.Eta();
               myEvent.phiStarREC[k]=hStar.Phi();
               myEvent.ptStar2REC[k]=hStar2.Pt();
               myEvent.etaStar2REC[k]=hStar2.Eta();
               myEvent.phiStar2REC[k]=hStar2.Phi();

               myEvent.log10zREC[k]=log10z;
               myEvent.chi2vtxREC[k]=chi2vtx;
               myEvent.chi2nvREC[k]=chi2nv;
               myEvent.vtxNdfREC[k]=vtxNdf;
               myEvent.nvNdfREC[k]=nvNdf;
               myEvent.vtxNHitsREC[k]=vtxNHits;
               myEvent.nvNHitsREC[k]=nvNHits;
               myEvent.vtxTrackLengthREC[k]=vtxTrackLength;
               myEvent.nvTrackLengthREC[k]=nvTrackLength;
               myEvent.dcaPrimeREC[k]=dcaPrime;
               myEvent.dz0PrimeREC[k]=dz0Prime;

               myEvent.startHitsRadiusREC[k]=startHitsRadius;
               myEvent.endHitsRadiusREC[k]=endHitsRadius;
               myEvent.trkThetaREC[k]=trkTheta;
               myEvent.chi2TrkREC[k]=chi2Trk;
               myEvent.ndfTrkREC[k]=ndfTrk;
               myEvent.zLengthHitREC[k]=zLengthHit;
               myEvent.chi2LinkREC[k]=chi2Link;
               myEvent.ndfLinkREC[k]=ndfLink;
               myEvent.rZeroREC[k]=rZero;

               myEvent.nucliaREC[k]=1.;
               myEvent.momREC[k]=h.Vect();
               myEvent.covREC[k].ResizeTo(3,3);
               myEvent.imatchREC[k]=-999;
               myEvent.dmatchREC[k]=-1.;
               if(fstTrack) {
                  myEvent.covREC[k]=fstTrack->GetMomentumCovar();
                  myEvent.imatchREC[k]=-1;
               } else {
                  // H1PartCand const *partCandI=track->GetParticle();
                  // H1Track const *trackI=partCandI ? partCandI->GetTrack():0;
                  H1Track const *trackI=cand->GetTrack();
                  if(trackI) {
                     myEvent.covREC[k]=trackI->GetMomentumCovar();
                     myEvent.imatchREC[k]=-1;
                  }
               }
               myEvent.nRECtrack=k+1;
            }
         }
      }

      // match MC particles and REC particles
      // (1) for each REC particle, find the best MC particle
      //    [may result in multiple REC particles matched to the same MCpart]
      //    matching is perfomed by selecting the MC particle which 
      //    gives the lowest chi**2 when comparing the momenta
      //
      //  this sets:  myEvent.dmatchREC[]  -> lowest chi**2
      //              myEvent.imatchREC[]  -> best matching MC particle
     
      if(*runtype==1){
         //only MC does matching:
         for(int iREC=0;iREC<myEvent.nRECtrack;iREC++) {
            // skip track where momentum covariance is not known
            //  -> these will never be matched
            if(myEvent.imatchREC[iREC]!=-1) continue;
            TMatrixDSym Vsym(3);
            for(int i=0;i<3;i++) {
               for(int j=0;j<3;j++) {
                  Vsym(i,j)=myEvent.covREC[iREC](i,j);
               }
            }
            TMatrixDSymEigen ODO(Vsym);
            TVectorD ev=ODO.GetEigenValues();
            //if(print) ev.Print();
            TMatrixD O=ODO.GetEigenVectors();
            TMatrixD Ot(TMatrixD::kTransposed,O);

            //TMatrixD Vinv(TMatrixD::kInverted,myEvent.covREC[iREC]);
            for(int jMC=0;jMC< myEvent.nMCtrack;jMC++) {
               TVector3 d=myEvent.momREC[iREC]- myEvent.partMC[jMC]->GetMomentum();
               TVector3 Otd=Ot*d;
               double chi2=0.;
               for(int i=0;i<3;i++) {
                  if(ev[i]>=1.E-6*ev[0]) {
                     chi2+=Otd[i]*Otd[i]/ev[i];
                  }
               }
               //double chi2simple=d.Dot(Vinv*d);
               //if(print) cout<<iREC<<" "<<jMC<<" "<<chi2<<" "<<chi2simple<<"\n";
               if((jMC==0)||(chi2<myEvent.dmatchREC[iREC])) {
                  myEvent.dmatchREC[iREC]=chi2;
                  myEvent.imatchREC[iREC]=jMC;
               }
            }
         }
         // (2) set pointers from MC to REC
         //     and remove duplicates
         for(int iREC=0;iREC<myEvent.nRECtrack;iREC++) {
            int iMC=myEvent.imatchREC[iREC];
            if(iMC<0) continue; // no match for this particle
            int jREC=myEvent.imatchMC[iMC];
            if(jREC>=0) {
               // duplicate match for this particle
               // iREC and jREC both are pointing to the same particle
               // compare matching distance
               if(myEvent.dmatchREC[jREC]<myEvent.dmatchREC[iREC]) {
                  // old match is better
                  // invalidate pointer REC->MC
                  myEvent.imatchREC[iREC]=-2-iMC;
               } else {
                  // new match is better
                  // invalidate old pointer REC->MC
                  myEvent.imatchREC[jREC]=-2-iMC;
                  // save new pointer MC->REC
                  myEvent.imatchMC[iMC]=iREC;
               }
            } 
            else {
               // save this match
               myEvent.imatchMC[iMC]=iREC;
            }
         }
         // now:
         //    myEvent.imatchMC[] points to the best matching REC particle
         //                         <0 -> inefficiency
         //    myEvent.imatchREC[] points to the best matching MC particle
         //                         <0 -> fake track

         // for matched particles, calculate extra weight for
         // nuclear interaction probability correction
         for(int iREC=0;iREC<myEvent.nRECtrack;iREC++) {
            int iMC=myEvent.imatchREC[iREC];
            int part=0;
            if(iMC>=0) {
               int pdg=myEvent.idMC[iMC];
               if(pdg<0) pdg= -pdg;
               part= (pdg==211) ? 1 : ((pdg==321) ? 2 : 0);
            }
            if(part) {
               myEvent.nucliaREC[iREC]=
                  H1NuclIACor::GetWeight
                  (part,myEvent.typeChgREC[iREC]>0 ? 1 : -1,
                   myEvent.momREC[iREC].Pt(),
                   myEvent.momREC[iREC].Phi(),myEvent.momREC[iREC].Theta(),
                   0.0 /* dca */,H1SelVertex::GetPrimaryVertex()->Z());
            }
         }

         if(print) {
            for(int iREC=0;iREC<myEvent.nRECtrack;iREC++) {
               if(myEvent.imatchREC[iREC]>=0) {
                  cout<<"REC track "<<iREC<<" is matched to MC particle "
                      << myEvent.imatchREC[iREC]<<" dmatch="
                      <<myEvent.dmatchREC[iREC]
                      <<" nuclIA="<<myEvent.nucliaREC[iREC]
                      <<"\n";
               }
            }
            for(int iREC=0;iREC<myEvent.nRECtrack;iREC++) {
               if(myEvent.imatchREC[iREC]<0) {
                  cout<<"REC track "<<iREC<<" NOT matched "
                      << myEvent.imatchREC[iREC]<<" dmatch="
                      <<myEvent.dmatchREC[iREC]
                      <<" nuclIA="<<myEvent.nucliaREC[iREC]
                      <<"\n";
               }
            }
         }
      }

      myEvent.hfsPxREC=hfs.X();
      myEvent.hfsPyREC=hfs.Y();
      myEvent.hfsPzREC=hfs.Z();
      myEvent.hfsEREC=hfs.E();

      myEvent.hfsPxHadSpa=hfsHadSpa.X();
      myEvent.hfsPyHadSpa=hfsHadSpa.Y();
      myEvent.hfsPzHadSpa=hfsHadSpa.Z();
      myEvent.hfsEHadSpa=hfsHadSpa.E();

      myEvent.hfsPxEMSpa=hfsEMSpa.X();
      myEvent.hfsPyEMSpa=hfsEMSpa.Y();
      myEvent.hfsPzEMSpa=hfsEMSpa.Z();
      myEvent.hfsEEMSpa=hfsEMSpa.E();

      if(myEvent.haveScatteredElectron && print) {
         cout<<"reconstructed electron w/o photons in lab: ";
         escat0_REC_lab.Print();
         cout<<"reconstructed electron with photons in lab: ";
         escatPhot_REC_lab.Print();
      }

      if(print) {
         for(size_t type=0;type<trackType.size();type++) {
            cout<<" "<<trackType[type];
         }
         cout<<"\n";
      }

      if(print) {
         print--;
      }

      // main analysis cut
      //  keep all MC events
      //  keep trigger S0 : electron in SpaCal, for DIS selection
      //  keep trigger S82 : electron in SpaCal, for photoproduction selection
      if((*runtype==1)||  // MC: keep all events
         (havePrimaryVertex&&myEvent.haveSpacalTrigger&&
          myEvent.haveScatteredElectron)
         ||
         (havePrimaryVertex&&myEvent.haveEtag6Trigger)) {
         output->Fill();
      }
   }

    // Summary
    cout << "\nProcessed " << eventCounter
        << " events\n\n";
    cerr << "\nProcessed " << eventCounter
       << " events\n\n";

    // Write histogram to file
    output->Write();

    /*h_dPhi_theta_noR->Write();
    h_dPhi_theta_ISR->Write();
    h_dPhi_theta_FSR->Write();

    h_Q2diff->Write();
    h_Xdiff->Write();
    h_Ydiff->Write();

    h_ISR_Q2diff->Write();
    h_ISR_Xdiff->Write();
    h_ISR_Ydiff->Write();

    h_FSR_Q2diff->Write();
    h_FSR_Xdiff->Write();
    h_FSR_Ydiff->Write();

    h_noR_Q2diff->Write();
    h_noR_Xdiff->Write();
    h_noR_Ydiff->Write(); */

    //file.Close();
    delete file;

    cout << "Histograms written to " << opts.GetOutput() << endl;
    cerr << "Histograms written to " << opts.GetOutput() << endl;

    return 0;
}
