///////////////////////////////////////////////////////
// Example program how to access jets that were found 
// in different reference frames
//
// Author     : Roman Kogler (roman.kogler@desy.de)
// Created    : 10.1.2008
// last update: 
// by         : 
//
///////////////////////////////////////////////////////
#include <stdlib.h>
#include <iostream>
// ROOT-header:
#include <TFile.h>
#include <TH1.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TObjArray.h>
#include <TApplication.h>
// H1 header:
#include "H1Skeleton/H1Tree.h"
#include "H1Steering/H1StdCmdLine.h"
#include "H1Pointers/H1FloatPtr.h"
#include "H1PhysUtils/H1BoostedJets.h"
#include "H1Steering/H1ErrorHandler.h"

using namespace std;

int main(int argc, char* argv[])
{
    // Some general cosmetics:
    gROOT->SetStyle("Plain");
    gStyle->SetHistLineWidth(2);

    // parse the command line to give the option, e.g. nr of events to 
    // be processed with dstar_mods -n 1000 
    // the 2 lines are not needed if no cmd line options are given
    H1StdCmdLine opts;
    opts.Parse(&argc, argv);
    
    // needed for graphic, but MUST be AFTER Parse(...):
    TApplication theApp("boosted_jets", &argc, argv);

    H1Tree::Instance()->Open(); //must be there!

    // needed kinematics for the boost:
    H1FloatPtr Q2("Q2e");     // viruality
    H1FloatPtr Y("Ye");       // inelasticity
    H1FloatPtr Q2Gen("Q2eGen"); // generated viruality
    H1FloatPtr YGen("YeGen");   // generated inelasticity
    H1FloatPtr Ee("EBeamE");  // electron beam energy
    H1FloatPtr Ep("EBeamP");  // proton beam energy
 
    // electron quantities needed for the boost:
    H1FloatPtr ElecE("ElecE");           // energy of scattered electron
    H1FloatPtr ElecTheta("ElecTheta");   // theta of scattered electron
    H1FloatPtr ElecPhi("ElecPhi");       // phi of scattered electron

    H1BoostedJets* boostedjets = H1BoostedJets::Instance();
 
    H1ErrorHandler::Instance()->SetMaxCount(5); // mop up those error messages

    // some settings for the boosted jets - play with them!
    //    boostedjets->SetCalibrationMethod(H1BoostedJets::eHighPtJetsOnly);
    boostedjets->SetReferenceFrame(H1BoostedJets::eBreit);
    boostedjets->UseFastJet();
    boostedjets->SetFastJetFinder(H1PartJet::eAntiKt);
    boostedjets->SetJetFinderRecom(H1PartJet::ePt);

    // if you would like to use an inclusive jet sample, where each jet 
    // should fulfill the requirements 
    //                   pt_min < pt_boostedjet < pt_max 
    //                               AND 
    //                eta_lab_min < eta_labjet < eta_lab_max 
    // you can use the CutFlags:
    boostedjets->SetPtCut(7.0, 50.0); 
    boostedjets->SetEtaLabCut(-1.0, 2.5);
    

    // open file to store histograms:
    TFile* file = new TFile(opts.GetOutput(), "RECREATE"); 
  

    // book some jet histograms
    TH1F* pthisto = new TH1F("pthisto", "P_{T} of inclusive jets in boosted frame", 50, 0, 50);
    TH1F* etahisto = new TH1F("etahisto", "#eta of inclusive jets in laboratory rest frame", 50, -1.5, 3);
    TH1F* jetmultiplicity = new TH1F("jetmultiplicity", "Number of jets after cuts", 6, -0.5, 5.5);

    // open the canvas before the event loop
    TCanvas *canvas = new TCanvas("boosted_jets", "Plots", 10, 10, 600, 800);
    canvas->Divide(1, 3);

    // start event selection
    //cout << "Starting HAT selection: Q2 > 150" << endl;
    //cout << H1Tree::Instance()->SelectHat("Q2e > 150 && (Ye>0.2 && Ye<0.7)") << " events selected!" << endl;

    
    Int_t events = 0;
    while (H1Tree::Instance()->Next() && (!opts.IsMaxEvent(events))) {

      // first thing to do in the eventloop:
      // reset the boosted jets 
      boostedjets->Reset();
      
      // and set reconstructed variables for the boost (must be there if a boost is used)
      // choose your preferred way of calculating these quantities:
      Float_t s = 4 * (*Ep) * (*Ee);
      Float_t xbj = (*Q2)/((*Y) * s);

      Float_t ElecPx = *ElecE * TMath::Sin(*ElecTheta)*TMath::Cos(*ElecPhi);
      Float_t ElecPy = *ElecE * TMath::Sin(*ElecTheta)*TMath::Sin(*ElecPhi);
      Float_t ElecPz = *ElecE * TMath::Cos(*ElecTheta);
      TLorentzVector ScatteredElec(ElecPx, ElecPy, ElecPz, *ElecE);
 
      boostedjets->SetXandScatElecVect(xbj,ScatteredElec);
      
      // if generator information is available: set generated variables for the boost
      Float_t xbjGen = (*Q2Gen)/((*YGen) * s);
      boostedjets->SetXGen(xbjGen);

      

      // level of jet finding (reconstructed, hadron or parton)
      H1BoostedJets::JetType level = H1BoostedJets::eRecLev;
	
      // get the jets in the boosted frame:
      TObjArray* boostedjetarray = boostedjets->GetBoostedJets(level);

      // get the jets boosted back to the lab frame:
      TObjArray* labjetarray = boostedjets->GetLabJets(level);

      // boolean flags to see which jets fulfill the cuts:
      Bool_t* cutflags = boostedjets->GetCutFlags(level);

      // this is the number of jets found without cuts:
      Int_t NumJets = boostedjets->GetNumJets(level);


      // fill the histograms:
      for (Int_t ijet = 0; ijet<NumJets; ++ijet){
	H1PartJet* jet = (H1PartJet*) boostedjetarray->At(ijet); 
	H1PartJet* labjet = (H1PartJet*) labjetarray->At(ijet);
	if (cutflags[ijet]){
	  pthisto->Fill(jet->GetPt());
	  etahisto->Fill(labjet->GetEta());
	}	  
      }
	
      // this is the number of jets found after cuts:
      Int_t NumJetsAfterCuts = boostedjets->GetNumJetsAfterCuts(level);
      jetmultiplicity->Fill(NumJetsAfterCuts);

      
      if (events % 1000 == 0) {
	cout <<"event " << events <<" **********\n";
        canvas->cd(1);
        pthisto->Draw();
        //gPad->SetLogy();
        gPad->Update();
        canvas->cd(2);
        etahisto->Draw();
        //gPad->SetLogy();
        gPad->Update();
        canvas->cd(3);
        jetmultiplicity->Draw();
        gPad->SetLogy();
        gPad->Update();
      }
      ++events;
      if (opts.IsMaxEvent(events)) break;
    }

    canvas->cd(1);
    pthisto->Draw();
    //gPad->SetLogy();
    gPad->Update();
    canvas->cd(2);
    etahisto->Draw();
    //gPad->SetLogy();
    gPad->Update();
    canvas->cd(3);
    jetmultiplicity->Draw();
    gPad->SetLogy();
    gPad->Update();
    
    
    boostedjets->Print("Example Program");

    cout << "\nProcessed " << events
	 << " events and wrote jet histos to file "
	 << opts.GetOutput()  << "." << endl;

    pthisto->Write();
    etahisto->Write();
    jetmultiplicity->Write();
    file->Write();
    file->Close();
}
