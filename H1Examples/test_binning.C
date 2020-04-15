#include <iostream>
#include <TROOT.h>
#include <TApplication.h>
#include "H1Skeleton/H1Tree.h"
#include "H1Skeleton/H1SteerTree.h"
#include "H1Steering/H1SteerManager.h"
#include "H1Arrays/H1ArrayD.h"
#include "H1Arrays/H1ArrayI.h"
#include "H1Binning/H1LinearBinGrid.h"
#include "H1Binning/H1BinCentreGrid.h"
#include "H1Binning/H1DataBinner.h"
#include "H1Binning/H1ModelBinner.h"
#include "H1Binning/H1BinSelResults.h"
#include "H1Pointers/H1FloatPtr.h"
#include "H1Binning/H1SimpleBinMeas.h"
#include "H1Binning/H1CompositeBinMeas.h"
#include "H1Benchmarks/H1Benchmark.h"
#include "H1Binning/H1PlottableBinMeas.h"
#include "H1Binning/H1MultiPlottableBinMeas.h"
#include "H1Binning/H1StampPlotter.h"
#include <TSystem.h>
#include <TCanvas.h>
#include "H1Calculator/H1CalcSystematic.h"
#include "H1Calculator/H1Calculator.h"

//______________________________________________________________________
//
// Authors    : Paul Laycock
// Created    : 2004/01/07
// Last update: $Date: 2008/04/29 11:28:18 $ (UTC)
//          by: $Author: laycock $
//
//*******************************************************
//
// Classes to do N dimensional binning
// H1LinearBinGrid             - define N sets of bin edges
//                             - returns a 1 dimensional bin number given N values
// H1SimpleBinMeas             - Base class is H1BinMeas
//                             - Constructed using an H1BinGrid
//                             - Stores an array of numbers (e.g. number of reconstructed events)
//                             - and the error on that number
// H1DataBinner                - Bins a Data sample (i.e. just reconstructed level information)
//                             - Produces an H1SimpleBinMeas
// H1ModelBinner               - Bins a monte carlo which models the signal
//                             - Produces H1SimpleBinMeas of the uncorrelated variables used to
//                             - calculate Acceptance, Purity and Stability and the correct errors
//                             - Base class H1Binner stores the algorithms
// H1BinSelResults          - A Container class
//                             - Stores the information needed for binning an event :
//                             - Generator and Reconstructed level variables
//                             - Flags to say whether the event passed the cuts
//                             - (One for generator level, another for reconstructed level)
// H1PlottableBinMeas          - A graphical wrapper for the H1SimpleBinMeas class
//
// H1MultiPlottableBinMeas     - Container for all of the H1PlottableBinMeas
//                             - that you want to plot.
// H1StampPlotter              - Produces the final stampplot of your results
//______________________________________________________________________


// Initialize the root framework
TROOT h1tree("h1tree","Test H1Tree");


using namespace std;

