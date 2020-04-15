///////////////////////////////////////////////////////
// Read E-P_z from HAT
//
// Author     : Bengt Wessling
// Created    : 15.1.2002
// Last update: $Date: 2006/09/22 09:00:40 $
//          by: $Author: ozerov $
//
///////////////////////////////////////////////////////

#include <iostream>
#include <stdlib.h>

// ROOT includes
#include <TFile.h>
#include <TROOT.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TApplication.h>

// H1 OO includes
#include "H1Skeleton/H1Tree.h"
#include "H1Pointers/H1FloatPtr.h"
#include "H1Steering/H1StdCmdLine.h"

using namespace std;

int
main(int argc, char* argv[])
{
    // parse the command line
    H1StdCmdLine opts;
    opts.Parse(&argc, argv);
    // needed for graphic, but MUST be AFTER Parse(...):
    TApplication theApp("empz_hat", &argc, argv);

    // Load HAT files
    H1Tree::Instance()->Open();            // this statement must be here!

    cout << "Starting HAT selection: Q2e > 2." << endl;
    H1Tree::Instance()->SelectHat("Q2e > 2.");
    cout << "HAT selection finished." << endl;

    // Define histogram
    TH1F *hEmpz = new TH1F("Epz", "Epz", 100, 0., 100.);

    // Make a link to HAT-vars
    H1FloatPtr epz("Epz");    // E-p_z of all particles in main detector

    cout << "\nStart looping over the "
         << H1Tree::Instance()->GetEntries()
         << " selected events\n" << endl;

    // open the canvas before the event loop
    TCanvas *canvas = new TCanvas("Empz", "Empz");

    // Loop over events
    Int_t eventCounter = 0;
    while (H1Tree::Instance()->Next() && (!opts.IsMaxEvent(eventCounter)) ) {
        hEmpz->Fill(*epz);           // H1FloatPtr have to be dereferenced
        ++eventCounter;
        // draw the histogram every 10.000 events
        if (eventCounter % 10000 == 0) {
            cout << "Processing event " << eventCounter << endl;
            hEmpz->Draw();
            gPad->Update();
        }
    }

    
    //
    // Write histograms to file
    //
    TFile file(opts.GetOutput(), "RECREATE");
    hEmpz->Write();
    file.Write();
    file.Close();
    cout << "\nHistogram written to " << opts.GetOutput() << endl;
    
    hEmpz->Draw();
    
    return 0;
}
