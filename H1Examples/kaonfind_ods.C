///////////////////////////////////////////////////////
// K0 finder on ODS
//
// Author     : wessling
// Created    : 2002/01/23
// Last update: $Date: 2006/09/22 09:00:40 $ (UTC)
//          by: $Author: ozerov $
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
#include <TDatabasePDG.h>
#include <TApplication.h>

// H1 OO includes
#include "H1Skeleton/H1Tree.h"
// tracks
#include "H1Tracks/H1Track.h"
// vertices
#include "H1Tracks/H1CentralVertex.h"
#include "H1Tracks/H1CentralVertexArrayPtr.h"
#include "H1Steering/H1StdCmdLine.h"

using namespace std;

int
main(int argc, char* argv[])
{
    // event counter
    Int_t eventCounter = 0;

    // parse the command line
    H1StdCmdLine opts;
    opts.Parse(&argc, argv);
    // needed for graphic, but MUST be AFTER Parse(...):
    TApplication theApp("kaonfind_ods", &argc, argv);

    // Load mODS/HAT files
    H1Tree::Instance()->Open();            // this statement must be here

    // Histograms to fill
    TH1F *hKaon = new TH1F("Kaon", "Kaon", 100, 0.3, 1.0);
    TCanvas *canvas = new TCanvas("ODS", "Plot", 10, 10, 800, 400);

    // define radial vertex cuts
    const Float_t minDca = 2.;
    const Float_t maxDca = 40.;
    const Float_t lowerBeamPipe = 4.;
    const Float_t upperBeamPipe = 5.5;

    // define cuts on tracks
    const Float_t minPtPion = 0.4;
    const Float_t minPtKZero = 0.15;
    const Float_t minTracklenPion = 15;
    const Float_t ptrelCut = 0.15;

    // Pointer to vertices to loop over
    static H1CentralVertexArrayPtr vertex;

    // Get the pion's mass
    Double_t mPion = TDatabasePDG::Instance()->GetParticle(211)->Mass();

    // Loop as long as there's data
    while (H1Tree::Instance()->Next() && !opts.IsMaxEvent(eventCounter) ) {

      // Loop over all vertices found in the event
        for (Int_t loop = 0; loop < vertex.GetEntries(); loop++) {

            // In the following statements, some checks regarding
            // vertex type,
            // vertex position (exclude beam pipe),
            // number of mother and daughter particles and
            // momentum properties
            // are performed. If it's all ok, cut for pTrel
            // to exclue Lambdas and fill in the invariant mass.

            if (vertex[loop]->GetType() == H1Vertex::vtDecay
                && ((vertex[loop]->GetPosition().Perp() > minDca
                     && vertex[loop]->GetPosition().Perp() < lowerBeamPipe)
                    || (vertex[loop]->GetPosition().Perp() > upperBeamPipe
                        && vertex[loop]->GetPosition().Perp() < maxDca))) {

                const TObjArray* daughters = vertex[loop]->GetDaughters();
                if (daughters->GetEntries() == 2) {
                    const TObjArray* mothers = vertex[loop]->GetMothers();

                    if (mothers->GetEntries() != 1) {
                        // that shouldn't happen
                        break;
                    }

                    H1Track* firstDaughter = (H1Track* )daughters->At(0);
                    H1Track* secondDaughter = (H1Track* )daughters->At(1);

                    if (firstDaughter->GetCharge() != (-1) * secondDaughter->GetCharge()) {
                        // DST bug: Link from vertex fitted track
                        // to non-vertex fitted track missing
                        break;
                    }

                    if ((firstDaughter->GetPt() < minPtPion
                         || secondDaughter->GetPt() < minPtPion)
                        || ((H1Track*) mothers->At(0))->GetPt() < minPtKZero
                        || firstDaughter->GetLength() < minTracklenPion
                        || secondDaughter->GetLength() < minTracklenPion) {
                        break;
                    }

                    Double_t pTrelOne = (firstDaughter->GetFourVector(mPion))
                        .Perp(firstDaughter->GetFourVector(mPion).Vect()
                              + secondDaughter->GetFourVector(mPion).Vect());

                    Double_t pTrelTwo = (secondDaughter->GetFourVector(mPion))
                        .Perp(firstDaughter->GetFourVector(mPion).Vect()
                              + secondDaughter->GetFourVector(mPion).Vect());

                    /*
                     * Alternatively, the last lines could have been written as:
                     *
                     * TVector3 vecPionOne = firstDaughter->GetFourVector(mPion).Vect();
                     * TVector3 vecPionTwo = secondDaughter->GetFourVector(mPion).Vect();
                     * TVector3 pKaon = vecPionOne + vecPionTwo;
                     *
                     * Double_t pTrelOne = pKaon.Cross(vecPionOne).Mag() / pKaon.Mag();
                     * Double_t pTrelTwo = pKaon.Cross(vecPionTwo).Mag() / pKaon.Mag();
                     *
                     */

                    if (TMath::Min(pTrelOne, pTrelTwo) >= ptrelCut) {
                        Double_t invMass = (firstDaughter->GetFourVector(mPion)
                                            + secondDaughter->GetFourVector(mPion)).M();
                        hKaon->Fill(invMass);
                    }
                } // if #daughters == 2
            } // vertices' properties ok?
        } // loop over vertices

        eventCounter++;
        if (eventCounter % 1000 == 0) {
            cout << "Processing ODS event " << eventCounter << endl;
            // Redraw the canvas every 1000 events
            hKaon->Draw();
            gPad->Update();
        }
    }

    cout << "\n" << eventCounter << " events processed." << endl;

    // Final draw
    hKaon->Draw();
    gPad->Update();

//
// Write histogram to file
//

    TFile file(opts.GetOutput(), "RECREATE");
    hKaon->Write();
    file.Write();
    file.Close();
    cout << "Histograms written to " << opts.GetOutput() << endl;

    return 0;
}
