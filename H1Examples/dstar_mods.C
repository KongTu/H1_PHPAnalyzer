
///////////////////////////////////////////////////////
// Search for D* candidates on MODS
//
// Author     : Gero Flucke (flucke@mail.desy.de)
// Created    : 23.1.2002
// last update: $Date: 2005/01/07 12:03:02 $
// by         : $Author: ameyer $
//
///////////////////////////////////////////////////////
#include <stdlib.h>
#include <iostream>
// ROOT-header:
#include <TFile.h>
#include <TH1.h>
#include <TROOT.h>
#include <TString.h>
#include <TDatabasePDG.h>
#include <TMath.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TPostScript.h>
#include <TCanvas.h>
// H1 header:
#include "H1Skeleton/H1Tree.h"
#include "H1Mods/H1PartSelTrack.h" // because H1PartDstar::GetKaon() returns this type
#include "H1Mods/H1PartDstar.h"
#include "H1Mods/H1PartDstarArrayPtr.h"
#include "H1Steering/H1StdCmdLine.h"

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
    TApplication theApp("dstar_mods", &argc, argv);
    H1Tree::Instance()->Open(); //must be there!
    
    // cuts:
    Double_t ptCut = 2.0;    // pt-cut on D*-candidate
    Double_t ptKCut = 0.5;
    Double_t ptPiCut = 0.3;
    Double_t mD0Cut = 0.08;  // cut on |m_{D0}-m_{Kpi}|

    // D0 mass:
    const Float_t mD0 = TDatabasePDG::Instance()->GetParticle(421)->Mass();

    // book histograms:
    TH1F* dmHist = new TH1F("dmHist", "#muODS: #Delta M (D*)",
                               35, 0.135,  0.17);
    TH1F* d0Hist = new TH1F("d0Hist", "#muODS: D^{0} mass peak of D* candidates",
                                  25, mD0-0.25,  mD0+0.25);
    TH1F* numDsHist = new TH1F("numDsHist", "N(D*) candidates per event on #muODS", 9, .5, 9.5);

    // start event selection		    
    cout << "Starting HAT selection: NumDstar > 0" << endl;
    cout << H1Tree::Instance()->SelectHat("NumDstar > 0") << " events selected!" << endl;

    H1PartDstarArrayPtr dstars;

    Int_t events = 0;
    while (H1Tree::Instance()->Next() && (!opts.IsMaxEvent(events))) {

        ++events;
        if (events % 5000 == 0) {
            cout <<"event " << events <<" **********\n";
        }
	// If you use an H1..ArrayPtr inside the event loop, it should be 'static':
	//static H1PartDstarArrayPtr dstars; 

        Int_t numDstars = dstars.GetEntries();
        numDsHist->Fill(numDstars);

        for (Int_t i = 0; i < numDstars; ++i) {
	  if ((dstars[i]->GetPt() > ptCut)
	      && (dstars[i]->GetKaon()->GetPt() > ptKCut)
	      && (dstars[i]->GetPion()->GetPt() > ptPiCut)){

	    Double_t dm = dstars[i]->GetDm();
	    if(dm < 0.148 && dm > 0.143){
	      d0Hist->Fill(dstars[i]->GetD0Mass());
	    }
	    if(TMath::Abs(dstars[i]->GetD0Mass() - mD0) < mD0Cut){
	      dmHist->Fill(dm);
	    }
	  }
	}// loop over dstars
    }

    // put plots into postscript file
    TPostScript ps ("dstar_mods.ps", 111);

    TCanvas* canvas = new TCanvas("dstar", "mass", 600, 800);
    canvas->Clear();
    canvas->Divide(1,2);
    canvas->cd(1); dmHist->Draw();
    canvas->cd(2); d0Hist->Draw();
    canvas->Update();
    
    ps.Close();
    
    // open histograms and file to store them:
    // (if the file is opened after booking of the TH1F,
    //  they will be written to the file if we do file->Write()
    //  and hist->Write() is not necessary!)
    TFile* file = new TFile(opts.GetOutput(), "RECREATE");

    d0Hist->Write(); // write histogram to file
    dmHist->Write(); // write histogram to file
    
    cout << "\nProcessed " << events
        << " events with D* candidates and wrote histos to file "
        << opts.GetOutput()  << "." << endl;
    file->Close();

}
