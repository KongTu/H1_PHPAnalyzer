////////////////////////////////////////////////////
//
// example program for lumi treatment,
// loading a SelectedRuns file, 
// printing out the total integrated luminosity
// binning the runperiod in bins of 500nb^-1
// calculating trigger prescales for a combination
// of sub-triggers s0 and s3
// and saving the polarisation histogram to pola.eps
// (if available)
//
// NOTE: this example program needs access to a .root file
//       containing a run-list;
//       to produce this file do: 
//         
//         oolumi -f oolumi_lumi.steer
//
// Author     : Christian Veelken
// Created    : 2004/03/10
// Last update: $Date: 2008/01/29 15:45:13 $ (UTC)
//          by: $Author: ozerov $
//
// for help do: lumicalc -h
//
////////////////////////////////////////////////////

#ifndef _LUMI_C
#define _LUMI_C

#include <iostream>

#include "TROOT.h"
#include "TFile.h"
#include "TArrayI.h"
#include "TArrayF.h"
#include "TObjArray.h"
#include "TString.h"
#include "TH1F.h"
#include "TCanvas.h"

#include "H1Tools/H1RunList.h"
#include "H1Tools/H1TrigPrescaleWeight.h"

#include "H1Steering/H1SteerManager.h"
#include "H1Steering/H1StdCmdLine.h"
#include "H1Steering/H1OptionString.h"
#include "H1Steering/H1OptionBool.h"

using namespace std;

int main(int argc, char **argv)
{
    gROOT->SetBatch();
    gROOT->SetStyle("Plain");

//--- parse the command line 

  H1StdCmdLine opts;
  opts.PrintOptions(); // will print all accepted options to STDOUT
  opts.Parse(&argc,argv);

  // index of the first of the remaining arguments
  Int_t optind = opts.GetOptionIndex();
        
  if ( opts.IsHelp() ) { 
    // print help message and exit
    cout << "\nexample program for lumi treatment. \n"
	 << "Usage: lumi [options] [SelectedRuns.root] \n\n"
	 << "available options:\n"
	 << " --help, -h     this help\n"
	 << " --version, -V  prints version\n"
	 << endl;
    cout << "NOTE: this example needs access to a SelectedRuns.root file containing a run-list;\n" 
	 << "      to produce this file, do \n" 
	 << "      oolumi -f oolumi_lumi.steer\n"
	 << endl;
    return 0;
  }

  if ( opts.IsVersion() ){
    cout << "$Revision: 1.5 $" << endl;
    //return 0;
  }
  
  gROOT->SetBatch();
  
//--- retrieve run-list (for run-lumi information)

  TString lumiFile;

  if ( argv[optind] ) { 
    lumiFile = argv[optind];
  } else {
    lumiFile = "SelectedRuns.root";
  }      

  cout << "Reading lumi file " << lumiFile << "..." << endl;

  TFile* inputFile = new TFile(lumiFile.Data());
  if ( !inputFile->IsOpen() ){
    cerr << "Error: Could not open file " << lumiFile << " !" << endl;
    exit(1);
  }
    
  TObject* object = inputFile->Get("H1RunList");
  if ( !(object != 0 && object->InheritsFrom(H1RunList::Class())) ){
    cerr << "Error: Could not retrieve H1RunList object from file " << lumiFile << " !" << endl;
    exit(1);
  }
     
  H1RunList* runList = (H1RunList*)object;
    
  cout << "first run = " << runList->FirstRun() 
       << ", last run = " << runList->LastRun() 
       << ", integrated luminosity = " << runList->GetIntLumi() << " nb^-1" << endl;

  cout << endl << endl;

//--- bin run-range in sub periods of 10pb^-1 integrated luminosity

  cout << "run-range divided into periods of (approximately) 10pb^-1 luminosity:" << endl;

  H1ArrayI runPeriods     = runList->GetRunRangeDivision(10000); // input expected in nb^-1 
  H1ArrayF runPeriodLumis = runList->GetIntLumi(runPeriods);

  for ( Int_t iperiod = 0; iperiod <= runPeriodLumis.GetLast(); iperiod++ ){
    cout << " " << runPeriods[iperiod] << " - " << runPeriods[iperiod + 1] - 1 << " : " << runPeriodLumis[iperiod] << " nb^-1" << endl;
  }
  
  cout << endl << endl;

//--- calculate trigger-prescale weights for combination of sub-triggers s0 and s3

  cout << "Calculating trigger-prescale weights..." << endl;

  runList->GetTrigPrescaleWeight()->UseSubTrigger(0);
  runList->GetTrigPrescaleWeight()->UseSubTrigger(3);
  
  cout << "prescale weights for s0 and s3 in run-range " << runList->FirstRun() << " - " << runList->LastRun() << ":" << endl;

  //
  // input to the trigger-prescale calculation are raw L1 sub-trigger
  //   (Il1rw on HAT)
  // in case you use L1 sub-trigger which use L2 verification
  //   (or you are not sure about L2 verification)
  // please read the H1-internal note H1-IN-517(04/1997)
  // 
  // you can check the L1-L2 trigger-setup at http://www-h1.desy.de/icgi-trigger/l1trignew.cgi,
  // enter a run-number from your run-range and selected trigger logic level CTL1&CTL2
  //

  TArrayI stPattern(128);
  H1TrigPrescaleWeight* prescaleWeight = runList->GetTrigPrescaleWeight();
  stPattern[0] = 0; stPattern[3] = 0; cout << " s0 = OFF, s3 = OFF : " << prescaleWeight->GetPrescaleWeight(stPattern) << endl;
  stPattern[0] = 0; stPattern[3] = 1; cout << " s0 = OFF, s3 =  ON : " << prescaleWeight->GetPrescaleWeight(stPattern) << endl;
  stPattern[0] = 1; stPattern[3] = 0; cout << " s0 =  ON, s3 = OFF : " << prescaleWeight->GetPrescaleWeight(stPattern) << endl;
  stPattern[0] = 1; stPattern[3] = 1; cout << " s0 =  ON, s3 =  ON : " << prescaleWeight->GetPrescaleWeight(stPattern) << endl;
    
  cout << endl << endl;

//--- if available: save polarisation histo to an eps file

  TH1F* pola = static_cast<TH1F*>(inputFile->Get("pola"));
  if ( !pola ){
    cerr << "No polarisation histogram found in " << lumiFile << " !" << endl;
  } else {

    cout << "Lumiweighted average polarisation: " << pola->GetMean() << "%" << endl;

    pola->SetTitle(Form("Polarisation Profile Runs %d - %d",runList->FirstRun(),runList->LastRun()));
        

    Int_t nzero=pola->GetXaxis()->FindBin(0.);
    pola->SetBinContent(nzero,0.);
    pola->GetXaxis()->SetRangeUser(-100,100);
   
    pola->Rebin(pola->GetNbinsX()/40);
    pola->Scale(1./pola->Integral());
    pola->GetXaxis()->SetTitle("%");
   
    pola->Draw();  
    gPad->Print("pola.eps");
  }
  
//--- do some clean-up

  delete inputFile;

//--- done.
  
  return 0;       
}

#endif
