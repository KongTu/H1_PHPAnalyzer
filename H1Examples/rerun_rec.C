///////////////////////////////////////////////////////
//  Rerun H1REC module on POT events
//
// Author     : Ingo Strauch (strauch@mail.desy.de)
// Created    : 19.12.2000
// Redone by  : Gero Flucke (gero.flucke@desy.de)
//          and Bengt Wessling (bengt.wessling@desy.de)
// Last update: $Date: 2006/09/22 09:00:40 $
//          by: $Author: ozerov $
//
///////////////////////////////////////////////////////

#include <iostream>
#include <stdlib.h>

// ROOT includes
//#include <TFile.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TStyle.h>
// optional includes for histogramming and such
//#include <TH1.h>
//#include <TF1.h>
//#include <TCanvas.h>
//#include <TGraphErrors.h>
//#include <TMath.h>

// includes for fortran calls
#include "H1Wrappers/bos.h"
#include "H1Wrappers/fpack.h"
#include "H1Wrappers/h1rec.h"

// H1 OO includes
#include "H1Skeleton/H1Tree.h"
#include "H1Steering/H1StdCmdLine.h"

// access to mODS
//#include "H1Mods/H1PartSelTrack.h"
//#include "H1Mods/H1PartSelTrackArrayPtr.h"

// access to banks (generic access)
#include "H1OOBanks/H1BankPtr.h"
#include "H1OOBanks/H1Row.h"

using namespace std;
using std::ios;
using std::cout;

BCS_DEF BCS;                        // load the bos common

int
main(int argc, char* argv[])
{
    // sync fortran output, printf and cout
    ios::sync_with_stdio();
    // parse the command line
    H1StdCmdLine opts;
    opts.Parse(&argc, argv);
    // needed for graphic, but MUST be AFTER Parse(...):
    TApplication theApp("rerun_rec", &argc, argv);

    gROOT->SetStyle("Plain");
    gStyle->SetHistLineWidth(2);

    // Load mODS/HAT files
    H1Tree::Instance()->Open();            // this statement must be there!

    // Optionally run hat-selection
    // cout << H1Tree::Instance()->SelectHat("Q2e>1")
    //      << " events selected " << endl;

    // Optionally create branch pointer to branch (for mODS selection)
    // static H1PartSelTrackArrayPtr seltracks;

    // optionally activate FPACK printout
    // H1BosFpackInterface::Instance()->fparm("PRINT 1");
    // H1BosFpackInterface::Instance()->fparm("IOSTATISTICS");
		    		    
    // Loop over events
    Int_t eventCounter = 0;
    while (H1Tree::Instance()->Next() && (!opts.IsMaxEvent(eventCounter)) ) {
        eventCounter++;
	
	cout << "New Event: ================" <<endl;
        cout << H1Tree::Instance()->GetRunNumber() << " " <<
		H1Tree::Instance()->GetEventNumber() << endl;
  
	// access DST from mods/ods or POT from dq-hat	
/*	H1Tree::Instance()->LoadFpackEvent() ;
        printf("DST:  Run %-7d  Event %d\n", 
	   H1FpackIndexManager::GetBosRunNumber(), 
	   H1FpackIndexManager::GetBosEventNumber());      
        printf("DtraPtr.GetEntries() = %d\n", DtraPtr.GetEntries());
*/

	// access POT from mods/ods	
	H1Tree::Instance()->LoadOriginalFpackEvent() ;
        printf("POT:  Run %-7d  Event %d\n", 
	   H1FpackIndexManager::GetBosRunNumber(), 
	   H1FpackIndexManager::GetBosEventNumber());      
        	
        static H1BankPtr IrwePtr(0,"IRWE");    
        static H1BankPtr IwcrPtr(0,"IWCR");    
	// rerun iron reconstruction if there are iron hits
	// irweptr needs to be dereferenced once in order for it
	// to work. ....
	if (IrwePtr.GetEntries()) ITREC();

	// if successful IWCR should have been created in itrec
	if (IrwePtr.GetEntries()!=IwcrPtr.GetEntries()) {
	    cout << "Warning: Different length of IWCR and IRWE" << endl;
        }
	else { 
	    cout << "IrwePtr.GetEntries()=IwcrPtr.GetEntries()" << 
	             IrwePtr.GetEntries() << endl;
	    cout << " Entry, IRWE, Module , Layer , x , y , z" << endl;
 	    for (Int_t i=0; i<IrwePtr.GetEntries(); i++) {
      	    cout << " " << i << " " 
			<< IrwePtr[i]->GetInt(1) << " " 
			<< IwcrPtr[i]->GetInt(0) << " "
			<< IwcrPtr[i]->GetInt(1) << " "
			<< IwcrPtr[i]->GetFloat(2) << " "
			<< IwcrPtr[i]->GetFloat(3) << " "
			<< IwcrPtr[i]->GetFloat(4) << endl;
            }
        }
	
    }

    return 0;
}
