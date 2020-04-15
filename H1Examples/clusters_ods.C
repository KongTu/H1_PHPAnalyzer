///////////////////////////////////////////////////////
// Read clusters and DELE bank on ODS
//
// Authors    : ameyer@mail.desy.de,
//              Gero Flucke (gero.flucke@desy.de),
//              Bengt Wessling (bengt.wessling@desy.de)
// Created    : 28.11.2001
// Last update: $Date: 2005/06/10 09:07:28 $ (UTC)
//          by: $Author: katzy $
//
///////////////////////////////////////////////////////

#include <iostream>
#include <stdlib.h>

// ROOT includes
#include <TFile.h>
#include <TROOT.h>
#include <TH1.h>
#include <TClonesArray.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TApplication.h>

// H1 OO includes
#include "H1Skeleton/H1Tree.h"
#include "H1OOBanks/ODS/H1Dele.h"
#include "H1OOBanks/ODS/H1DeleBankPtr.h"
// cluster-headers
#include "H1Clusters/H1Cluster.h"
#include "H1Clusters/H1Cell.h"
#include "H1Clusters/H1ClusterArrayPtr.h"
#include "H1Geom/H1CaloGeometry.h"
#include "H1Geom/H1CellGeometry.h"
#include "H1Steering/H1StdCmdLine.h"
#include "H1Geom/H1LArCellGeo.h"

using namespace std;

int
main(int argc, char* argv[])
{
    // count the events processed
    Int_t eventCounter = 0;

    // parse the command line
    H1StdCmdLine opts;
    opts.Parse(&argc, argv);
    // needed for graphic, but MUST be AFTER Parse(...):
    TApplication theApp("clusters_ods", &argc, argv);

    // Load mODS/HAT files and open all files    
    H1Tree::Instance()->Open();            // this statement must be here

    // Histograms to fill
    TH1F *hDeleE = new TH1F("DeleE", "DeleE", 100, 0., 50.);
    TH1F *hClusterE = new TH1F("ClusterE", "ClusterE", 100, 0., 50.);

    static H1DeleBankPtr dele;
    static H1ClusterArrayPtr clusters;// it is an H1MultiArrayPtr

    // open the canvas before the event loop
    TCanvas *canvas = new TCanvas("ods", "Plots", 10, 10, 600, 800);
    canvas->Divide(1, 2);

    while (H1Tree::Instance()->Next() && !opts.IsMaxEvent(eventCounter)) {
        // DELE Bank
        if (dele.GetEntries() > 0)
            hDeleE->Fill(dele[0]->GetE_el());
        // Clusters
        if (clusters.GetEntries() > 0) {
	    TObjArray *myClusterList = clusters.CreateArray();
            myClusterList->Sort();// sort clusters according to energy
	    H1Cluster *hottestClus = static_cast<H1Cluster*>(myClusterList->At(0));
            hClusterE->Fill(hottestClus->GetEfinal());

            const Int_t fNCells = hottestClus->GetNCells();
	    for (Int_t iCell = 0; iCell < fNCells; iCell++) {
		   H1Cell* cell = hottestClus->GetCell(iCell);
		   Float_t cellEnergy = cell->GetEfinal();
		   Int_t h1CellNumber = cell->GetNumber();
		   TVector3 cellPos   = cell->GetPosition();
	    }
	    delete myClusterList;
	}

	
        eventCounter++;
        // redraw the histograms every 1000 events
        // and write a message about it to the screen
        if (eventCounter % 1000 == 0) {
            cout << "Processing ODS event " << eventCounter << endl;

            canvas->cd(1);
            hDeleE->Draw();
            gPad->Update();            
            canvas->cd(2);
            hClusterE->Draw();
            gPad->Update();
        }
    }

    cout << "\n" << eventCounter << " events processed." << endl;

    canvas->cd(1);
    hDeleE->Draw();
    canvas->cd(2);
    hClusterE->Draw();
    gPad->Update();

//
// Write histograms to file
//
    TFile file(opts.GetOutput(), "RECREATE");
    hClusterE->Write();
    hDeleE->Write();
    file.Write();
    file.Close();
    cout << "Histograms written to " << opts.GetOutput() << endl;

    return 0;
}
