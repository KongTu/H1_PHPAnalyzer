///////////////////////////////////////////////////////
// Search for J/Psi on MODS
//
// Author     : Ingo Strauch (strauch@mail.desy.de)
// Created    : 19.12.2000
// Redone by  : Gero Flucke (gero.flucke@desy.de)
//          and Bengt Wessling (bengt.wessling@desy.de)
// Last update: $Date: 2010/10/08 13:23:57 $
//          by: $Author: msteder $
//
///////////////////////////////////////////////////////

#include <iostream>
#include <stdlib.h>

// ROOT includes
#include <TFile.h>
#include <TROOT.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TApplication.h>
#include <TStyle.h>

// H1 OO includes
#include "H1Skeleton/H1Tree.h"
#include "H1Mods/H1PartSelTrack.h"
#include "H1Mods/H1PartJPsi.h"
#include "H1Mods/H1PartJPsiArrayPtr.h"
#include "H1Mods/H1PartCand.h"
#include "H1Mods/H1PartMuon.h"
#include "H1Steering/H1StdCmdLine.h"

using namespace std;

int
main(int argc, char* argv[])
{
    // parse the command line
    H1StdCmdLine opts;
    opts.Parse(&argc, argv);
    // needed for graphic, but MUST be AFTER Parse(...):
    TApplication theApp("jpsi_mods", &argc, argv);

    gROOT->SetStyle("Plain");
    gStyle->SetHistLineWidth(2);

    // Load mODS/HAT files
    H1Tree::Instance()->Open();            // this statement must be there!

    cout << H1Tree::Instance()->SelectHat("NumJPsi>0")
        << " events selected " << endl;

    TCanvas* canvas = new TCanvas("jpsi", "mass", 600, 700);
    canvas->Divide(1, 3);

    TFile file(opts.GetOutput(), "RECREATE");
    // Define histograms
    TH1F* hJpsiElasticTT = new TH1F("JPsiElasticTT", "#muODS: m_{J/#Psi}: elastic production, track-track (#mu-#mu)", 40, 2., 4.);
    TH1F* hJpsiElasticTC = new TH1F("JPsiElasticTC", "#muODS: m_{J/#Psi}: elastic production, track-cluster", 40, 2., 4.);
    TH1F* hJpsiInelasticTT = new TH1F("JPsiInelasticTT", "#muODS: m_{J/#Psi}: inelastic production, track-track (#mu-#mu)", 40, 2., 4.);


    // Create branch pointer to branch H1PartJPsi
    static H1PartJPsiArrayPtr jpsi;

    Int_t eventCounter = 0;

    // Loop over events
    while (gH1Tree->Next() && !opts.IsMaxEvent(eventCounter)) {
        ++eventCounter;
        for (Int_t i = 0 ;i < jpsi.GetEntries(); ++i) {
            // get the J/Psi's invariant mass. The H1PartJPsi will
            // figure out by itself wether it has to use the muon or
            // the electron mass hypothesis. (If you don't like that,
            // pass the PDG code, 11 or 13, to GetMass()).
            const Float_t mass = jpsi[i]->GetMass();

            if (jpsi[i]->IsElastic() == kTRUE) {
                if (jpsi[i]->IsTrackTrack() == kTRUE) {
                    // both tracks have to be muons (calo or iron) - we want to see a peak
                    // + reject cosmics
                    if (jpsi[i]->GetTrack1()->GetParticle()->IsMuon() &&
                        jpsi[i]->GetTrack2()->GetParticle()->IsMuon() &&
                        jpsi[i]->IsCosmic() == kFALSE) {
                        hJpsiElasticTT->Fill(mass);
                    }
                } else {
                    hJpsiElasticTC->Fill(mass);
                }
            } else {
                // require two iron muons so that we can actually see a peak
                if (jpsi[i]->GetTrack1()->GetParticle()->IsMuon() &&
                    jpsi[i]->GetTrack1()->GetParticle()->GetIDMuon()->IsIronMuon() &&
                    jpsi[i]->GetTrack2()->GetParticle()->IsMuon() &&
                    jpsi[i]->GetTrack2()->GetParticle()->GetIDMuon()->IsIronMuon()){
                    hJpsiInelasticTT->Fill(mass);
                }
            }
        }
        if (eventCounter % 5000 == 0) {
            cout << "event " << eventCounter << " **********" << endl;
            canvas->cd(1);
            hJpsiElasticTT->Draw();
            canvas->cd(2);
            hJpsiElasticTC->Draw();
            canvas->cd(3);
            hJpsiInelasticTT->Draw();
            canvas->cd();
            gPad->Update();
        }
    }
    // Summary
    cout << "\nProcessed " << eventCounter
        << " events with J/Psi candidates\n\n";


    // Write histogram to file
    file.Write();
    file.Close();


    cout << "Histogram written to " << opts.GetOutput() << endl;

    return 0;
}
