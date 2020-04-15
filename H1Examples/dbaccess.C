///////////////////////////////////////////////////////
// example for access to database information
//    so far only access to objects written to the snaphot is shown
//
// Author     : Gero Flucke
// Created    : 8.9.2003
// Last update: $Date: 2008/02/04 16:59:04 $
//          by: $Author: aziza $
//
///////////////////////////////////////////////////////

#include <iostream>
#include <stdlib.h>

// ROOT includes
#include <TFile.h>
#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TApplication.h>

// H1 OO includes
#include "H1Skeleton/H1Tree.h"
#include "H1Pointers/H1FloatPtr.h"
#include "H1Steering/H1StdCmdLine.h"
#include "H1Geom/H1DBManager.h"
#include "H1Geom/H1ElectronTagger.h"
#include "H1Geom/H1ElectronTagger6.h"
#include "H1Geom/H1DBBankPtr.h"
#include "H1Geom/H1Constants.h"


using namespace std;

int main(int argc, char* argv[])
{
    // count the events processed
    Int_t eventCounter = 0;
    
    // parse the command line
    H1StdCmdLine opts;
    opts.Parse(&argc, argv);
    // needed for graphic, but MUST be AFTER Parse(...):
    TApplication theApp("clusters_ods", &argc, argv);
    
    // Load HAT files
    H1Tree::Instance()->Open();            // this statement must be here!

    // Define histogram
    TH1 *hYh = new TH1F("yh", "y_{jb}", 50, 0., 1.);
    TH2 *hAcc = new TH2F("accept", "Aceptance", 50, 0., 1., 50, 0., 1.);

    // The shortcut for the H1Constants::Instance()
    H1Constants* constant = H1Constants::Instance();

    // Make a link to HAT-var (electron at 33 meter HERA I, electron at 6 meter  HERA II)
    H1FloatPtr e33("Etag33");
    H1FloatPtr e6("Etag6");

    // open the canvas before the event loop
    TCanvas *canvas = new TCanvas("ods", "Plots", 10, 10, 600, 800);
    canvas->Divide(1, 2);

    // All objects written to the snapshot file inherit from H1DBEntry.
    // You must (!) access them via H1DBManager, but than you will 
    // not have to care about the range of their validity: the framework takes care of it!
    H1ElectronTagger *tag33 = static_cast<H1ElectronTagger*>
        (H1DBManager::Instance()->GetDBEntry(H1ElectronTagger::Class()));
    H1ElectronTagger6 *tag6 = static_cast<H1ElectronTagger6*>
        (H1DBManager::Instance()->GetDBEntry(H1ElectronTagger6::Class()));
    
    cout << "\nStart looping over the " << H1Tree::Instance()->GetEntries()
         << " selected events\n" << endl;
    
    cout << "Processing event " << eventCounter << endl;

    cout << "\nThere are 13 Run Periods, they are: " << endl <<
        "\n*************************" <<
        "\n* eEplus94 == 0         *" <<
        "\n* eEplus95 == 1         *" <<
        "\n* eEplus96 == 2         *" <<
        "\n* eEplus97 == 3         *" <<
        "\n* eEminus9899 == 4      *" <<
        "\n* eEplus9900 == 5       *" <<
        "\n* eEplus0203 == 6       *" <<
        "\n* eEplus0304 == 7       *" <<
        "\n* eEminus0405 == 8      *" <<
        "\n* eEminus06 == 9        *" <<
        "\n* eEplus0607 == 10      *" <<
        "\n* eEplus07lowE460 == 11 *" <<
        "\n* eEplus07lowE575 == 12 *" <<
        "\n*************************" << endl;
    

    while (H1Tree::Instance()->Next() && !opts.IsMaxEvent(eventCounter)) {
        if (eventCounter == 0){

            cout << "\nThe Run Period you have chosen in the steering file is "
                 << constant->RunToPeriod(H1Tree::Instance()->GetRunNumber()) << endl;
            cout << "\nThe Electron Beam Energy related to this Run Period is "
                 << constant->GetElectronBeamEnergy() << endl << endl;
        }

        if(constant->RunToPeriod(H1Tree::Instance()->GetRunNumber()) <= 5){

            if ( *e33 > 0.1 ) {
                Float_t yh = 1.-*e33/constant->GetElectronBeamEnergy();
                hYh->Fill(yh);           // H1FloatPtr have to be dereferenced
                hAcc->Fill(yh, tag33->Acceptance33(yh)); // here we call a method of the snapshot object
            }

        } else if(constant->RunToPeriod(H1Tree::Instance()->GetRunNumber()) > 5){

            if ( *e6 > 0.1 ) {
                Float_t yh = 1.-*e6/constant->GetElectronBeamEnergy();
                hYh->Fill(yh);          // H1FloatPtr have to be dereferenced
                hAcc->Fill(yh, tag6->Acceptance6(yh)); // here we call a method of the snapshot object
            }

        }

        eventCounter++;
        
        if (eventCounter % 10000 == 0) {

            cout << "***** Processing ODS event " << eventCounter << " *****" <<  endl;

            canvas->cd(1);
            hYh->Draw();
            gPad->Update();            
            canvas->cd(2);
            hAcc->Draw();
            gPad->Update();
        }
        
        if (opts.IsMaxEvent(eventCounter)) break;
    }
    
    cout << "\n" << eventCounter << " events processed." << endl;

    canvas->cd(1);
    hYh->Draw();
    gPad->Update();            
    canvas->cd(2);
    hAcc->Draw();
    gPad->Update();
    
    //
    // Write histograms to file
    //
    TFile file(opts.GetOutput(), "RECREATE");
    hYh->Write();
    hAcc->Write();
    file.Write();
    file.Close();
    cout << "\nHistograms written to " << opts.GetOutput() << endl;
 
    return 0;
}
