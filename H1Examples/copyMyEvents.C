///////////////////////////////////////////////////////
// Select events with a D* and a muon of the same charge
// and copy the events to a local HAT and MODS file
//
// Author     : Thomas Kuhr
// Created    : 14.10.2002
// last update: $Date: 2003/09/22 14:58:47 $
// by         : $Author: wessling $
//
///////////////////////////////////////////////////////
#include <TROOT.h>
#include "H1Skeleton/H1Tree.h"
#include "H1Mods/H1PartDstar.h"
#include "H1Mods/H1PartDstarArrayPtr.h"
#include "H1Mods/H1PartMuon.h"
#include "H1Mods/H1PartMuonArrayPtr.h"
#include "H1Steering/H1StdCmdLine.h"



int main(int argc, char* argv[])
{
  // parse the command line
  H1StdCmdLine opts;
  opts.Parse(&argc, argv);

  H1Tree* tree = H1Tree::Instance();
  tree->Open();

  if (!tree->CopyTree("HAT", "myHAT.root") ||      // prepare to copy HAT
      !tree->CopyTree("MODS", "myMODS.root")) {    // and MODS tree
    printf("Error: could not copy HAT and MODS tree!\n");
    return 1;
  }

  printf("\nselecting events with a D* and a muon...\n");
  tree->SelectHat("NumDstar>0 && NumMuons>0");  // select events with a D* and a muon
  printf("%d events selected on HAT\n", tree->GetEntries());

  printf("\nselecting events with a D* and a muon of same charge...\n");
  static H1PartDstarArrayPtr dstars;
  static H1PartMuonArrayPtr muons;
  Int_t nSelected = 0;

  while (tree->Next()) {          // loop over events, D* candidates and muons
    Bool_t selectEvent = false;
    for (Int_t iDstar = 0; iDstar < dstars.GetEntries(); iDstar++) {
      for (Int_t iMuon = 0; iMuon < muons.GetEntries(); iMuon++) {
	// select events with a D* and a muon of same charge
	if (dstars[iDstar]->GetCharge() * muons[iMuon]->GetCharge() > 0) selectEvent = true;
	if (selectEvent) break;
      }
      if (selectEvent) break;
    }
    if (selectEvent) {
      tree->CopyEvent();  // copy the event if it was selected
      nSelected++;
    }
  }
  printf("%d events selected on MODS and copied\n\n", nSelected);

  tree->Close();   // close the copied files
}
