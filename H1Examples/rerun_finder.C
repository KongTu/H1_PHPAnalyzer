///////////////////////////////////////////////////////
// This example shows how to rerun finder algorithms.
//
// Author     : Ingo Strauch
// Created    : 2001/12/20
// Rewritten  : 2005/03/15 by Martin Göttlich
// Last update: $Date: 2005/03/17 08:53:04 $
//          by: $Author: goett $
//
///////////////////////////////////////////////////////

// ROOT includes
#include <TBenchmark.h>
#include <TH1.h>
#include <TROOT.h>
#include <stdlib.h>
#include <iomanip>

// H1 OO includes
#include "H1Skeleton/H1Tree.h"
#include "H1Finder/H1FindDstar.h"
#include "H1PartEmFinder/H1CreatePartEm.h"
#include "H1MuonFinder/H1CreateMuon.h"
#include "H1TrkFinder/H1CreateSelTracks.h"
#include "H1Ods/H1OdsEvent.h"
#include "H1Mods/H1ModsEvent.h"
#include "H1Mods/H1PartCand.h"
#include "H1Mods/H1PartCandArrayPtr.h"
#include "H1Mods/H1PartMuon.h"
#include "H1Mods/H1PartMuonArrayPtr.h"
#include "H1Mods/H1PartDstar.h"
#include "H1Mods/H1PartDstarArrayPtr.h"
#include "H1Mods/H1PartSelTrack.h"
#include "H1Mods/H1PartSelTrackArrayPtr.h"
#include "H1HfsFinder/H1HfsCreatePart.h"
#include "H1Mods/H1ExclHfsIterator.h"
#include "H1Mods/H1PartEm.h"
#include "H1Mods/H1PartEmArrayPtr.h"
#include "H1Steering/H1StdCmdLine.h"

using std::cout;
using std::endl;
using std::cerr;
using std::setw;

