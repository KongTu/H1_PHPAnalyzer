////////////////////////////////////////////////////////////////
// Program: read_usertree
//
// Authors    : Ursula Berthon
// Created    : 22.06.2001
// Last update: $Date: 2005/10/02 16:59:58 $
//          by: $Author: flucke $
// Description: See option '-h'
//
///////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
// ROOT headers
#include <TLorentzVector.h>
// OO headers
#include "H1Skeleton/H1Tree.h"
#include "H1Pointers/H1ShortPtr.h"
#include "H1Steering/H1StdCmdLine.h"
// this will be auto generated, so it is in H1Examples/:
#include "H1Examples/TLorentzVectorPtr.h"

using namespace std; // to avoid typing 'std::' in front of 'cout', 'endl',...

int main(int argc, char* argv[])
{
  // parse the command line
  H1StdCmdLine opts;
  opts.Parse(&argc, argv);

  if (opts.IsHelp()) {
    cout << "Program to read files created with 'oosubset -f write_usertree.steer',\n"
	 << "showing how to access the data in the user tree.\n"
	 << "As an example it prints out some data for each event.\n\n"
	 << "Usage: read_usertree -f read_usertree.steer [furtheroptions]\n\n"
	 << "available further options:\n"
	 << " --help, -h             this help\n"
	 << " --file, -f <file>      specify the name of further steering files\n"
	 << " --nevents, -n          processes only the given number of events\n"
	 << endl;
    return 0;
  }
  // open gH1Tree:
  H1Tree::Instance()->Open();
  
  // Event counter
  Int_t event = 0;

  H1ShortPtr hatIvTyp("Ivtyp");
  // usual branch pointer even to access user tree variables:
  H1ShortPtr userIvTyp("myIvtyp");
  // ... but UserTree branch pointer have no default branchname:
  // (TLorentzVectorPtr has to be generated, cf. Makefile!)
  TLorentzVectorPtr highPtLorVec("HighestPtTrackLorV");

  cout << setw(4) << "Loop" 
       << setw(8) << "myIvTyp" 
       << setw(9) << "hatIvTyp" 
       << setw(30) << "highest track pt (USER Tree)"
       << endl;

  // loop over events
  while(gH1Tree->Next() && !opts.IsMaxEvent(event + 1)) {
    ++event;
    cout << setw(3) << event 
	 << setw(7) << *userIvTyp 
	 << setw(8) << *hatIvTyp 
	 << setw(20) << highPtLorVec->Pt() 
	 << endl;

  }

  cout << "Read " << event << " events from HAT and USER!" << endl;
  gH1Tree->Close();

  return 0;
}