int main(int argc, char** argv)
{

  TApplication theApp("App",&argc,argv);

  // ***********************************************************
  // First of all we need to setup some objects
  // ***********************************************************

  // Set up a bin grid, e.g here in Q2 and x

  H1BinGrid* TheBinGrid = new H1LinearBinGrid("TheBinGrid");
  
  Double_t q2bins[] = {2.12, 2.45, 2.75, 3.05, 3.35};
  
  Double_t xbins[] = {-1.6, -1.4, -1.2, -1.0, -0.8386, -0.68, -0.5, -0.3, 0.00};
  
  H1ArrayD Q2bins(5, q2bins);
  H1ArrayD Xbins(9, xbins);
  
  // Bin in Q2 and X (in that order)  
  TheBinGrid->AddBinEdges(Q2bins);
  TheBinGrid->AddBinEdges(Xbins);

  // A SelectionResults object to store the info needed by the Binner objects
  H1BinSelResults* TheResults = new H1BinSelResults();

  // and arrays for the Reconstructed level information and the Generator level information
  H1ArrayD TheRecInfo(2);
  TheRecInfo.SetLast(1);

  H1ArrayD TheGenInfo(2);
  TheGenInfo.SetLast(1);

  // benchmarking to give feedback during the eventloop

  H1Benchmark* TheBenchmark = new H1Benchmark();  
  TheBenchmark->SetInterval(10000);
  TheBenchmark->SetNEvents(1000000);

  // HAT pointers to the variables that you will need

  H1FloatPtr Weight("Weight1");
  H1FloatPtr Q2("Q2e");
  H1FloatPtr Y("Ye");
  H1FloatPtr Q2Gen("Q2eGen");
  H1FloatPtr YGen("YeGen");
  Float_t s = 4 * 920.0 * 27.6; // The Mandelstam variable (centre of mass energy)

  // ***********************************************************
  // General setup completed, now move on to the event loops ...
  // ***********************************************************

  // ***********************************************************
  // Run over some data
  // ***********************************************************

  cout << "Running over some data ..." << endl;

  // Use the H1SteerManager with the H1SteerTree class to take care of file handling

  H1SteerManager theSteering;
  theSteering.ParseFile("test_binning.steer");

  H1Tree* TheTree = H1Tree::Instance();
  H1SteerTree* SteerTree = (H1SteerTree*)(gH1SteerManager->GetSteer(H1SteerTree::Class(), "Data"));
  TheTree->AddFilesFromSteer(SteerTree);
  TheTree->Open();
  TheTree->SelectAll();
  TheTree->Reset();
  
  // a rather rough high q2 selection ...

  Int_t selectedEvents =
    TheTree->SelectHat("(ITrigac[67] || ITrigac[71] || ITrigac[75] || ITrigac[77]) && Ivtyp>=1 && abs(VtxZ) < 35.0 && ElecE > 11.0 && Q2e > 133.0 && Ye < 0.63 && Epz > 35.0");
  cout << "-> Number of selected events = " << selectedEvents << endl;
  
  // check what the event sample looks like ...

  TheTree->DrawHat("ElecE", "ElecE < 50.0");
  gPad->Update();

  // Make a DataBinner object to bin your data

  H1Binner* TheBinner = new H1DataBinner(TheBinGrid, "DATA");

  // Now the event loop ...

  while(TheTree->Next() && TheBenchmark->Next()){
    
    TheResults->Reset();
    
    if(true){ // You would put your reconstructed level cuts in here
      TheResults->SetRecCutsFlag(true);
    }

    // read the values of Q2 and x
    TheRecInfo[0] = *Q2;
    TheRecInfo[1] = (*Q2/(*Y * s));
    TheResults->SetRecInfo(TheRecInfo);

    // read the event weights
    TheResults->SetWeight(*Weight);
    TheResults->SetWeightGen(*Weight);

    // Bin the results
    TheBinner->Bin(TheResults);
  }

  // End of the event loop, you can now retrieve your binned measurement

  H1BinMeas* TheData = TheBinner->GetBinMeas();

  // ***********************************************************
  // Run over some Monte Carlo
  // ***********************************************************

  cout << "Finished binning data, now move onto the Monte Carlo model" << endl;

  delete TheTree;

  TheTree = H1Tree::Instance();
  SteerTree = (H1SteerTree*)(gH1SteerManager->GetSteer(H1SteerTree::Class(), "Model"));
  TheTree->AddFilesFromSteer(SteerTree);
  TheTree->Open();
  TheTree->SelectAll();
  TheTree->Reset();

  // use an H1ModelBinner for model MC

  // use systematics

  H1ArrayI sysNum;
  sysNum += (H1CalcSystematic::eElecEn);
  sysNum += (H1CalcSystematic::eElecTh);
  sysNum += (H1CalcSystematic::eHadEn);
  Int_t numLoops = (sysNum.GetEntries() * 2) + 1; // shift up, shift down and no shifts

  TheBinner = new H1ModelBinner(TheBinGrid, "MODEL");
  ((H1ModelBinner*)TheBinner)->AddSystematic(TheBinGrid, "eElecEn");
  ((H1ModelBinner*)TheBinner)->AddSystematic(TheBinGrid, "eElecTh");
  ((H1ModelBinner*)TheBinner)->AddSystematic(TheBinGrid, "eHadEn");
  
  // Never ever use SelectHat to put a preselection on your model !

  // Reset the event counter and run over your MC

  TheBenchmark->ResetEventNum();
  while(TheTree->Next() && TheBenchmark->Next()){
    
    TheResults->Reset();
        
    for(Int_t i=0; i < numLoops; ++i){// loop over MC several times for systematic studies

      if(true){ // you may have cuts at the generator level for mixing monte carlos
	
	// Reset all calculator quantities and shifts and TheResults
	H1Calculator::Instance()->Reset();
	H1Calculator::Instance()->Systematic()->ResetAllShifts();
	if(TheResults) TheResults->Reset();
	
	// Perform required shift
	if(i != 0){
	  Int_t sys = abs((i-1)/2);
	  if((i%2)==0) H1Calculator::Instance()->Systematic()->ShiftDo(sysNum[sys]);
	  else H1Calculator::Instance()->Systematic()->ShiftUp(sysNum[sys]);
	}
	// ---------------------------------------------------------------------
	
	if(true){ // you would put your reconstructed level cuts in here
	  TheResults->SetRecCutsFlag(true);	  
	}
	if(true){ // you would put your generator level cuts in here
	  TheResults->SetGenCutsFlag(true);	
	}
	
	// read both rec and gen values of Q2 and x
	TheRecInfo[0] = *Q2;
	TheRecInfo[1] = (*Q2/(*Y * s));
	if(i > 0){
	  //simulate systematic shifts!
	  // If you use the H1Calculator then you simply get the variable as usual
	  // any systematic shifts are taken care of by H1Calculator

	  if((i%2)==0){
	    TheRecInfo[0] *= 0.98;
	    TheRecInfo[1] *= 0.98;
	  } else {
	    TheRecInfo[0] *=1.02;
	    TheRecInfo[1] *=1.02;
	  }
	}
	TheResults->SetRecInfo(TheRecInfo);
	
	TheGenInfo[0] = *Q2Gen;
	TheGenInfo[1] = (*Q2Gen/(*YGen * s));
	TheResults->SetGenInfo(TheGenInfo);
	
	// read the event weights
	TheResults->SetWeight(*Weight);
	TheResults->SetWeightGen(*Weight);
	
	// Bin the results      
	TheBinner->Bin(TheResults, i);
      }
    }
  }

  // End of the event loop, you can now retrieve your binned measurement

  H1BinMeas* TheModel = TheBinner->GetBinMeas();

  // The model binner produces a composite measurement
  // among other things this contains the calculated acceptance

  H1BinMeas* TheAcceptance = TheModel->Get("ACCEPTANCE");

  // ***********************************************************
  // A quick cross-section calculation ....
  // ***********************************************************
  // We will forget about bin centre corrections and radiative corrections and the like ...

  // You would retrieve the luminosity of your data from the oolumi tool
  Float_t Luminosity = 47.7305;

  H1SimpleBinMeas Data(*(H1SimpleBinMeas*)TheData);
  H1SimpleBinMeas Acceptance(*(H1SimpleBinMeas*)TheAcceptance);

  H1SimpleBinMeas CrossSection = Data/(Acceptance*Luminosity);

  // note that you would have binned a background monte carlo exactly like the data
  // and then just subtracted it from the data, i.e.
  // H1SimpleBinMeas CrossSection = (Data - Background)/(Acceptance*Luminosity);

  // If you want to look at the numerical values, look in the text files
  // You need to create the subdirectory "Text"

  CrossSection.SetNameTitle("CrossSection", "");
  Data.SetNameTitle("Data", "");
  Acceptance.SetNameTitle("Acceptance", "");

  cout << "Create the directory Text if you want to see the following files" << endl;

  cout << "Am I changing anything?" << endl;

  CrossSection.PrintToFile("Text/", -1);
  Data.PrintToFile("Text/", -1);
  Acceptance.PrintToFile("Text/", -1);

  // look at systematics
  cout << "====================================================" << endl;
  // first of all look at the fractional systematics, bin by bin from each source
  TObjArray* fracSystematics = ((H1CompositeBinMeas*)(TheModel))->GetFractionalSystematics();
  for(int i = 0; i < fracSystematics->GetEntries(); i++){
    
    H1SimpleBinMeas* sys = ((H1SimpleBinMeas*)(fracSystematics->At(i)));
    sys->Print();
    sys->PrintToFile("Text/", -1);
  }
  cout << "====================================================" << endl;
  
  // now get the total systematic error, the sum of the above sources added in quadrature
  H1SimpleBinMeas* fullsys = ((H1CompositeBinMeas*)(TheModel))->GetFinalSystematic();
  fullsys->Print();
  fullsys->PrintToFile("Text/", -1);
  cout << "====================================================" << endl;
  
  // now look at the total systematic error (with statistical error on MC included)
  // you can see you would probably like some more MC statistics!
  H1SimpleBinMeas* finalerror = ((H1CompositeBinMeas*)(TheModel))->GetFinalError();
  finalerror->Print();
  cout << "====================================================" << endl;
  finalerror->PrintToFile("Text/", -1);

  // ===========================================================
  // Making the cross-section plottable
  // ===========================================================

  // Define the bin centres with an H1BinCentreGrid

  H1BinCentreGrid* TheBinCentreGrid = new H1BinCentreGrid("BinCentreGrid");
  Double_t q2cbins[] = {200.0, 400.0, 800.0, 1600.0};
  
  Double_t xcbins[] = {0.032, 0.05, 0.08, 0.13, 0.18, 0.25, 0.4, 0.65};
  
  H1ArrayD Q2cbins(4, q2cbins);
  H1ArrayD Xcbins(8, xcbins);

  TheBinCentreGrid->AddBinEdges(Q2cbins, false);
  TheBinCentreGrid->AddBinEdges(Xcbins, false);

  // Make a new measurement using the H1BinCentreGrid

  H1SimpleBinMeas* TheBinCentredXS = new H1SimpleBinMeas(TheBinCentreGrid, "BinCentredXS");
  // whose contents are the cross-section
  TheBinCentredXS->Add(&CrossSection);
  TheBinCentredXS->PrintToFile("Text/", -1);

  // Make the measurement plottable

  H1PlottableBinMeas* ThePlottableXS = new H1PlottableBinMeas(TheBinCentredXS);

  // set some attributes

  ThePlottableXS->NameDimension(0, "Q^{2}");
  ThePlottableXS->NameDimension(1, "x");
  ThePlottableXS->SetMarkerSize(0.6);
  ThePlottableXS->SetMarkerStyle(20);
  ThePlottableXS->SetMarkerColor(4);
  ThePlottableXS->SetFillColor(4);
  ThePlottableXS->SetLineWidth(1);

  // maybe you would like to plot x vs Q2 instead, we'll stick to the default

  H1ArrayI order(2);
  order.SetLast(1);
  order.AddAt(0,  0);
  order.AddAt(1,  1);

  ThePlottableXS->ReOrder(order);

  // Use exponents (base 10) instead of values for the axes

  ThePlottableXS->UseExponents(true);

  // Make an H1MultiPlottableBinMeas

  H1MultiPlottableBinMeas* TheMulti = new H1MultiPlottableBinMeas(ThePlottableXS, "H1 Data 9900");
  // Set the y axis range

  TheMulti->SetMinimum(0.0);
  TheMulti->SetMaximum(250.0);

  // you add theory and systematics to the same multiplottable add
  // theory using e.g. multi->Add(plottabletheory, "Theory"); Use the
  // final error as calculated by the ModelBinner it's the total
  // FRACTIONAL error, so we have to multiply it by the cross section
  // in addition we want to add the statistical error on the cross
  // section to get the total error on the cross section as the outer
  // error bar

  H1SimpleBinMeas* TheTotalError = new H1SimpleBinMeas(TheBinCentreGrid, "TheTotalError");
  TheTotalError->SetNameTitle("TheTotalError", "");
  TheTotalError->Add(TheBinCentredXS);
  Double_t xsec = 0.0;
  Double_t staterror = 0.0;
  Double_t toterror = 0.0;
  for(int i = 0; i < TheTotalError->GetEntries(); i++){
    xsec = TheTotalError->GetValue(i);
    staterror = (TheTotalError->GetValue(i)) ? TheTotalError->GetError(i) / TheTotalError->GetValue(i) : 0.0;
    toterror = (finalerror->GetValue(i) * finalerror->GetValue(i)) + (staterror*staterror);
    toterror = sqrt(toterror);
    TheTotalError->SetValueError(xsec, (xsec*toterror), i);
  }
  TheTotalError->PrintToFile("Text/", -1);

  // Make the systematic measurement plottable

  H1PlottableBinMeas* ThePlottableSys = new H1PlottableBinMeas(TheTotalError);
  ThePlottableSys->NameDimension(0, "Q^{2}");
  ThePlottableSys->NameDimension(1, "x");
  ThePlottableSys->SetMarkerSize(0.6);
  ThePlottableSys->SetMarkerStyle(20);
  ThePlottableSys->SetMarkerColor(4);
  ThePlottableSys->SetFillColor(4);
  ThePlottableSys->SetLineWidth(1);
  ThePlottableXS->ReOrder(order);
  ThePlottableXS->UseExponents(true);

  // and add it to the multiplottable

  TheMulti->Add(ThePlottableSys, "NoEntry");

  // The name "NoEntry" means you don't get an extra entry in the key

  // ===========================================================
  // Making the Stamp plot of the cross-section
  // ===========================================================

  // Make an H1StampPlotter with your measurements

  H1StampPlotter* TheStampPlotter = new H1StampPlotter(TheMulti);

  // setup the titles
  TheStampPlotter->SetMainTitle("The High Q^{2} Neutral Current Cross-Section");
  TheStampPlotter->SetXAxisTitle("log_{10}Q^{2}");
  TheStampPlotter->SetYAxisTitle("NC Cross-Section (pb)");

  // Set the output filename
  TheStampPlotter->SetFileName("StampPlot.eps");

  // setup the plot e.g. to eliminate empty bins

  // use this function to work out the total number of bins that can be plotted :
  Int_t nbins = ThePlottableXS->GetNBins();
  cout << "Total number of bins to plot = " << nbins << endl;

  // Here plot all available bins, there is only one page in this example

  H1ArrayI page1;
  for(int i = 0; i < 8; i++){page1 += i;}

  TObjArray* pageNumbers = new TObjArray();
  pageNumbers->Add(&page1);
  pageNumbers->Compress();

  TheStampPlotter->SetupPages(pageNumbers);

  // you don't need to watch Root do the plotting

  gROOT->SetBatch(kTRUE);

  // finally make the stamp plot

  TheStampPlotter->CreateStampPlot();

  gSystem->Exit(0);

  theApp.Run(kTRUE);

  return 0;
}