int main(int argc, char* argv[])
{
    gROOT->SetBatch();
    
    // parse the command line
    H1StdCmdLine opts;
    opts.Parse(&argc, argv);

    // create helper objects    
    TBenchmark *gBenchmark = new TBenchmark();
    gBenchmark->Start("Total execution time");

    // Load mODS/HAT files
    if ( H1Tree::Instance()->Open() < 3 ) { // no files specified or error while opening
        Error("rerun_finder", "Problems in H1Tree::Open(), exiting...");
        return 1;
    }
    
    // Declare counting variables
    Int_t counter = 0;
    Int_t numCand, numTracks, numEm, numMuon, numHFS, numDstar;

    // Make link to branches
    static H1PartCandArrayPtr     cand;
    static H1PartSelTrackArrayPtr tracks;
    static H1PartEmArrayPtr       emparts;
    static H1PartMuonArrayPtr     muons;
    static H1PartDstarArrayPtr    dstars;

    TLorentzVector  elec_vec, new_vec;

    // Loop over events
    while ( H1Tree::Instance()->Next() ) {

        if ( opts.IsMaxEvent(++counter) ) {
          break;
	}

        cout << "========== "
             << setw(6) << counter << ": " 
             << setw(6) << H1Tree::Instance()->GetRunNumber() << " " 
	     << setw(6) << H1Tree::Instance()->GetEventNumber()
             << " =========="<< endl;

        H1ExclHfsIterator hfs; 

        // particle multiplicities 
        numCand   = cand.GetEntries();
        numTracks = tracks.GetEntries();
        numEm     = emparts.GetEntries();
        numMuon   = muons.GetEntries();
        numDstar  = dstars.GetEntries();
        numHFS    = hfs.GetEntries();

        if ( numEm ) {
            elec_vec = emparts[0]->GetFourVector();
        } else {
            elec_vec.SetPxPyPzE(0,0,0,0);
        }
        
        cout << "Original mods:"<<endl;
        cout << " NumCand    = " << setw(3) <<numCand
             << " NumTracks  = " << setw(3) <<numTracks
             << " NumEm      = " << setw(3) <<numEm
             << " NumMuon    = " << setw(3) <<numMuon
             << " NumHFS     = " << setw(3) <<numHFS
             << " NumDstar   = " << setw(3) <<numDstar
             << endl;

        H1ModsEvent *modsEvent = static_cast<H1ModsEvent*>( H1Tree::Instance()->GetEvent(H1Tree::kModsTree) );
	// delete the original modsevent
	modsEvent->Delete();

        H1ExclHfsIterator hfs1; 
        numCand   = cand.GetEntries();
        numTracks = tracks.GetEntries();
        numEm     = emparts.GetEntries();
        numMuon   = muons.GetEntries();
        numDstar  = dstars.GetEntries();
        numHFS    = hfs1.GetEntries();

        cout << "After calling Delete():"<<endl;
        cout << " NumCand    = " << setw(3) <<numCand
             << " NumTracks  = " << setw(3) <<numTracks
             << " NumEm      = " << setw(3) <<numEm
             << " NumMuon    = " << setw(3) <<numMuon
             << " NumHFS     = " << setw(3) <<numHFS
             << " NumDstar   = " << setw(3) <<numDstar
             << endl;


	// get tracks and clusters fom ODS
        H1OdsEvent *odsEvent = static_cast<H1OdsEvent*>( gH1Tree->GetEvent(H1Tree::kOdsTree) );
        H1TrackEvent   *trackEvent   = odsEvent->GetTrackEvent();
        H1ClusterEvent *clusterEvent = odsEvent->GetClusterEvent();

        gH1Tree->LoadAllBranches(H1Tree::kOdsTree); // important!


	// Running finders. The order is important. You can run any other finder you like.

        // lee west track selection
        static H1CreateSelTracks trackselection;
        trackselection.Find(trackEvent, clusterEvent, modsEvent);
   
        // run electron finder
        static H1CreatePartEm elecfinder;
        elecfinder.Find(trackEvent, clusterEvent, modsEvent);
   
        // run muon finder
        static H1CreateMuon muonfinder;
        muonfinder.Find(trackEvent, clusterEvent, modsEvent);
   
        // run HFS finder
        static H1HfsCreatePart hfsfinder;
        hfsfinder.Find(trackEvent, clusterEvent, modsEvent);

	// run D* finder
        TClonesArray* tr = modsEvent->GetArray(H1PartSelTrack::Class());
        TClonesArray* ds = modsEvent->GetArray(H1PartDstar::Class());
        H1FindDstar dstarFinder;
        dstarFinder.Find(tr, ds);


        // particle multiplicities 
        numCand   = cand.GetEntries();
        numTracks = tracks.GetEntries();
        numEm     = emparts.GetEntries();
        numMuon   = muons.GetEntries();
        numDstar  = dstars.GetEntries();
        H1ExclHfsIterator hfs2; 
        numHFS    = hfs2.GetEntries();
      
        cout << "After rerunning finders:"<<endl;
        cout << " NumCand    = " << setw(3) <<numCand
             << " NumTracks  = " << setw(3) <<numTracks
             << " NumEm      = " << setw(3) <<numEm
             << " NumMuon    = " << setw(3) <<numMuon
             << " NumHFS     = " << setw(3) <<numHFS
             << " NumDstar   = " << setw(3) <<numDstar
             << endl;
        
    }// event loop
   
    // show benchmark numbers
    gBenchmark->Show("Total execution time");
    
    Float_t cpu  = gBenchmark->GetCpuTime("Total execution time");
    Float_t real = gBenchmark->GetRealTime("Total execution time");
    
    printf("                    : Ev./real  = %6.2f         Ev./CPU  = %6.2f\n"
           ,counter/real,counter/cpu);
    
//
// Cleaning Up
//
    cout << "\n\nCleaning up...\n";
    delete gBenchmark;
    
    cout << "Deleted all objects\n\n";


    return EXIT_SUCCESS;
}
